#include "pch.h" // 如果你的项目没有使用预编译头，请注释掉这一行，或者保留它（MFC项目默认通常需要）
#include "TaskScheduler.h"

// 初始化静态成员
TaskScheduler* TaskScheduler::instance = nullptr;

// 构造函数：初始化日志记录器和停止标志
// 注意：这里的日志文件名 "scheduler_log.txt" 会生成在程序运行目录下
TaskScheduler::TaskScheduler() : logger("scheduler_log.txt"), stopScheduler(false) {
    // ▼▼▼ 初始化监控标志 ▼▼▼
    stopMonitor = false;
    isTaskRunning = false;
    taskStartTime = 0;
}


void TaskScheduler::AttachObserver(IObserver* observer) {
    std::lock_guard<std::mutex> lock(observerMutex);
    observers.push_back(observer);
}

void TaskScheduler::DetachObserver(IObserver* observer) {
    std::lock_guard<std::mutex> lock(observerMutex);
    // 简单的移除逻辑
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}
void TaskScheduler::NotifyObservers(const std::string& msg) {
    std::lock_guard<std::mutex> lock(observerMutex);
    for (auto obs : observers) {
        if (obs) obs->OnLogUpdate(msg);
    }
}
// 获取单例实例
TaskScheduler* TaskScheduler::GetInstance() {
    if (instance == nullptr) {
        instance = new TaskScheduler();
    }
    return instance;
}

// 启动调度器
void TaskScheduler::Start() {
    if (stopScheduler) {
        stopScheduler = false;
    }
    // 启动后台线程，执行 WorkerLoop
    if (!workerThread.joinable()) {
        workerThread = std::thread(&TaskScheduler::WorkerLoop, this);
        logger.Write("[System] Scheduler Started.");
    }
    if (!monitorThread.joinable()) {
        stopMonitor = false;
        monitorThread = std::thread(&TaskScheduler::MonitorLoop, this);
        logger.Write("[System] Watchdog Monitor Started.");
    }
}

// 停止调度器
void TaskScheduler::Stop() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stopScheduler = true;
    }
    cv.notify_all(); // 唤醒工作线程，让它退出

    if (workerThread.joinable()) {
        workerThread.join(); // 等待线程结束
    }
    stopMonitor = true;
    if (monitorThread.joinable()) {
        monitorThread.join();
    }
    logger.Write("[System] Scheduler Stopped.");

}

// 添加任务
void TaskScheduler::AddTask(std::shared_ptr<ITask> task, int delayMs, bool periodic, int intervalMs) {
    auto now = std::chrono::system_clock::now();
    auto executeTime = now + std::chrono::milliseconds(delayMs);

    ScheduledTask newTask(task, executeTime, periodic, intervalMs);

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push(newTask);
        logger.Write("[Task] Added task: " + task->GetName());
    }
    cv.notify_one(); // 通知工作线程有新任务了
}

// 核心工作循环
void TaskScheduler::WorkerLoop() {
    while (true) {
        std::shared_ptr<ITask> taskToRun = nullptr;
        bool isPeriodic = false;
        int intervalMs = 0;

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            // 等待条件：停止标志为 true，或者队列不为空
            // 如果队列为空且没停止，就一直等
            cv.wait(lock, [this] {
                return stopScheduler || !taskQueue.empty();
                });

            // 如果收到停止信号且队列已处理完（或者强制退出），则退出循环
            if (stopScheduler && taskQueue.empty()) {
                break;
            }

            // 查看队首任务
            const auto& topTask = taskQueue.top();
            auto now = std::chrono::system_clock::now();

            if (now >= topTask.executeTime) {
                // 时间到了，取出任务执行
                taskToRun = topTask.task;
                isPeriodic = topTask.isPeriodic;
                intervalMs = static_cast<int>(topTask.interval.count());
                taskQueue.pop(); // 移除任务
            }
            else {
                // 时间还没到，使用 wait_until 等待特定时间
                // 这里不仅等待时间，还要监听是否有新任务插入（notify）或停止信号
                auto status = cv.wait_until(lock, topTask.executeTime);

                // 如果是被超时唤醒（时间到了），或者因为新任务插入被唤醒
                // 这里我们简单的 continue，重新进入循环检查队首
                if (status == std::cv_status::timeout) {
                    // 时间到了，下一次循环会进入 if 分支执行
                }
                continue;
            }
        } // 离开作用域，自动解锁 (lock 析构)，以便任务执行时不阻塞队列操作

        // 执行任务 (在锁外执行，避免阻塞添加任务的操作)
        if (taskToRun) {
            try {
                // 记录日志
                logger.Write("[Running] Executing task: " + taskToRun->GetName());

                // 执行具体策略
                NotifyObservers("[Running] " + taskToRun->GetName());
                taskToRun->Execute();

                logger.Write("[Finished] Task completed: " + taskToRun->GetName());

                // 如果是周期任务，重新加入队列
                if (isPeriodic) {
                    AddTask(taskToRun, intervalMs, true, intervalMs);
                }
            }
            catch (const std::exception& e) {
                logger.Write("[Error] Exception in task " + taskToRun->GetName() + ": " + e.what());
            }
            catch (...) {
                logger.Write("[Error] Unknown exception in task " + taskToRun->GetName());
            }
        }
    }
}

void TaskScheduler::MonitorLoop() {
    while (!stopMonitor) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (stopMonitor) break;

        if (isTaskRunning) {
            auto now = std::chrono::system_clock::now().time_since_epoch().count();
            // 这里单位取决于 time_since_epoch 的精度，通常转换成秒更安全
            // 我们简单处理：获取当前时间点对比
            auto start = std::chrono::system_clock::time_point(std::chrono::system_clock::duration(taskStartTime));
            auto current = std::chrono::system_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::seconds>(current - start).count();

            // 阈值：如果任务执行超过 10 秒，视为潜在死锁
            if (duration > 10) {
                std::string warning = "[DEADLOCK WARNING] Task is stuck for over 10 seconds!";
                // 写入日志并通知 UI
                logger.Write(warning);
                NotifyObservers(warning);

                // 注意：C++ std::thread 无法安全地强制杀死 (Kill)，
                // 所以我们只能报警，或者在日志里标记“系统处于不健康状态”。
            }
        }
    }
}