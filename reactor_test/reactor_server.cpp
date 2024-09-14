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

namespace my_reactor{
Reactor::Reactor(unsigned int start_port, int listen_num){
	status = REACTOR_IDLE;
	tp = new ThreadPool(20);
	status = REACTOR_RUNNING;
	epoll_thread=thread(_epoll_main_th, NULL);
	epoll_thread.detach();
}

int Reactor::init_sock(const std::string ip_addr,int port) {
	struct sockaddr_in serveraddr;//定义一个用来处理网络通信的数据结构，sockaddr_in分别将端口和地址存储在两个结构体中
	int listen_fd=0;
	if(port<=0 || port>65535) return -1;
    listenfd=socket(AF_INET,SOCK_STREAM,0);//在socket编程中，AF_INET是必须的，等同于固定搭配

    if(listenfd==-1)
    {
        printf("socket create fail\n");
        return -1;
    }

	memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
	if(!ip_addr.empty())
		serveraddr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
	else
    	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //serveraddr.sin_addr.s_addr=atoi(argv[1]);// specify ip address
    serveraddr.sin_port=htons(port);
	if(bind(listenfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))!=0)
    {
        printf("bind failed \n");
        return -1;
    }
	if(listen(listen_fd,5)!=0)
    {
        printf("Listen failed\n");
        close(listenfd);
        return -1;
    }
}

void _epoll_main_th(){
	struct epoll_event ev;
	int listen_fd;
	epfd=epoll_create(1);
	for(i=0;i<listen_num;i++){
		//init_sock-->创建socket， bind, listen，将监听的listenfd交由epoll管理
		listen_fd=init_sock(start_port+listen_num);
		struct sockitem  si;
		si->sockfd=listen_fd;
		si->cb=accept_cb;
		ev.events=EPOLLIN;
		ev.data.ptr=si;
		ev.data.fd=listen_fd;
		epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);
	}
	while(status==REACTOR_RUNNING){
		int nready = epoll_wait(epfd, events, 512, -1);
		if (nready<-1){
			break;
		}
		for(int i=0;i<num;i++){
			epoll_event &e=events[i];
			if(e.event & epollin){//可读事件
				si=e.data.ptr;
				si->call_back();-->读事件的回调函数
			}
			...
		}
	}
}

}


int main()
{
	cout << "Hello CMake." << endl;
	test_thread();
	return 0;
}
