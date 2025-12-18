#pragma once
#include <fstream>
#include <string>
#include <mutex>
#include <iostream>

// RAII 包装文件写入，构造打开、析构自动关闭 [cite: 206]
class LogWriter {
private:
    std::ofstream logFile;
    std::mutex mtx; // 互斥锁，保证多线程写入安全

public:
    // 构造函数：打开文件
    LogWriter(const std::string& filename) {
        // ios::app 表示追加模式 (append)
        logFile.open(filename, std::ios::app);
        if (!logFile.is_open()) {
            // 在实际项目中这里可能需要更严谨的错误处理
            std::cerr << "Failed to open log file: " << filename << std::endl;
        }
    }

    // 析构函数：关闭文件 (RAII 核心)
    ~LogWriter() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    // 写入日志的方法
    void Write(const std::string& message) {
        std::lock_guard<std::mutex> lock(mtx); // 自动加锁解锁
        if (logFile.is_open()) {
            logFile << message << std::endl;
        }
    }
};