/*2023 8 19*/
#include"wrap.h"

/*function definiton*/
void sys_err(char *str)
{
	perror(str);
	exit(EXIT_FAILURE);
}
int Socket(int domain, int type, int protocol)
{
	int ret_socket;
	ret_socket=socket(domain,type,protocol);
	if(ret_socket==-1)
	{
		sys_err("socket err");
	}
	return ret_socket;
}
int Bind(int sockfd,const struct sockaddr* addr,socklen_t addrlen)
{
	int ret_bind;
	ret_bind=bind(sockfd,addr,addrlen);
	if(ret_bind==-1)
	{
		sys_err("bind error");
	}
	return ret_bind;
}
int Listen(int sockfd,int backlog)
{
	int ret_listen;
	ret_listen=listen(sockfd,backlog);
	if(ret_listen==-1)
	{
		sys_err("listen error");
	}
	return ret_listen;
}
int Accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen)
{
	int ret_accept;
accept_again:
	ret_accept=accept(sockfd,addr,addrlen);
	if(ret_accept==-1)
	{
		if(ret_accept==EINTR)
		{
			goto accept_again;
		}
		else
		{
			sys_err("accept error");
		}
	}
	return ret_accept;
}
ssize_t Read(int fd,void *buf,size_t count)
{
	ssize_t ret_read;
read_again:
	ret_read=read(fd,buf,count);
	if(ret_read==-1)
	{
		if((errno=EAGAIN)||(errno==EWOULDBLOCK))
		{
			goto read_again;
		}
		else
		{
			sys_err("read error");
		}
	}
	return ret_read;
}
ssize_t Write(int fd,const void *buf,size_t count)
{
	ssize_t ret_write;
	ret_write=write(fd,buf,count);
	if(ret_write==-1)
	{
		sys_err("write errno");
	}
	return ret_write;
}
