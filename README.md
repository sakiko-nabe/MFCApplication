# MFC Lightweight Multi-task Scheduler
![Language](https://img.shields.io/badge/language-C%2B%2B-blue)
![Platform](https://img.shields.io/badge/platform-Windows%20x64-blue)
![Framework](https://img.shields.io/badge/framework-MFC-orange)

## 📖 项目简介 (Introduction)

本项目是一个基于 **MFC (Microsoft Foundation Classes)** 的轻量级多任务调度系统。项目采用 **三层架构** 设计，支持一次性任务、延迟任务和周期性任务的并发执行。

核心目标是演示多线程编程、资源安全管理 (RAII) 以及多种设计模式（Singleton, Factory, Strategy, Observer）在 C++ 桌面应用中的综合运用。

## ✨ 核心功能 (Features)

调度器支持以下类型的任务调度与执行：

* **Task B - 矩阵乘法 (CPU 密集型)**: 模拟耗时计算，周期性执行 (每 5s) 。
* **Task C - HTTP 请求 (I/O 密集型)**: 请求 GitHub API 并保存响应至 `zen.txt`，立即执行 。
* **Task D - 课堂提醒 (UI 交互)**: 跨线程发送消息弹窗提醒，周期性执行 (每 1min) 。
* **Task E - 统计计算 (延迟任务)**: 生成随机数并计算均值方差，延迟 10s 执行 。
* **可视化监控**: 通过观察者模式，在 MFC 界面实时滚动显示后台任务状态日志。

## 🏗️ 架构与设计模式 (Architecture & Patterns)

本项目严格遵循面向对象设计原则，实现了以下设计模式：

| 模式 | 类/模块 | 作用 |
| :--- | :--- | :--- |
| **Singleton** (单例) | `TaskScheduler` | 确保全局只有一个调度器实例，统一管理线程池与队列。 |
| **Strategy** (策略) | `ITask` 接口 | 定义任务的通用行为 `Execute()`，允许运行时切换不同任务算法。 |
| **Factory** (工厂) | `TaskFactory` | 将任务的实例化逻辑与业务逻辑解耦，便于扩展新任务。 |
| **Command** (命令) | `ScheduledTask` | 封装任务对象与执行时间 (`executeTime`)，支持优先队列排序。 |
| **Observer** (观察者) | `IObserver` / `Dlg` | 实现后台线程与 UI 主线程的解耦，实时更新日志。 |
| **RAII** | `LogWriter` | 利用对象生命周期自动管理文件句柄资源，防止泄露。 |

## 📂 文件结构 (File Structure)

```text
MultiTaskScheduler/
├── src/
│   ├── TaskScheduler.h/.cpp    # 核心调度器 (单例, 线程安全)
│   ├── ITask.h                 # 任务接口 (策略模式)
│   ├── ScheduledTask.h         # 任务包装类 (命令模式)
│   ├── ConcreteTasks.h         # 具体任务实现 (B, C, D, E)
│   ├── TaskFactory.h           # 任务工厂
│   ├── LogWriter.h             # RAII 日志工具
│   └── IObserver.h             # 观察者接口
├── docs/
│   └── ai_logs/                # AI 辅助编程日志 (Prompt/Response/Diff) [cite: 130]
├── README.md                   # 项目说明文档
└── MultiTaskScheduler.sln      # VS2026 解决方案
