/*2023.8.26*/
#include"wrap.h"

#define SERVER_PORT 8000
#define MAX 1024
#define TIME_OUT 30
#define TIME_ERR "time out,plsase restart"

//define the struct
struct my_event
{
	int fd;
	int event;
	void* arg;
	void (*callback)(int,int,void*);
	int status;
	char buf[BUFSIZ];
	int len;
	int number;
	long time;
	struct epoll_event* seep;
};

//declare gloabl tree
int epfd;

//initial struct array
struct my_event evbuf[MAX+1];

//callback function declare
void acception(int fd,int event,void* arg);
void receivedata(int fd,int event,void* arg);
void senddata(int fd,int event,void* arg);

//epoperate function declare
void eventset(int fd,void* arg,void(*callback)(int,int,void*));
void eventadd(int epfd,int event,void* arg);
void eventdel(int epfd,void* arg);

//function defination

int get_line(int cfd,char* buf,int size)
{
	int i=0;
	char c='\0';
	int n;
	while((i<size-1)&&(c!='\n'))
	{
		n=recv(cfd,&c,1,0);
		if(n>0)
		{
			if(c=='\r')
			{
				n=recv(cfd,&c,1,MSG_PEEK);
				if((n>0)&&(c=='\n'))
				{
					recv(cfd,&c,1,0);
				}
				else
				{
					c='\r';
				}
			}
			buf[i]=c;
			i++;
		}
		else
		{
			c='\n';
		}
	}
	buf[i]='\0';
	if(n==-1)
	{
		i=n;
	}
	return i;
}
		
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

	//int flag=fcntl(cfd,F_GETFL);
	//flag|=O_NONBLOCK;
	//fcntl(cfd,F_SETFL,flag);

	eventset(cfd,(void*)&evbuf[j],receivedata);
	eventadd(epfd,EPOLLIN|EPOLLET,(void*)&evbuf[j]);

	return ;
}

void receivedata(int fd,int event,void* arg)
{
	struct my_event* ev=(struct my_event*)arg;

	char line[BUFSIZ]={0};
	int len=get_line(fd,line,BUFSIZ);
	if(len==0)
	{
		printf("the client %d finished,socket will close\n",ev->number);
		eventdel(epfd,arg);
		close(fd);
		free(ev->seep);
		memset(ev,0,sizeof(struct my_event));
		return;
	}
	char method[16];
	char path[256];
	char protocol[16];
		
	sscanf(line,"%[^ ] %[^ ] %[^ ]",method,path,protocol);
	
	printf("received data from %d client\n",ev->number);
	printf("%s    %s      %s\n",method,path,protocol);

	eventdel(epfd,arg);
	eventset(fd,arg,senddata);
	eventadd(epfd,EPOLLOUT,arg);

	return ;
}

void senddata(int fd,int event,void* arg)
{
	struct my_event* ev=(struct my_event*)arg;
	printf("will send data to %d\n",ev->number);
	//Write(ev->fd,ev->buf,ev->len);
	eventdel(epfd,arg);
	eventset(fd,arg,receivedata);
	eventadd(epfd,EPOLLIN|EPOLLET,arg);

	return ;
}

void eventset(int fd,void* arg,void(*callback)(int,int,void*))
{
	struct my_event* ev=(struct my_event*)arg;

	ev->fd=fd;
	ev->callback=callback;
	ev->arg=arg;
	ev->time=time(NULL);
	return ;
}

void eventadd(int epfd,int event,void* arg)
{
	struct my_event* ev=(struct my_event*)arg;

	ev->event=event;
	ev->status=1;

	struct epoll_event* epv;
	epv=(struct epoll_event*)malloc(sizeof(epv));

	epv->events=event;
	epv->data.ptr=arg;

	ev->seep=epv;
	
	epoll_ctl(epfd,EPOLL_CTL_ADD,ev->fd,epv);

	return ;
}

void eventdel(int epfd,void* arg)
{
	struct my_event* ev=(struct my_event*)arg;

	ev->status=0;
	epoll_ctl(epfd,EPOLL_CTL_DEL,ev->fd,NULL);

	return ;
}	

int main(int argc,char* argv[])
{
	//int port=atoi(argv[1]);

	int ret=chdir(argv[1]);

	if(ret==-1)
	{
		perror("chdir error");
		exit(EXIT_FAILURE);
	}

	//epoll
	epfd=epoll_create(MAX);

	//socket
	int lfd=Socket(AF_INET,SOCK_STREAM,0);

	//setnonblock
	//fcntl(lfd,F_SETFL,O_NONBLOCK);

	//setsockopt
	//int opt;
	//setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,(void*)&opt,sizeof(opt));

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
	eventadd(epfd,EPOLLIN,(void*)&evbuf[MAX]);

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
					eventdel(epfd,evbuf[t].arg);
					write(evbuf[t].fd,TIME_ERR,sizeof(TIME_ERR));
					printf("%d client time out\n",evbuf[t].number);
					close(evbuf[t].fd);
				}
			}
		}
		int ret=epoll_wait(epfd,events,MAX,-1);
		for(int i=0;i<ret;i++)
		{
			struct my_event *ev=(struct my_event*)events[i].data.ptr;
			ev->callback(ev->fd,ev->event,events[i].data.ptr);
		}
	}
	return 0;
}
