/*2023 8 19*/
#ifndef _WRAP_H_
#define _WRAP_H_

/*header file*/
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<ctype.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/epoll.h>
#include<time.h>

/*function declaration*/
int Socket(int domain,int type,int protocol);
int Bind(int sockfd,const struct sockaddr* addr,socklen_t addrlen);
int Listen(int sockfd,int backlog);
int Accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen);
ssize_t Read(int fd,void *buf,size_t count);
ssize_t Write(int fd,const void *buf,size_t count);

#endif
