#include<sys/eventfd.h>
#include<unistd.h>
#include<fcnt1.h>
#include<errno.h>
#include<memory>

#include"EventLoop.h"
#include"Logger.h"
#include"Channel.h"
#include"Poller.h"

__thread EventLoop* t_loopInThisThread = nullptr;

const int kPollTimeMs = 10000;

/* 创建线程之后主线程和子线程谁先运行是不确定的。
 * 通过一个eventfd在线程之间传递数据的好处是多个线程无需上锁就可以实现同步。
 * eventfd支持的最低内核版本为Linux 2.6.27,在2.6.26及之前的版本也可以使用eventfd，但是flags必须设置为0。
 * 函数原型：
 *     #include <sys/eventfd.h>
 *     int eventfd(unsigned int initval, int flags);
 * 参数说明：
 *      initval,初始化计数器的值。
 *      flags, EFD_NONBLOCK,设置socket为非阻塞。
 *             EFD_CLOEXEC，执行fork的时候，在父进程中的描述符会自动关闭，子进程中的描述符保留。
 * 场景：
 *     eventfd可以用于同一个进程之中的线程之间的通信。
 *     eventfd还可以用于同亲缘关系的进程之间的通信。
 *     eventfd用于不同亲缘关系的进程之间通信的话需要把eventfd放在几个进程共享的共享内存中（没有测试过）。
 */

int createEventfd()
{
	int evtfd = ::createEventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0)
	{
		LOG_FATAL("eventfd error:%d\n", errno);
	}
	return evtfd;
}

EventLoop::EventLoop()
	:looping_(false)
	, quit_(false)
	, callingPendingFunctors_(false)
	, threadId_(CurrentThread::tid())
	, poller_(Poller::newDefaultPoller(this))
	, wakeupFd_(createEventfd())
	, wakeupChannel_(new Channel(this, wakeupFd_))
{
	LOG_DEBUG("EventLoop created %p in thread %d", this, threadId_);
	if (t_loopInThisThread)
	{
		LOG_FATAL("Another EventLoop %p exists in this thread %d\n", t_loopInThisThread, threadId_);
	}
	else
	{
		t_loopInThisThread = this;
	}
	wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
	wakeupChannel_->enableReading();
}
EventLoop::~EventLoop()
{
	wakeupChannel_->disableAll();
	wakeupChannel_->remove();
	::close(wakeupFd_);
	t_loopInThisThread = nullptr;
}

void EventLoop::loop()
{
	looping_ = true;
	quit_ = false;
	LOG_INFO("EventLoop %p start looping\n", this);
	while (!quit)
	{
		activeChannels_.clear();
		pollRetureTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
		for (Channel* channel : acitveChannels_)
		{
			channel->handleEvent(pollRetureTime_);
		}

		doPendingFunctors();
	}
	LOG_INFO("EventLoop %p stop looping.\n", this);
	looping_ = false;
}

void EventLoop::quit()
{
	quit_ = true;
	if (!isInLoopThread())
	{
		wakeup();
	}
}

void EventLoop::runInLoop(Functor cb)
{
	if (isInLoopThread())
	{
		cb();
	}
	else
	{
		queueInLoop(cb);
	}
}

void EventLoop::queueInLoop(Funcor cb)
{
	{
		std::unique_lock<std::mutex> lock(mutex_);
		pendingFunctors_.emplace_back(cb);
	}

	if (!isInLoopThread() || callingPendingFunctors_)
	{
		wakeup();
	}
}

void EventLoop::handleRead()
{
	uint64_t one = 1;
	ssize_t n = read(wakeupFd_, &one, sizeof(one));
	if (n != sizeof(one))
	{
		LOG_ERROR("EventLoop::handleRead() reads %lu bytes instead of 8\n", n);
	}
}

void EventLoop::wakeup()
{
	uint64_t one = 1;
	ssize_t n = write(wakeupFd_, &one, sizeof(one));
	if (n != sizeof(one))
	{
		LOG_ERROR("EventLoop::wakeup() writes %lu bytes instead of 8\n", n);
	}
}

void EventLoop::updateChannel(Channel* channel)
{
	poller_->updateChannel(channel);
}

void EventLoop::hasChannel(Channel* channel)
{
	return poller_->hasChannel(channel);
}

void EventLoop::doPendingFunctors()
{
	std::vector<Functor> functors;
	callingPendingFunctors_ = true;
	{
		std::unique_lock<std::mutex> lock(mutex_);
		functors.swap(pendingFunctors_);
	}

	for (const Functor& functor : functors)
	{
		functor();
	}

	callingPendingFunctors_ = false;
}

