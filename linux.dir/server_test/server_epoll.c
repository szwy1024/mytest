/*2023.8.26*/
#include"wrap.h"

#define SERVER_PORT 8888
#define MAX 1024
#define TIME_OUT 30
#define TIME_ERR "time out,plsase restart"

//define the struct
struct my_event
{
	int fd;
	int event;
	void* arg;
	void (*callback)(int,int ,void*);
	int status;
	char buf[BUFSIZ];
	int len;
	int number;
	long time;
};

//declare gloabl tree
int epfd;

//initial struct array
struct my_event evbuf[MAX+1];

//callback function declare
void acception(int fd,int event,void* arg);
void receivedata(int fd,int event,void*arg);
void senddata(int fd,int event,void*arg);

//epoperate function declare
void eventset(int fd,void*arg,void(*callback)(int,int,void*));
void eventadd(int epfd,int event,void*arg);
void eventdel(int fd,void *arg);

//function defination
void acception(int fd,int event,void* arg)
{
	int j;
	for(j=0;j<MAX;j++)
	{
		if(evbuf[j].status==0)
		{
			break;
		}
	}
	if(j==MAX)
	{
		printf("too many client!!,please wait a minutes\n");
		return;
	}
	evbuf[j].number=j+1;
	struct sockaddr_in c_addr;
	socklen_t addrlen=sizeof(c_addr);

	int cfd=Accept(fd,(struct sockaddr*)&c_addr,&addrlen);

	char c_ip[INET_ADDRSTRLEN];
	printf("accpet the client:%d,IP is:%s,PORT is:%d\n",j,inet_ntop(AF_INET,&c_addr.sin_addr,c_ip,sizeof(c_ip)),ntohs(c_addr.sin_port));

	int flag=fcntl(cfd,F_GETFL);
	flag|=O_NONBLOCK;
	fcntl(cfd,F_SETFL,flag);

	eventset(cfd,(void*)&evbuf[j],receivedata);
	eventadd(epfd,EPOLLIN|EPOLLET,(void*)&evbuf[j]);
}

void receivedata(int fd,int event,void*arg)
{
	struct my_event *ev=(struct my_event*)arg;
	int ret_read=Read(ev->fd,ev->buf,BUFSIZ);
	for(int k=0;k<ret_read;k++)
	{
		ev->buf[k]=toupper(ev->buf[k]);
	}
	printf("received data from %d client\n",ev->number);
	if(ret_read==0)
	{
		printf("the client %d finished,socket will close\n",ev->number);
		eventdel(fd,arg);
		close(fd);
		memset(ev,0,sizeof(struct my_event));
		return;
	}
	ev->len=ret_read;
	eventdel(fd,arg);
	eventset(fd,arg,senddata);
	eventadd(epfd,EPOLLOUT|EPOLLET,arg);
}

void senddata(int fd,int event,void*arg)
{
	struct my_event *ev=(struct my_event*)arg;
	printf("will send data to %d\n",ev->number);
	Write(ev->fd,ev->buf,ev->len);
	eventdel(fd,arg);
	eventset(fd,arg,receivedata);
	eventadd(epfd,EPOLLIN|EPOLLET,arg);
}

void eventset(int fd,void*arg,void(*callback)(int,int,void*))
{
	struct my_event *ev=(struct my_event*)arg;

	ev->fd=fd;
	ev->callback=callback;
	ev->arg=arg;
	ev->time=time(NULL);
}

void eventadd(int epfd,int event,void*arg)
{
	struct my_event *ev=(struct my_event*)arg;

	ev->event=event;
	ev->status=1;

	struct epoll_event *epv;
	epv=(struct epoll_event*)malloc(sizeof(epv));

	epv->events=event;
	epv->data.ptr=arg;

	epoll_ctl(epfd,EPOLL_CTL_ADD,ev->fd,epv);
}

void eventdel(int fd,void *arg)
{
	struct my_event *ev=(struct my_event*)arg;

	ev->status=0;
	epoll_ctl(epfd,EPOLL_CTL_DEL,ev->fd,NULL);
}	

int main(int argc,char*argv[])
{
	//epoll
	epfd=epoll_create(MAX);

	//socket
	int lfd=Socket(AF_INET,SOCK_STREAM,0);

	//setsockopt
	int opt;
	setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,(void*)&opt,sizeof(opt));

	//bind
	struct sockaddr_in s_addr;
	s_addr.sin_family=AF_INET;
	s_addr.sin_port=SERVER_PORT;
	s_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	Bind(lfd,(struct sockaddr*)&s_addr,sizeof(s_addr));

	//listen
	Listen(lfd,128);

	//eventset lfd
	eventset(lfd,(void*)&evbuf[MAX],acception);

	//eventsddd lfd
	eventadd(epfd,EPOLLIN|EPOLLET,(void*)&evbuf[MAX]);

	struct epoll_event events[MAX];

	int time_max=0;
	while(1)
	{
		long now=time(NULL);
		//printf("system time %lu\n",now);
		for(int t=0;t<MAX/10+1;t++,time_max++)
		{
			if(time_max==MAX)
			{
				time_max=0;
			}
			if(evbuf[t].status==1)
			{
				//printf("%d client time %lu\n",evbuf[t].number,evbuf[t].time);
				long interval=now-evbuf[t].time;
				if(interval>TIME_OUT)
				{
					eventdel(evbuf[t].fd,evbuf[t].arg);
					write(evbuf[t].fd,TIME_ERR,sizeof(TIME_ERR));
					printf("%d client time out\n",evbuf[t].number);
					close(evbuf[t].fd);
				}
			}
		}
		int ret=epoll_wait(epfd,events,MAX,0);
		for(int i=0;i<ret;i++)
		{
			struct my_event *ev=(struct my_event*)events[i].data.ptr;
			ev->callback(ev->fd,ev->event,events[i].data.ptr);
		}
	}
	return 0;
}
