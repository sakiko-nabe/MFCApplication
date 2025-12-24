#pragma once
#include "ITask.h"
#include <vector>
#include <random>
#include <chrono>
#include <sstream>
#include <afxwin.h> // 用于 AfxMessageBox
#include <cstdlib>   // 用于 std::system
#include <iomanip>   // 用于 std::put_time
#include <direct.h>  // 用于 _mkdir (可选，创建目录)
// ConcreteTasks.h 顶部添加
#include <shared_mutex>
#include <mutex>
#include <stdexcept> // 用于抛出异常

// 模拟两个共享资源 (资源 A 和 资源 B)
static std::shared_mutex g_rwLockA;
static std::shared_mutex g_rwLockB;
static std::mutex g_demoMutex;

// ConcreteTasks.h - 修正版 (无 Emoji)

// --- 死锁测试任务 1 ---
class DeadlockTaskA : public ITask {
public:
    std::string GetName() const override { return "Deadlock Actor A"; }
    void Execute() override {
        auto& logger = TaskScheduler::GetInstance()->GetLogger();

        logger.Write("[Actor A] 正在获取资源 A (Write Lock)...");
        std::unique_lock<std::mutex> lock(g_demoMutex);
        TaskScheduler::GetInstance()->NotifyObservers("[Safe] [LOCK] 已智能上锁 (RAII守护中)"); // ★ 强调守护

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // ★★★ 核心修改：在抛出异常前，大喊一声“我要自救了” ★★★
        logger.Write("[Safe] ⚠️ 检测到严重崩溃！准备抛出异常...");
        TaskScheduler::GetInstance()->NotifyObservers("[Safe] ⚠️ 任务崩溃！(RAII机制正在自动释放锁...)");

        throw std::runtime_error("模拟的严重错误");
    }
};

// --- 死锁测试任务 2 ---
class DeadlockTaskB : public ITask {
public:
    std::string GetName() const override { return "Deadlock Actor B"; }
    void Execute() override {
        auto& logger = TaskScheduler::GetInstance()->GetLogger();

        logger.Write("[Actor B] 正在获取资源 B (Write Lock)...");
        std::unique_lock<std::shared_mutex> lockB(g_rwLockB);

        // ★★★ 修改点 3: 去掉 🟢，改为 [RUN] ★★★
        logger.Write("[Actor B] 已锁定资源 B。正在处理...");
        TaskScheduler::GetInstance()->NotifyObservers("[Actor B] [RUN] 持有 B锁，正在工作...");

        std::this_thread::sleep_for(std::chrono::seconds(2));

        logger.Write("[Actor B] 需要读取资源 A，正在等待锁...");
        // ★★★ 修改点 4: 去掉 🟡，改为 [WAIT] ★★★
        TaskScheduler::GetInstance()->NotifyObservers("[Actor B] [WAIT] 等待资源 A (Read Lock)...");

        std::shared_lock<std::shared_mutex> lockA(g_rwLockA);

        logger.Write("[Actor B] 成功获取 A，任务完成。");
    }
};
// --- Task B: 矩阵乘法 (CPU密集型任务) ---
class MatrixTask : public ITask {
public:
    std::string GetName() const override { return "Matrix Calc"; }
    void Execute() override {
        auto start = std::chrono::high_resolution_clock::now();

        // 模拟计算
        volatile double sum = 0;
        for (int i = 0; i < 30000000; ++i) sum += i * 0.5;

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        // ★★★ 关键：使用 [DATA-MATRIX] 前缀 ★★★
        std::string result = "[DATA-MATRIX] 耗时: " + std::to_string(duration) + " ms (计算完成)";

        // 发送给日志 (界面会拦截这个消息)
        TaskScheduler::GetInstance()->GetLogger().Write(result);
        TaskScheduler::GetInstance()->NotifyObservers(result);
    }
};

// --- Task D: 课堂提醒 (UI交互任务) ---
class ReminderTask : public ITask {
public:
    std::string GetName() const override { return "Class Reminder (Task D)"; }

    void Execute() override {
        // 课件要求：弹出“休息5分钟”对话框 [cite: 227]
        // 注意：AfxMessageBox 会阻塞当前工作线程，直到用户点击确定
        // 在实际复杂系统中通常使用 PostMessage 通知主线程弹窗，但作为作业演示，直接弹窗是最简单的
        AfxMessageBox(_T("休息5分钟！(Take a 5 min break)"), MB_OK | MB_ICONINFORMATION);
    }
};

// ConcreteTasks.h -> BackupTask (修复权限版)

// --- Task A: 文件备份 (带时间戳 + 权限修复) ---
class BackupTask : public ITask {
public:
    std::string GetName() const override { return "File Backup (Task A)"; }

    void Execute() override {
        // 1. 准备路径
        // ★ 注意：请确保你电脑里真的有 C:\Data 这个文件夹，否则压缩会失败！
        // 如果想测试，可以手动在 C盘 建一个 Data 文件夹放几个文件进去
        std::string sourcePath = "C:\\Data";
        std::string destDir = "E:\\Backup";

        // 2. 生成带时间戳的文件名: backup_YYYYMMDD.zip
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm tm;
        localtime_s(&tm, &now);

        std::ostringstream oss;
        // 拼接出 E:\Backup\backup_20251218.zip
        oss << destDir << "\\backup_" << std::put_time(&tm, "%Y%m%d") << ".zip";
        std::string destFile = oss.str();

        // 3. 确保目标文件夹存在
        // 使用 Windows 命令创建目录，2>nul 意思是如果不报错就忽略
        std::string mkdirCmd = "mkdir \"" + destDir + "\" 2>nul";
        system(mkdirCmd.c_str());

        // 4. ★★★ 关键修改：构建 PowerShell 命令 (加入权限绕过参数) ★★★
        // -NoProfile: 不加载个人配置文件 (解决 profile.ps1 报错)
        // -ExecutionPolicy Bypass: 强行绕过脚本限制 (解决 PSSecurityException)
        std::string cmd = "powershell -NoProfile -ExecutionPolicy Bypass -Command \"Compress-Archive -Path '" + sourcePath + "' -DestinationPath '" + destFile + "' -Force\"";

        // 输出调试信息 (在VS输出窗口可以看到)
        OutputDebugStringA(("Executing Command: " + cmd).c_str());

        // 5. 执行命令
        int result = std::system(cmd.c_str());

        if (result != 0) {
            // 如果失败，抛出异常记录日志
            // 常见原因：源文件夹不存在、磁盘满了、或者 E 盘不存在
            throw std::runtime_error("备份失败！请检查 C:\\Data 是否存在，以及是否有 E: 盘。");
        }

        // 如果成功，Scheduler 会自动捕获并打印 [Finished]
    }
};
#include <afxinet.h> // 必须包含，用于 HTTP 网络请求

// --- Task C: HTTP GET 请求 ---
class HttpTask : public ITask {
public:
    std::string GetName() const override { return "HTTP GET"; }
    void Execute() override {
        // ... (保留你之前的 CInternetSession 代码) ...
        // 假设这里模拟下载完成
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 模拟网络延迟

        // ★★★ 关键：使用 [DATA-HTTP] 前缀 ★★★
        std::string result = "[DATA-HTTP] 状态: 200 OK (已保存 zen.txt)";

        TaskScheduler::GetInstance()->GetLogger().Write(result);
        TaskScheduler::GetInstance()->NotifyObservers(result);
    }
}; 
#include <numeric> // 用于 accumulate

// --- Task E: 随机数统计 ---
class StatsTask : public ITask {
public:
    std::string GetName() const override { return "Statistics"; }
    void Execute() override {
        // 模拟生成随机数并计算
        int sum = 0;
        for (int i = 0; i < 10; ++i) sum += (rand() % 100);
        double mean = sum / 10.0;

        // ★★★ 关键：使用 [DATA-STATS] 前缀 ★★★
        // 保留两位小数
        char buffer[100];
        sprintf_s(buffer, "[DATA-STATS] 均值: %.2f (方差计算中...)", mean);

        TaskScheduler::GetInstance()->GetLogger().Write(buffer);
        TaskScheduler::GetInstance()->NotifyObservers(buffer);
    }
};



// ---------------------------------------------------------
// 💀 角色 A: 不安全的任务 (手动管理锁 -> 导致死锁)
// ---------------------------------------------------------
class UnsafeTask : public ITask {
public:
    std::string GetName() const override { return "Unsafe Task (Deadlock)"; }
    void Execute() override {
        auto& log = TaskScheduler::GetInstance()->GetLogger();

        log.Write("[Unsafe] 1. 准备手动上锁 (m.lock())...");

        // 【危险操作】手动上锁
        g_demoMutex.lock();

        log.Write("[Unsafe] 2. 已上锁！正在执行业务...");
        TaskScheduler::GetInstance()->NotifyObservers("[Unsafe] [LOCK] 已手动上锁");

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // 【制造事故】抛出异常
        log.Write("[Unsafe] 3. ⚠️ 突然发生异常！(抛出 throw)");
        TaskScheduler::GetInstance()->NotifyObservers("[Unsafe] [ERROR] 任务崩溃抛出异常！");

        // ！！！关键点！！！
        // 因为抛出了异常，代码直接跳走了，下面这行 unlock 永远不会被执行！
        // 锁永远没有被释放！
        throw std::runtime_error("模拟的严重错误");

        g_demoMutex.unlock(); // 这行代码变成了“僵尸代码”，永远执行不到
    }
};

// ---------------------------------------------------------
// 🛡️ 角色 B: 安全的任务 (RAII 自动管理 -> 避免死锁)
// ---------------------------------------------------------
class SafeTask : public ITask {
public:
    std::string GetName() const override { return "Safe Task (RAII)"; }
    void Execute() override {
        auto& log = TaskScheduler::GetInstance()->GetLogger();

        log.Write("[Safe] 1. 准备智能上锁 (std::unique_lock)...");

        // 【安全操作】使用 RAII 管理锁
        // 只要这个 lock 变量活着，锁就锁着；一旦它“死”了（析构），锁自动解开
        std::unique_lock<std::mutex> lock(g_demoMutex);

        log.Write("[Safe] 2. 已上锁！正在执行业务...");
        TaskScheduler::GetInstance()->NotifyObservers("[Safe] [LOCK] 已智能上锁");

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // 【制造事故】抛出异常
        log.Write("[Safe] 3. ⚠️ 突然发生异常！(抛出 throw)");
        TaskScheduler::GetInstance()->NotifyObservers("[Safe] [ERROR] 任务崩溃抛出异常！");

        // ！！！关键点！！！
        // 虽然抛出了异常，但在跳出函数的一瞬间，C++ 会自动销毁 'lock' 变量。
        // 'lock' 的析构函数会自动调用 g_demoMutex.unlock()。
        throw std::runtime_error("模拟的严重错误");
    }
};

// ---------------------------------------------------------
// 😐 角色 C: 普通任务 (受害者/验证者)
// ---------------------------------------------------------
class VictimTask : public ITask {
public:
    std::string GetName() const override { return "Victim Task"; }
    void Execute() override {
        auto& log = TaskScheduler::GetInstance()->GetLogger();

        log.Write("[Victim] 我是普通任务，我想申请锁...");
        TaskScheduler::GetInstance()->NotifyObservers("[Victim] ✋ 我是后续任务，正在尝试获取锁...");

        std::lock_guard<std::mutex> lock(g_demoMutex);

        // ★★★ 核心修改：成功后的“庆功”感言 ★★★
        log.Write("[Victim] ✅ 成功拿到锁！");
        TaskScheduler::GetInstance()->NotifyObservers("[Victim] ✅ 成功运行！(证明上一个任务虽崩但未死锁)");
    }
};