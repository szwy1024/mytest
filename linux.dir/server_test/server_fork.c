/* 2023 8 19*/
#include"wrap.h"

/*macro definition*/
#define SERVER_MAX_PROC 128
#define SERVER_PORT 8888
#define IP_LENGTH 16

/*sigaction function*/
void catch_child(int argc)
{
	pid_t pid;
	while((pid=(waitpid(0,NULL,WNOHANG)))>0)
	{	
		printf("process %d finshed\n",pid);
	}
	return ;
}

/*main funtion*/
int main()
{
	//value
int listen_fd,connect_fd;
	struct sockaddr_in server_addr,client_addr;
	socklen_t client_addr_len;
	int pid;
	struct sigaction act;

	//init
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=SERVER_PORT;
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	client_addr_len=sizeof(client_addr);

	/*create socket*/
	listen_fd=Socket(AF_INET,SOCK_STREAM,0);
	Bind(listen_fd,(struct sockaddr*)&server_addr,sizeof(server_addr));
	Listen(listen_fd,SERVER_MAX_PROC);

	/*register sigaction handler*/
	act.sa_handler=catch_child;
	act.sa_flags=0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGCHLD,&act,NULL);

	/*accept client handler*/
	while(1)
	{
		connect_fd=Accept(listen_fd,(struct sockaddr*)&client_addr,&client_addr_len);
		pid=fork();
		if(pid<0)
		{
			perror("fork error");
			exit(EXIT_FAILURE);
		}
		else if(pid==0)
		{
			close(listen_fd);
			break;
		}
		else if(pid>0)
		{
			close(connect_fd);
			continue;
		}
	}

	/*child process*/
	if(pid==0)
	{
		char buf[BUFSIZ];
		memset(buf,0,BUFSIZ);
		char *IP;
		int i;
		IP=(char *)malloc(IP_LENGTH);
		memset(IP,0,IP_LENGTH);
		inet_ntop(AF_INET,&(client_addr.sin_addr.s_addr),IP,32);
		int PORT=ntohs(client_addr.sin_port);
		printf("%s client %d port connect successed,server process pid is%d\n",IP,PORT,getpid());
		while(1)
		{
			int read_num=Read(connect_fd,buf,sizeof(buf));
			if(read_num==0)
			{
				close(connect_fd);
				printf("%s client %d port finshsd\n",IP,PORT);
				exit(EXIT_SUCCESS);
			}
			else
			{
				Write(STDOUT_FILENO,buf,read_num);
				for(i=0;i<read_num;i++)
				{
					buf[i]=toupper(buf[i]);
				}
				Write(connect_fd,buf,read_num);
			}
		}
	}
}
