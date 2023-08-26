#include"wrap.h"

#define SERVER_PORT 8888

int main(int argc,char*argv[])
{
	int lfd,cfd;//listen fd,connect fd
	int fdmax;//max fd 

	struct sockaddr_in c_addr,s_addr;
	socklen_t addrlen;
	char buf[BUFSIZ];

	int ret_setsockopt;
	int opt=1;

	fd_set rset,allset;
	
	////socket
	lfd=Socket(AF_INET,SOCK_STREAM,0);
	////setsockopt
	ret_setsockopt=setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,(void *)&opt,sizeof(opt));
	if(ret_setsockopt==-1)
	{
		perror("setsockopt error");
		exit(EXIT_FAILURE);
	}
	////init
	s_addr.sin_family=AF_INET;
	s_addr.sin_port=SERVER_PORT;
	s_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	////bind
	Bind(lfd,(struct sockaddr*)&s_addr,sizeof(s_addr));
	////listen
	Listen(lfd,128);
	////io_init
	FD_ZERO(&allset);
	FD_SET(lfd,&allset);
	fdmax=lfd;
	while(1)
	{
		rset=allset;
		int ret_select;//the number of the fd
		int fd_i;
		int backfd;
		ret_select=select(fdmax+1,&rset,NULL,NULL,NULL);
		if(FD_ISSET(lfd,&rset))
		{
			addrlen=sizeof(c_addr);
			cfd=Accept(lfd,(struct sockaddr*)&c_addr,&addrlen);
			FD_SET(cfd,&allset);
			if(fdmax<cfd)
			{
				fdmax=cfd;
			}
			if(--ret_select==0)
			{
				continue;
			}
		}
		for(fd_i=lfd+1;fd_i<=fdmax;fd_i++)
		{
			if(FD_ISSET(fd_i,&rset))
			{
				int ret_read=Read(fd_i,buf,sizeof(buf));
				if(ret_read==0)//client closed
				{
					close(fd_i);//server will close
					FD_CLR(fd_i,&allset);//clear the set
					if(fd_i==fdmax)
					{
						fdmax=backfd;//if the close fd is the max fd ,go back
					}
					continue; 
				}
				for(int j=0;j<ret_read;j++)
				{
					buf[j]=toupper(buf[j]);
				}
				Write(fd_i,buf,ret_read);
				Write(STDOUT_FILENO,buf,ret_read);
				backfd=fd_i;
			}
		}
	}
	return 0;
}
