#ifndef _EPOLL_SERVER_H
#define _EPOLL_SERVER_H
 
	//初始化监听
		int init_listen_fd(int port, int epfd);
	//epoll模型
		void epoll_run(int port);
	//接受新连接处理
		void do_accept(int lfd, int epfd);
	//读数据
		void do_read(int cfd, int epfd);
	//解析http请求消息的每一行内容
		int get_line(int sock, char *buf, int size);
	//断开连接的函数
		void disconnect(int cfd, int epfd);
	//http请求处理
		void http_request(const char* request, int cfd);
	//发送响应头
		void send_respond_head(int cfd, int no, const char* desp, const char* type, long len);
	//发送文件
		void send_file(int cfd, const char* filename);
	//发送目录内容
		void send_dir(int cfd, const char* dirname);
	//编码函数
		void encode_str(char* to, int tosize, const char* from);
	//解码函数
		void decode_str(char *to, char *from);
	//通过文件名获取文件的类型
		const char *get_file_type(const char *name);
 
#endif
