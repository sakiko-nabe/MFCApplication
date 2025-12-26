#pragma once
#include "ITask.h"
#include "TaskScheduler.h"
#include <string>
#include <thread>
#include <mutex>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>

// ==========================================
// 1. 全局资源锁 (防死锁演示用)
// ==========================================
static std::mutex g_resourceMutex;



// --- 矩阵计算任务 ---
class MatrixTask : public ITask {
public:
    std::string GetName() const override { return "Matrix Calc"; }
    void Execute() override {
        // 简单模拟矩阵计算
        TaskScheduler::GetInstance()->GetLogger().Write("[Matrix] 正在进行矩阵乘法运算...");
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        TaskScheduler::GetInstance()->GetLogger().Write("[Matrix] 运算完成。结果已生成。");
    }
};

// --- 课堂提醒任务 ---
class ReminderTask : public ITask {
public:
    std::string GetName() const override { return "Class Reminder"; }
    void Execute() override {
        TaskScheduler::GetInstance()->GetLogger().Write("[Reminder] 检查课程表中...");
        ::MessageBox(NULL, _T("该上课了！\n请检查您的日程安排。"), _T("课堂提醒"), MB_OK | MB_TOPMOST);
        TaskScheduler::GetInstance()->GetLogger().Write("[Reminder] 提醒已发送。");
    }
};

// --- 文件备份任务 (修复版：带权限绕过) ---
class BackupTask : public ITask {
public:
    std::string GetName() const override { return "File Backup"; }
    void Execute() override {
        auto& log = TaskScheduler::GetInstance()->GetLogger();
        log.Write("[Backup] 正在启动 PowerShell 备份...");

        // 模拟命令，防止报错
        std::string cmd = "powershell -NoProfile -ExecutionPolicy Bypass -Command \"Write-Host 'Backup Test'\"";
        system(cmd.c_str());

        log.Write("[Backup] ✅ 备份流程结束。");
    }
};

// --- HTTP 任务 (空壳，防止报错) ---
class HttpTask : public ITask {
public:
    std::string GetName() const override { return "HTTP Request"; }
    void Execute() override {
        TaskScheduler::GetInstance()->GetLogger().Write("[HTTP] 正在抓取数据...");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        TaskScheduler::GetInstance()->GetLogger().Write("[HTTP] 数据获取成功。");
    }
};

// --- 统计任务 (空壳，防止报错) ---
class StatsTask : public ITask {
public:
    std::string GetName() const override { return "Data Stats"; }
    void Execute() override {
        TaskScheduler::GetInstance()->GetLogger().Write("[Stats] 正在分析数据...");
    }
};



// --- 角色A: 崩溃导致死锁 (CrashTask) ---
class CrashTask : public ITask {
public:
    std::string GetName() const override { return "Crash Task (Unsafe)"; }
    void Execute() override {
        auto& log = TaskScheduler::GetInstance()->GetLogger();

        log.Write("[Crash A] 正在手动上锁 (lock)...");
        g_resourceMutex.lock(); // <--- 手动上锁
        TaskScheduler::GetInstance()->NotifyObservers("[Crash A] [LOCK] 已上锁，正在操作内存...");

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        log.Write("[Crash A] 发生严重内存错误！程序即将跳出...");
        TaskScheduler::GetInstance()->NotifyObservers("[Crash A] [ERROR] 模拟崩溃: Memory Violation!");

        // 模拟崩溃跳出，导致没有 unlock
        throw std::runtime_error("Critical Memory Error");

        g_resourceMutex.unlock(); // 永远执行不到这里
    }
};

// --- 角色B: 防死锁的安全任务 (SafeCrashTask) ---
class SafeCrashTask : public ITask {
public:
    std::string GetName() const override { return "Safe Crash Task"; }
    void Execute() override {
        auto& log = TaskScheduler::GetInstance()->GetLogger();

        log.Write("[Safe A] 正在智能上锁 (unique_lock)...");
        std::unique_lock<std::mutex> lock(g_resourceMutex); // <--- RAII 智能上锁
        TaskScheduler::GetInstance()->NotifyObservers("[Safe A] [LOCK] 已上锁 (RAII保护中)...");

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        log.Write("[Safe A] 发生严重内存错误！准备跳出...");
        TaskScheduler::GetInstance()->NotifyObservers("[Safe A] [ERROR] 模拟崩溃! 但RAII正在自动解锁...");

        // 抛出异常，lock 自动析构释放锁
        throw std::runtime_error("Critical Memory Error (Safe)");
    }
};

// --- 角色C: 普通验证任务 (NormalTask) ---
class NormalTask : public ITask {
public:
    std::string GetName() const override { return "Normal Task B"; }
    void Execute() override {
        auto& log = TaskScheduler::GetInstance()->GetLogger();

        log.Write("[Normal B] 我是普通任务B，我想申请锁...");
        TaskScheduler::GetInstance()->NotifyObservers("[Normal B] [WAIT] 请求锁资源...");

        std::lock_guard<std::mutex> lock(g_resourceMutex); // 拿锁

        log.Write("[Normal B] 成功拿到锁！");
        TaskScheduler::GetInstance()->NotifyObservers("[Normal B] [OK] 成功执行！(锁未遗弃)");
    }
};