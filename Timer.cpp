/****************************************************************
* desc; 定时器设计，采用最小堆来作为定时器的存储结构
* author: linyubin
* date: 2017-11-03
****************************************************************/

#include "Timer.h"
#include <algorithm>


int TimerObject::StopTimer(void)
{
	if (m_pTimer == NULL || m_pTimer->m_pTimeContraller == NULL)
		return -1;

	// 向 timeout epoll 发送消息, 移除定时器
	m_pTimer->m_pTimeContraller->DeleteTimer(this);
	return 0;
}

int TimerObject::StartTimer(int timeout, TimerCallback *func)
{
	if (func == NULL || m_pTimer == NULL || m_pTimer->m_pTimeContraller == NULL)
		return -1;

	this->m_timeout = timeout;
	this->m_func = func;
	// 向 timeout epoll 注册定时器， 将this 添加到全局最小堆结构
	m_pTimer->m_pTimeContraller->RegisterTimer(this);
	return 0;
}

int TimerObject::TimeOutNotify(void)
{
	this->m_func(m_timeid); 
	m_pTimer->StopTimer(m_timeid); 
	return 0;
}

////////////////////////////////////////////////////////////////////

TimerObject* Timer::GetTimerObj(int timeid)
{
	map<int, TimerObject*>::iterator itObj = m_timerMap.find(timeid);
	if (itObj != m_timerMap.end() && itObj->second != NULL)
		return itObj->second;

	TimerObject *pTimeObj = new TimerObject(this, timeid);
	m_timerMap[timeid] = pTimeObj;
	return pTimeObj;
}

int Timer::StartTimer(int timeid, int timeout, TimerCallback *func)
{
	if (func == NULL)
		return -1;

	TimerObject *pTimeObj = this->GetTimerObj(timeid);
	pTimeObj->StopTimer();
	pTimeObj->StartTimer(timeout, func);
	return 0;
}

int Timer::StopTimer(int timeid)
{
	map<int, TimerObject*>::iterator it = m_timerMap.find(timeid);
	if (it != m_timerMap.end())
		m_timerMap.erase(it);
	
	return 0;
}


/////////////////////////////////////////////////////////////////////

int TimerContraller::RegisterTimer(TimerObject *pTimerObj)
{
	if (pTimerObj == NULL)
		return -1;

	const int64 timeout = TimerContraller::TransToTimeStamp(pTimerObj->m_timeout);
	m_TimerQueue.push(timeout);
	m_SameTimerList[timeout].push_back(pTimerObj);
	return 0;
}

int TimerContraller::DeleteTimer(TimerObject *pTimerObj)
{
	const int64 timeout = TimerContraller::TransToTimeStamp(pTimerObj->m_timeout);
	map<int64, list<TimerObject*> >::iterator itList = m_SameTimerList.find(timeout);
	if (itList == m_SameTimerList.end())
		return 0;

	list<TimerObject*> &timeList = m_SameTimerList[timeout];
	list<TimerObject*>::iterator it = std::find(timeList.begin(), timeList.end(), pTimerObj);
	if (it != timeList.end())
		timeList.erase(it);

	return 0;
}

inline int64 TimerContraller::GetTimeStamp(void)
{
	struct timeval tv = {};
	gettimeofday(&tv, NULL);
	const int64 time = tv.tv_sec * TIMESTAMP_PRECISION + tv.tv_usec%TIMESTAMP_PRECISION;
	return time;
}

inline int64 TimerContraller::TransToTimeStamp(int afterTime)
{
	return TimerContraller::GetTimeStamp() + afterTime*(TIMESTAMP_PRECISION/1000) * 1000;
}


int TimerContraller::CheckExpire(void)
{
	const int64 now = TimerContraller::GetTimeStamp();

	while(!m_TimerQueue.empty())
	{
		const int64 minExpire = m_TimerQueue.top();
		if (minExpire > now)
			break;

		// time out 
		list<TimerObject*> &timeList = m_SameTimerList[minExpire];
		while(!timeList.empty())
		{
			TimerObject *pTimerObj = timeList.front();
			timeList.pop_front();
			pTimerObj->TimeOutNotify();
			delete pTimerObj;
		}

		m_SameTimerList.erase(minExpire);
		m_TimerQueue.pop();
	}

	return 0;
}
