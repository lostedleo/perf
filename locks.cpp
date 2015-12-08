/*************************************************************************
	> File Name:    locks.cpp
	> Author:       Zhu Zhenwei
	> Mail:         losted.leo@gmail.com
	> Created Time: 一 12/ 7 16:05:06 2015
 ************************************************************************/

#include <iostream>
#include <pthread.h>
#include "thread.h"
#include "locks.h"

using namespace std;
using namespace utility;

static CThMutex gMutex;
static Spinlock gSpinlcok;
static uint64_t gCount=0;

#ifdef SPINLOCK
#define SCOPELOCKER ScopedLocker<Spinlock> tmp(&gSpinlcok)
#elif defined MUTEXLOCK
#define SCOPELOCKER ScopedLocker<CThMutex> tmp(&gMutex)
#else
#define SCOPELOCKER
#endif

enum ELockType
{
    ELT_MUTEX       = 1,
    ELT_SPINLOCK    = 2
};

class CLockThread : public CThread
{
public:
    CLockThread() {};
    ~CLockThread() {};

public:
    virtual void Run()
    {
        while (m_bStart)
        {
            SCOPELOCKER;
            gCount++;
        }
    }

    virtual void OnEnd()
    {
        CThread::OnDelete();
    }

};

void startLockThread(uint32_t uiNum, uint32_t uiLockSecs, ELockType lockType=ELT_MUTEX)
{
    if (lockType == ELT_MUTEX)
    {
        #define MUTEXLOCK
    }
    else if (lockType == ELT_SPINLOCK)
    {
        #define SPINLOCK
    }

    CLockThread* pThread = NULL;
    pThread = new CLockThread[uiNum];
    for (uint32_t i=0; i<uiNum; ++i)
    {
        pThread[i].Start();
    }

    sleep(uiLockSecs);

    for (uint32_t i=0; i<uiNum; ++i)
    {
        pThread[i].Stop();
        pThread[i].OnEnd();
    }

    delete [] pThread;

    cout << "Lock count:" << gCount << " locks per second lps:" << gCount/uiLockSecs  << endl;
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        cout << "Usage:" << argv[0] << " threads numbers [lock seconds]." << endl;
        return -1;
    }

    uint32_t uiNum = atoi(argv[1]);
    uint32_t uiLockSecs = 5;
    if (argc == 3)
    {
        uiLockSecs = atoi(argv[2]);
    }

    try
    {
        startLockThread(uiNum, uiLockSecs, ELT_MUTEX);
        startLockThread(uiNum, uiLockSecs, ELT_SPINLOCK);
    }
    catch (CException& e)
    {
        cout << "Exception:" << e.ErrorMsg() << endl;
    }
    catch (...)
    {
        cout << "Unknown exception" << endl;
    }

    return 0;
}

