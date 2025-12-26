#pragma once
#include "ConcreteTasks.h"
#include <memory>
#include <string>

// 对应设计模式：Factory (工厂模式)
class TaskFactory {
public:
    // 静态工厂方法：根据类型字符串创建具体任务
    static std::shared_ptr<ITask> CreateTask(const std::string& type) {
        if (type == "Matrix") {
            return std::make_shared<MatrixTask>();
        }
        else if (type == "Reminder") {
            return std::make_shared<ReminderTask>();
        }
        else if (type == "Backup") {
            return std::make_shared<BackupTask>();
        }
        else if (type == "Http") {
            return std::make_shared<HttpTask>();
        }
        else if (type == "Stats") {
            return std::make_shared<StatsTask>();
        }
        else if (type == "Backup") {
            return std::make_shared<BackupTask>();
        }
        // TaskFactory.h 的 CreateTask 函数里
        // TaskFactory.h 的 CreateTask 函数内部

// 1. 注册崩溃演示任务
        if (type == "Crash") return std::make_shared<CrashTask>();
        else if (type == "SafeCrash") return std::make_shared<SafeCrashTask>();
        else if (type == "Normal") return std::make_shared<NormalTask>();

        // 2. 注册备份任务
        else if (type == "Backup") return std::make_shared<BackupTask>();

        // 3. 如果找不到
        return nullptr;
        // 在 CreateTask 函数里：

        if (type == "Crash") return std::make_shared<CrashTask>();
        else if (type == "SafeCrash") return std::make_shared<SafeCrashTask>();
        else if (type == "Normal") return std::make_shared<NormalTask>();
        else if (type == "Backup") return std::make_shared<BackupTask>();
        // ... 其他任务 ...
        // 默认返回空或抛出异常
        return nullptr;
    }
};