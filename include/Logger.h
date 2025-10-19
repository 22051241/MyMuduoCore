#pragma once

#include<string>

#include"noncopyable.h"

enum LogLevel
{
	INFO,
	ERROR,
	FATAL,
	DEBUG
};

class Logger::noncopyable {
public:
	// 获取日志唯一的实例对象 单例
	static Logger& instance();
	void setLogLevel(int level);
	//写日志
	void log(std::string msg);
private:
	int logLevel_;
};