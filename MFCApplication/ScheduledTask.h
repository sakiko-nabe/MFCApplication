#pragma once
#include "ITask.h"
#include <memory>
#include <chrono>

// 对应设计模式：Command (命令模式)
// 将请求封装为对象，包含执行该请求所需的所有信息（任务 + 时间）
struct ScheduledTask {
    // 使用 std::shared_ptr 管理任务对象的生命周期
    std::shared_ptr<ITask> task;

    // 计划执行的时间点
    std::chrono::system_clock::time_point executeTime;

    // 是否为周期性任务 [cite: 206]
    bool isPeriodic;

    // 如果是周期性任务，周期的间隔（毫秒）
    std::chrono::milliseconds interval;

    // 构造函数
    ScheduledTask(std::shared_ptr<ITask> t, std::chrono::system_clock::time_point time, bool periodic = false, int intervalMs = 0)
        : task(t), executeTime(time), isPeriodic(periodic), interval(intervalMs) {
    }

    // 运算符重载 > ：用于优先队列的排序
    // std::priority_queue 默认是最大堆。我们希望时间越早（数值越小）的排在越前面。
    // 所以这里定义 "大于" 为 "时间更晚"，这样 std::greater 就能把时间早的放在队首。
    bool operator>(const ScheduledTask& other) const {
        return executeTime > other.executeTime;
    }
};