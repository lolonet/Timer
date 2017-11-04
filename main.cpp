#include <iostream>
#include "Timer.h"
#include <sys/epoll.h>

using namespace std;

// 定时器到了之后的中枢控制器，方便做统一管理，每个定时器也支持自定义函数
int timeoutNotify(int timeid)
{
	cout<<" timeout id=" << timeid <<endl;
	switch(timeid)
	{
		case 100:
			cout<<" do time id 100" <<endl;
			break;
		case 101:
			cout<<" do time id 101" <<endl;
			break;

		default:
			cout<<" do time id " << timeid <<endl;
			break;

	}
	return timeid;
}

int otherTimeoutNotify(int timeid)
{
	cout<<" time out notfiy " << timeid <<endl;
	return timeid;
}

int main(int argc, char *argv[])
{
	const int epfd = epoll_create(1);
	epoll_event events[1] = {};

	TimerContraller timeCtl;
	Timer timer(&timeCtl);
	timer.StartTimer(100, 3, timeoutNotify);
	timer.StartTimer(101, 6, timeoutNotify);
	timer.StartTimer(102, 4, otherTimeoutNotify);

	const int minEpollTime = 1000;			// 1000 msec
	while(true)
	{
		const int n = epoll_wait(epfd, events, sizeof(events)/sizeof(epoll_event), minEpollTime);
		timeCtl.CheckExpire();

		cout<<" contral list " << timeCtl.m_SameTimerList.size() <<" contral queue " << timeCtl.m_TimerQueue.size() <<endl;

	}

	return 0;
}