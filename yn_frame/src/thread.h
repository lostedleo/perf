#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <semaphore.h>

#include "Exception.h"

namespace utility
{

class CThCond;
class CThMutex
{
	friend class CThCond;
protected:
	string          m_name;
	pthread_mutex_t m_tMutex;

public:
	CThMutex();
	CThMutex(string name);
	virtual ~CThMutex();

	void SetName(string name);
	bool Lock();
	bool TryLock();
	bool Unlock();
};


class CThSem
{
protected:
	sem_t m_tSem;
	
public:
	CThSem(int iValue = 0);
	virtual ~CThSem();
	
	bool Wait();
	bool TryWait();
	bool Post();
	int GetValue();
};


class CThCond
{
protected:
	pthread_cond_t m_tCond;
	
public:
	CThCond();
	virtual ~CThCond();
	
	bool Signal();
	bool Broadcast();
	bool Wait(CThMutex& stThMutex);
	bool TimedWait(CThMutex& stThMutex, const struct timespec* pstAbsTime);
};


class CThRWLock
{
public:
	CThRWLock(bool bWriterFirst = false, int iMaxReader = 0);	//0 means no limit
	~CThRWLock();
	
	bool ReadLock();
	bool WriteLock();
	bool Unlock();
	
protected:
	const bool m_bWriterFirst;
	const int m_iMaxReader;
	
	CThMutex m_stMutex;
	CThCond m_stReaderCond;
	CThCond m_stWriterCond;
	int m_iWaitingReader;
	int m_iWaitingWriter;
	int m_iHoldingLock;	//-1 means writer holds the lock, >0 means reader count hloding the lock
};


class CThread
{
	friend void* ThreadRun(CThread* pstThread);

protected:
	pthread_attr_t m_tThreadAttr;
	pthread_t m_tThread;
	bool m_bDetach;
	bool m_bStart;

public:
	CThread(bool bDetach = true);
	virtual ~CThread();
	
	void Start();
    void Stop();
	void ThreadJoin();
	
protected:
	virtual void Run() = 0;
	virtual void OnEnd() {}	//perhaps you will delete THIS here.
	virtual void OnDelete();
	virtual void OnException(CException& e) {}
};

}
#endif
