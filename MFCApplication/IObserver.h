#pragma once
#include <string>

// 对应设计模式：Observer (观察者模式) - 接口
// 允许 UI 订阅调度器的消息
class IObserver {
public:
    virtual ~IObserver() = default;
    // 当调度器产生日志时，调用此方法通知观察者
    virtual void OnLogUpdate(const std::string& message) = 0;
};