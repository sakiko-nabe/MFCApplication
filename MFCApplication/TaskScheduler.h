#pragma once
#include "ScheduledTask.h"
#include "LogWriter.h"
#include "IObserver.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>

// 对应设计模式：Singleton (单例)
// 保证系统中只有一个调度器实例
class TaskScheduler {
private:
    static TaskScheduler* instance; // 单例指针

    // 优先队列：使用 std::greater 配合 ScheduledTask 的 operator>，
    // 确保时间最早的任务排在队首 (Min-Heap)
    
    std::priority_queue<ScheduledTask, std::vector<ScheduledTask>, std::greater<ScheduledTask>> taskQueue;
    std::vector<IObserver*> observers;
    std::mutex observerMutex;
    std::mutex queueMutex;             // 保护队列的互斥锁
    std::condition_variable cv;        // 条件变量，用于线程唤醒
    std::atomic<bool> stopScheduler;   // 停止标志位 (原子操作)
    std::thread workerThread;          // 后台工作线程
    LogWriter logger;                  // 日志记录器 (RAII)

    // 私有构造函数，防止外部直接创建
    TaskScheduler();

    // 后台工作线程的主循环函数
    void WorkerLoop();

    std::thread monitorThread;             // 监控线程（看门狗）
    std::atomic<bool> stopMonitor;         // 停止监控的标志
    std::atomic<bool> isTaskRunning;       // 标记当前是否有任务在跑
    std::atomic<long long> taskStartTime;  // 记录任务开始的时间戳

    void MonitorLoop(); // 监控线程的具体逻辑


public:
    void AttachObserver(IObserver* observer);
    void DetachObserver(IObserver* observer);
    // 删除拷贝构造和赋值操作，确保单例唯一性
    TaskScheduler(const TaskScheduler&) = delete;
    TaskScheduler& operator=(const TaskScheduler&) = delete;

    // 获取单例实例
    static TaskScheduler* GetInstance();
    void NotifyObservers(const std::string& msg);
    LogWriter& GetLogger() { return logger; }
    // 添加任务接口
    // delayMs: 延迟多少毫秒执行
    // periodic: 是否周期性执行
    // intervalMs: 周期执行的间隔
    void AddTask(std::shared_ptr<ITask> task, int delayMs, bool periodic = false, int intervalMs = 0);

    // 启动调度器
    void Start();

    // 停止调度器
    void Stop();
};