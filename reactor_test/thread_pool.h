#ifndef MY_THREAD_POOL_H
#define MY_THREAD_POOL_H

#include <thread>
#include <iostream>
#include <mutex>
#include <deque>
#include <list>
#include <condition_variable>
#include <string.h>
#define WINDOWS							0
#if WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

#define MY_THREAD_FREE					0x0
#define MY_THREAD_RUN					0x1


struct my_thread {
	std::thread my_th;
	int status_th;
	unsigned int thread_id;
};


struct task_elem {
	void (*func)(int);
	int args;
};

class ThreadPool {
private:
	friend void thread_wait_tasks(ThreadPool* tp, std::list<struct my_thread *>::iterator pth);
	int max_threads;
	int run_num;
	bool thread_pool_status;
	std::deque<struct task_elem>* wait_queue;
	std::list<struct my_thread *>* run_threads;
	std::list<struct my_thread *>* free_threads;
	std::mutex task_mtx;
	std::condition_variable wait_cv;
public:
	ThreadPool(int max_thread);
	~ThreadPool();
	//int get_threads_num();
	int alloc_task(void (*func)(int), int args,int argsize);
	//void join_all_tasks();//wait until all threads stop
};

void thread_wait_tasks(ThreadPool* tp, std::list<struct my_thread *>::iterator pth);


#endif