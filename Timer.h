/****************************************************************
* desc; 定时器设计，采用最小堆来作为定时器的存储结构
* author: linyubin
* date: 2017-11-03
****************************************************************/

#ifndef __H_TIMER_H__
#define __H_TIMER_H__

#include <map>
#include <queue>
#include <list>
#include <time.h>
#include <sys/time.h>
#include <iostream>

using namespace std;

typedef long long int int64;
typedef int TimerCallback(int timeid);
const int64 TIMESTAMP_PRECISION = 1000000;			// 时间精度，支持毫秒级别

class Timer;
class TimerContraller;

class TimerObject
{
public:
	TimerObject(Timer *pTime, int timeid): m_pTimer(pTime), m_timeid(timeid) {};
	int StopTimer(void);
	int StartTimer(int timeout, TimerCallback *func);
	int TimeOutNotify(void);

public:
	int m_timeid;
	int m_timeout;
	TimerCallback *m_func;
	Timer *m_pTimer;

};

class Timer
{
public:
	Timer(TimerContraller *pTimeContraller): m_pTimeContraller(pTimeContraller){};
	int StopTimer(int timeid);
	int StartTimer(int timeid, int timeout, TimerCallback *func);
	TimerObject* GetTimerObj(int timeid);

public:
	map<int, TimerObject*> m_timerMap;		
	TimerContraller *m_pTimeContraller;			// 关联定时器轮询对象
};


// 这个对象，管理整个进程的所有定时器对象
class TimerContraller
{
public:
	TimerContraller(){ m_SameTimerList.clear();  }
	int RegisterTimer(TimerObject *pTimerObj);
	int DeleteTimer(TimerObject *pTimerObj);
	int CheckExpire(void);

public:
	static int64 GetTimeStamp(void);
	static int64 TransToTimeStamp(int afterTime);

public:
	map<int64, list<TimerObject*> > m_SameTimerList;									// 绝对时间关联的同一时间列表
	priority_queue<int64, vector<int64>, std::greater<int64> > m_TimerQueue;			// 最小堆处理

};


#endif

