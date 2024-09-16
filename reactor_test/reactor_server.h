#ifndef MY_REACTOR_SERVER_H
#define MY_REACTOR_SERVER_H

#include <iostream>
#include <thread>
#include <mutex>
#include <map>
#include <list>
#include "thread_pool.h"
#include <sys/epoll.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>


#define REACTOR_RUNNING							0
#define REACTOR_INIIALIZING						1
#define REACTOR_IDLE							2


namespace my_reactor{


struct sockitem {
	int sockfd;
	int epfd;
	void (*task_cb)(void*);
	void* args;
};

class Reactor {
private:
	int status;
	std::list<struct sockaddr_in> listen_list;
	int cur_port;
	ThreadPool* tp;
	thread epoll_thread;
	int init_sock(const std::string ip_addr,int port);

public:
	Reactor(std::string server_name, unsigned int start_port, int listen_num);
	int epfd;
	unsigned int start_port;
	int listen_num;
	std::string server_name;
};

void _epoll_main_th();
#endif