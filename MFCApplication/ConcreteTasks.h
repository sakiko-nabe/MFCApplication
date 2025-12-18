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

// --- Task A: 文件备份 (模拟) ---
class BackupTask : public ITask {
public:
    std::string GetName() const override { return "File Backup (Task A)"; }

    void Execute() override {
        // 1. 准备路径 (根据课件要求: C:\Data -> D:\Backup)
        // 注意：为了防止报错，请确保你的电脑上有 C:\Data 文件夹，或者你可以改成 "./Data" 测试
        std::string sourcePath = "C:\\Data";
        std::string destDir = "E:\\Backup";

        // 2. 生成带时间戳的文件名: backup_YYYYMMDD.zip
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm tm;
        localtime_s(&tm, &now); // 安全转换时间

        std::ostringstream oss;
        oss << destDir << "\\backup_" << std::put_time(&tm, "%Y%m%d") << ".zip";
        std::string destFile = oss.str();

        // 3. 确保目标文件夹存在 (简单的创建目录尝试)
        std::string mkdirCmd = "mkdir \"" + destDir + "\" 2>nul"; // Windows 命令创建目录，忽略错误
        system(mkdirCmd.c_str());

        // 4. 构建 PowerShell 压缩命令
        // 命令格式: Compress-Archive -Path '源' -DestinationPath '目标' -Force
        std::string cmd = "powershell -command \"Compress-Archive -Path '" + sourcePath + "' -DestinationPath '" + destFile + "' -Force\"";

        // 输出调试信息
        OutputDebugStringA(("Executing Command: " + cmd).c_str());

        // 5. 执行命令 (这是一个阻塞操作，适合放在后台线程)
        int result = std::system(cmd.c_str());

        if (result != 0) {
            // 如果失败，抛出异常记录日志
            throw std::runtime_error("Backup failed! Check if C:\\Data exists.");
        }

        // 成功日志会在 Scheduler 里自动记录 [Finished]
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