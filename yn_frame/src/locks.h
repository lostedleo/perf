/*************************************************************************
	> File Name:    locks.h
	> Author:       Zhu Zhenwei
	> Mail:         losted.leo@gmail.com
	> Created Time: 一 12/ 7 18:01:27 2015
 ************************************************************************/

#ifndef _LOCKS_H
#define _LOCKS_H

#ifdef __APPLE__
#include <libkern/OSAtomic.h>
#else
#include <pthread.h>
#endif

template<typename T>
class ScopedLocker
{
public:
    typedef T LockType;

    explicit ScopedLocker(T* lock) : mLock(lock)
    {
        mLock->Lock();
    }
    explicit ScopedLocker(T& lock) : mLock(&lock)
    {
        mLock->Lock();
    }
    ~ScopedLocker()
    {
        mLock->Unlock();
    }

private:
        T* mLock;
};

template<typename T>
class ScopedUnlocker 
{
public:
    typedef T LockType;

    explicit ScopedUnlocker(T* lock) : mLock(lock)
    {
        mLock->Unlock();
    }
    explicit ScopedUnlocker(T& lock) : mLock(&lock)
    {
        mLock->Unlock();
    }
    ~ScopedUnlocker()
    {
        mLock->Lock();
    }

private:
    T* mLock;
};

class Spinlock
{
 private:    //private copy-ctor and assignment operator ensure the lock never gets copied, which might cause issues.
    Spinlock operator=(const Spinlock & asdf);
    Spinlock(const Spinlock & asdf);
#ifdef __APPLE__
    OSSpinLock m_lock;
 public:
    Spinlock()
    : m_lock(0)
    {}
    void Lock() {
            OSSpinLockLock(&m_lock);
        }
    bool try_lock() {
            return OSSpinLockTry(&m_lock);
        }
    void Unlock() {
            OSSpinLockUnlock(&m_lock);
        }
#else
    pthread_spinlock_t m_lock;
public:
    Spinlock() {
            pthread_spin_init(&m_lock, 0);
        }

    void Lock() {
            pthread_spin_lock(&m_lock);
        }
    bool try_lock() {
            int ret = pthread_spin_trylock(&m_lock);
            return ret != 16;   //EBUSY == 16, lock is already taken
        }
    void Unlock() {
            pthread_spin_unlock(&m_lock);
        }
    ~Spinlock() {
            pthread_spin_destroy(&m_lock);
        }
#endif
};

#endif
