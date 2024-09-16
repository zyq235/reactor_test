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
Reactor::Reactor(std::string server_name, unsigned int start_port, int listen_num):status(REACTOR_IDLE),ip_addr(server_name){
	status = REACTOR_INIIALIZING;
	tp = new ThreadPool(20);
	status = REACTOR_RUNNING;
	epoll_thread=thread(_epoll_main_th, this);//start epoll main thread
	epoll_thread.detach();
}

int Reactor::init_sock(const std::string ip_addr,int port) {
	struct sockaddr_in serveraddr;
	int listen_fd=0;
	if(port<=0 || port>65535) return -1;
    listen_fd=socket(AF_INET,SOCK_STREAM,0);

    if(listen_fd==-1)
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
	if(bind(listen_fd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))!=0)
    {
        printf("bind failed \n");
        return -1;
    }
	if(listen(listen_fd,5)!=0)
    {
        printf("Listen failed\n");
        close(listen_fd);
        return -1;
    }
	return listen_fd;
}






void send_cb(void* args){

	
}


void recv_cb(void* args){
	struct sockitem* sitem=(struct sockitem*)args;
	int fd = sitem->sockfd;
	int epfd = sitem->epfd;
	struct epoll_event ev;

	ev.events = EPOLLIN | EPOLLET;
	si->sockfd=cli_fd;
	si->cb=recv_cb;//注册recv事件的回调
	si->epfd = epfd;
	si->args = si;
	ev.data.ptr=si;
	ev.events=EPOLLIN;
	epoll_ctl(epfd, EPOLL_CTL_ADD, cli_fd, &ev);
}


void accept_cb(void* args){
	struct sockitem* sitem=(struct sockitem*)args;
	struct sockaddr* cli_addr;
	struct sockaddr_in* cli_addr2;
	struct epoll_event ev;
	int fd = sitem->sockfd,cli_len;
	int epfd = sitem->epfd;
	int cli_fd=accept(fd, cli_addr, &cli_len);
	if(cli_fd<=0) return -1;
	cli_addr2=(struct sockaddr_in*)cli_addr;
	printf("recv accept client addr: %s, port: %d",cli_addr2->sin_addr, cli_addr2->sin_port);
	ev.events = EPOLLIN | EPOLLET;
	si->sockfd=cli_fd;
	si->cb=recv_cb;//注册recv事件的回调
	si->epfd = epfd;
	si->args = si;
	ev.data.ptr=si;
	ev.events=EPOLLIN;
	epoll_ctl(epfd, EPOLL_CTL_ADD, cli_fd, &ev);
}

void _epoll_main_th(Reactor *rp){
	struct epoll_event ev;
	int listen_fd;
	epfd=epoll_create(1);
	struct sockitem *si;
	for(i=0;i<listen_num;i++){
		//init_sock-->创建socket， bind, listen，将监听的listenfd交由epoll管理
		listen_fd=init_sock(rp->ip_addr,rp->start_port+rp->listen_num);
		si->sockfd=listen_fd;
		si->cb=accept_cb;
		si->epfd = epfd;
		si->args = si;
		ev.events=EPOLLIN;
		ev.data.ptr=si;
		epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);
	}
	fcntl(listen_fd, F_SETFL, fcntl(listen_fd, F_GETFD, 0)| O_NONBLOCK);//设置listen_fd为非阻塞
	while(status==REACTOR_RUNNING){
		int nready = epoll_wait(epfd, events, 512, -1);
		if (nready<-1){
			break;
		}
		for(int i=0;i<num;i++){
			epoll_event &e=events[i];
			if(e.event & EPOLLIN){//可读事件
				si=e.data.ptr;
				tp->alloc_task(si->cb,si->args);//alloc to thread pool an accept task
				//si->cb(si->args);-->读事件的回调函数
			}
			else if (e.event & EPOLLOUT){
				si=e.data.ptr;
				tp->alloc_task(si->cb,si->args);
			}
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
