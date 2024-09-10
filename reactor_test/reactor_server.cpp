// reactor_test.cpp: 定义应用程序的入口点。
//

#include "reactor_server.h"
#include "thread_pool.h"

using namespace std;

std::mutex task_mtx;

void test_task(int num) {
	for (int i = 0; i < 3; i++) {
		cout << "thread running----------------" << num << endl;
#if WINDOWS
		Sleep(3000);
#else
		sleep(1);
#endif
	}
}

int test_thread() {
	ThreadPool tp(5);
	int li[6] = {0,1,2,3,4,5},i=0;

	for (i = 0; i < 6; i++) {
		tp.alloc_task(test_task, i, sizeof(int));
	}
	sleep(10);
	for (i = 6; i < 10; i++) {
		tp.alloc_task(test_task, i, sizeof(int));
	}
	sleep(1000);
	return 0;
}



int main()
{
	cout << "Hello CMake." << endl;
	test_thread();
	return 0;
}
