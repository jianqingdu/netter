/*
 * thread_pool.h
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

#ifndef __NETTER_THREAD_POOL_H__
#define __NETTER_THREAD_POOL_H__
#include "ostype.h"
#include <pthread.h>
#include <list>
using namespace std;

class Thread
{
public:
	Thread() : m_thread_id(0) {}
	virtual ~Thread() {}
    
	static void* StartRoutine(void* arg) {
        Thread* pThread = (Thread*)arg;
        pThread->OnThreadRun();
        return NULL;
    }
    
	virtual void StartThread(void) {
        pthread_create(&m_thread_id, NULL, StartRoutine, this);
    }
	virtual void OnThreadRun(void) = 0;
protected:
	pthread_t	m_thread_id;
};

class ThreadNotify
{
public:
	ThreadNotify();
	~ThreadNotify();
	void Lock() { pthread_mutex_lock(&m_mutex); }
	void Unlock() { pthread_mutex_unlock(&m_mutex); }
	void Wait() { pthread_cond_wait(&m_cond, &m_mutex); }
	void Signal() { pthread_cond_signal(&m_cond); }
private:
	pthread_mutex_t 	m_mutex;
	pthread_cond_t 		m_cond;
};

class Task {
public:
	Task() {}
	virtual ~Task() {}

	virtual void run() = 0;
    
    void setThreadIdx(uint32_t idx) { m_thread_idx = idx; }
    uint32_t getThreadIdx() { return m_thread_idx; }
private:
    uint32_t    m_thread_idx;
};

class WorkerThread {
public:
	WorkerThread();
	~WorkerThread();

	static void* StartRoutine(void* arg);

	void Start();
    void Stop();
	void Execute();
	void PushTask(Task* pTask);

    pthread_t GetThreadId() { return m_thread_id; }
	void SetThreadIdx(uint32_t idx) { m_thread_idx = idx; }
private:
    bool            m_stop;
	uint32_t		m_thread_idx;
	pthread_t		m_thread_id;
	ThreadNotify	m_thread_notify;
	list<Task*>     m_task_list;
};

class ThreadPool {
public:
	ThreadPool();
	virtual ~ThreadPool();

	int Init(uint32_t thread_num);
	void AddTask(Task* pTask);
    void AddTask(Task* pTask, uint32_t thread_idx);
	void Destory();
private:
	uint32_t 		m_thread_num;
	WorkerThread* 	m_worker_threads;
};


#endif /* THREADPOOL_H_ */
