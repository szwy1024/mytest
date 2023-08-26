//client_test.1.c
//2023.8.18

#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define SERVER_PORT 8888
#define SERVER_IP "127.0.0.1"

int main(int argc,char*argv[])
{
	//fd
	int cfd;

	//return value
	int ret_connect,ret_inet_pton,ret_write_server,ret_read_stdin,ret_write_stdout,ret_read_server;

	//init
	int addr;
	ret_inet_pton=inet_pton(AF_INET,SERVER_IP,(void*)&addr);
	if(ret_inet_pton==-1)
	{
		perror("pyon error");
		exit(1);
	}

	//buf
	char buf1[BUFSIZ];
	char buf2[BUFSIZ];

	//sock struct
	struct sockaddr_in s_addr;
	s_addr.sin_family=AF_INET;
	s_addr.sin_port=SERVER_PORT;
	s_addr.sin_addr.s_addr=addr;	

	////socket
	cfd=socket(AF_INET,SOCK_STREAM,0);
	if(cfd==-1)
	{
		perror("socket error");
		exit(1);
	}

	////connect
	ret_connect=connect(cfd,(struct sockaddr*)&s_addr,sizeof(s_addr));
	if(ret_connect==-1)
	{
		perror("connect error");
		exit(1);
	}

	////handler
	//int count=5;
	while(1)
	{
		ret_read_stdin=read(STDIN_FILENO,buf1,sizeof(buf1));
		if(ret_read_stdin==-1)
		{
			perror("read_stdin error");
			exit(1);
		}

		ret_write_server=write(cfd,buf1,ret_read_stdin);
		if(ret_write_server==-1)
		{
			perror("write_stdout error");
			exit(1);
		}
		ret_read_server=read(cfd,buf2,sizeof(buf2));
		if(ret_read_server==-1)
		{
			perror("read_server error");
			exit(1);
		}
		ret_write_stdout=write(STDOUT_FILENO,buf2,ret_read_server);
		if(ret_write_stdout==-1)
		{
			perror("write_stdout error");
			exit(1);
		}
	}

	//close
	close(cfd);

	return 0;
}
