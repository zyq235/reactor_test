#include "thread_pool.h"


using namespace std;

ThreadPool::ThreadPool(int max_thread):max_threads(max_thread),run_num(0) {
	if (max_threads < 0) max_threads = 0;
	wait_queue = new std::deque<struct task_elem>();
	run_threads = new std::list<struct my_thread*>();
	free_threads = new std::list<struct my_thread*>(max_threads);
	thread_pool_status = true;
	int id = 0;
	for (auto i = free_threads->begin(); i != free_threads->end(); i++) {
		(*i) = new struct my_thread;
		(*i)->status_th = MY_THREAD_FREE;
		(*i)->my_th = thread(thread_wait_tasks,this,i);
		(*i)->thread_id = id;
		id++;
	}

}

ThreadPool::~ThreadPool() {
	thread_pool_status = false;
	wait_cv.notify_all();
	for (auto i = free_threads->begin(); i != free_threads->end(); i++) {
		(*i)->my_th.join();
	}
	delete[] wait_queue;
	delete[] run_threads;
	delete[] free_threads;

};

int ThreadPool::alloc_task(void (*func)(int), int args, int argsize) {
	std::unique_lock<std::mutex> lck(task_mtx);
	struct task_elem task_elem;
	task_elem.func = func;
	task_elem.args = args;
	wait_queue->push_back(task_elem);
	wait_cv.notify_one();
	return 0;
}



void thread_wait_tasks(ThreadPool* tp, list<struct my_thread *>::iterator pth) {
	struct task_elem task;
	list<struct my_thread *>::iterator ip,np;//init pointer, new pointer
	ip = pth;
	while (tp->thread_pool_status) {
		{
			std::unique_lock<std::mutex> lck(tp->task_mtx);
			while (tp->wait_queue->empty() && tp->thread_pool_status)
			{
				tp->wait_cv.wait(lck);
			}
			//after get lock, means get the task
			task = tp->wait_queue->front();
			tp->wait_queue->pop_front();
			tp->run_num++;
			tp->run_threads->push_front(*ip);
			tp->free_threads->erase(ip);
			np = tp->run_threads->begin();
			(*np)->status_th = MY_THREAD_RUN;
		}
		//run task
		task.func(task.args);

		//clear_thread_status();
		{
			std::unique_lock<std::mutex> lck(tp->task_mtx);
			tp->run_num--;
			(*np)->status_th = MY_THREAD_FREE;
			tp->free_threads->push_front(*np);
			tp->run_threads->erase(np);
			ip = tp->free_threads->begin();
		}
	}
}
