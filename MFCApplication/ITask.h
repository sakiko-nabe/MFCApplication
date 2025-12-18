#pragma once
#include <string>

// 对应设计模式：Strategy (策略模式)
// 允许运行时切换具体的任务逻辑
class ITask {
public:
    virtual ~ITask() = default;

    // 纯虚函数：执行具体的任务逻辑
    virtual void Execute() = 0;

    // 纯虚函数：获取任务名称（用于日志和UI显示）
    virtual std::string GetName() const = 0;
};