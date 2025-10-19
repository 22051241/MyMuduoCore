## 构建说明

### 环境要求

- Visual Studio 2022（支持WSL）
- CMake 3.16+
- GCC 11+（在WSL中）

### 构建步骤

1. 克隆仓库：
   ```bash
   git clone https://github.com/YOUR_USERNAME/MyMuduoCore.git
   cd MyMuduoCore
   ```

2. 在Visual Studio 2022中打开项目
3. 选择WSL-GCC-Debug配置
4. 构建解决方案

## 当前状态

- [x] 项目结构搭建
- [x] 基础工具类（noncopyable, Timestamp）
- [ ] 日志系统
- [ ] 网络地址处理
- [ ] Socket操作
- [ ] 事件循环系统
- [ ] TCP服务器实现

## 开发进度

### 已完成
- ✅ 项目结构和CMake配置
- ✅ noncopyable基类（禁止对象拷贝）
- ✅ Timestamp类（微秒级时间戳）
- ✅ WSL构建环境搭建

### 进行中
- 🔄 Git仓库和GitHub集成设置

### 下一步计划
- 📋 实现Logger系统（包含宏定义）
- 📋 创建InetAddress类（网络地址）
- 📋 实现Socket封装类
- 📋 构建事件循环系统（基于epoll）

## 核心组件说明

### 基础工具类
- **noncopyable**: 禁止拷贝的基类，防止资源重复管理
- **Timestamp**: 高精度时间戳类，支持微秒级精度
- **Logger**: 线程安全的日志系统
- **CurrentThread**: 获取当前线程ID
- **Thread**: 线程封装类

### 网络基础
- **InetAddress**: IP地址和端口封装
- **Socket**: Socket操作封装
- **Buffer**: 高效的数据缓冲区
- **Callbacks**: 回调函数类型定义

### 核心组件
- **Channel**: 事件通道，封装文件描述符和事件
- **Poller**: 事件轮询器接口
- **EPollPoller**: epoll的具体实现
- **EventLoop**: 事件循环主类
- **EventLoopThread**: 事件循环线程
- **EventLoopThreadPool**: 线程池管理
- **Acceptor**: 连接接受器
- **TcpConnection**: TCP连接管理
- **TcpServer**: TCP服务器主类

## 设计理念

本项目采用Reactor模式，具有以下特点：

1. **事件驱动**: 基于epoll的高效I/O多路复用
2. **多线程**: One Loop Per Thread架构
3. **非阻塞**: 所有I/O操作都是非阻塞的
4. **回调机制**: 使用std::function实现灵活的回调
5. **资源管理**: 使用RAII和智能指针管理资源

## 学习目标

通过重写muduo网络库，学习：

- 网络编程的核心概念
- 事件驱动编程模式
- 多线程编程技巧
- 现代C++特性应用
- 高性能服务器设计

## 许可证

本项目仅用于教育目的。受muduo库启发。

## 致谢

感谢陈硕老师的muduo网络库，为本项目提供了设计灵感和学习参考。