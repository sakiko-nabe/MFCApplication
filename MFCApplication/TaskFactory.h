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
        else if (type == "DeadlockA") return std::make_shared<DeadlockTaskA>();
        else if (type == "DeadlockB") return std::make_shared<DeadlockTaskB>();
        // TaskFactory.h 的 CreateTask 函数里

        if (type == "Unsafe") return std::make_shared<UnsafeTask>();
        else if (type == "Safe") return std::make_shared<SafeTask>();
        else if (type == "Victim") return std::make_shared<VictimTask>();
        // 默认返回空或抛出异常
        return nullptr;
    }
};