#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <arpa/inet.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>
#include <errno.h>

#define MAXMSGLEN 100
#define FDADD 1000

// int op; // operation ID
// size_t totalSize;

int (*orig_close)(int fildes);

void receiveHelper(char *buf, int sockfd, size_t recvSize)
{
	size_t totalReceive = 0;
	// while (totalReceive < sizeof(size_t))
	// {
	// 	fprintf(stderr, "here:130 ");

	// 	totalReceive += recv(sessfd, buf + totalReceive, sizeof(size_t) - totalReceive, 0);
	// }

	// memcpy(&totalSize, buf, sizeof(size_t));
	// fprintf(stderr, "total size after first loop:  %ld\n, totalReceive: %ld", totalSize, totalReceive);

	while (totalReceive < recvSize)

	{
		// fprintf(stderr, "here:144 ");
		totalReceive += recv(sockfd, buf + totalReceive, recvSize - totalReceive, 0);
	}
	fprintf(stderr, "totalSize: %ld\ntotalReceive: %ld\n", recvSize, totalReceive);
}

int openSocket()
{
	// connect to server - code from clinet.c line 13-51
	char *serverip;
	char *serverport;
	unsigned short port;
	// char *msg = "Hello from client";
	// char buf[MAXMSGLEN + 1];
	int sockfd, rv;
	struct sockaddr_in srv;

	// Get environment variable indicating the ip address of the server
	serverip = getenv("server15440");
	if (serverip)
		fprintf(stderr, "Got environment variable server15540: %s\n", serverip);
	else
	{
		fprintf(stderr, "Environment variable server15440 not found.  Using 127.0.0.1\n");
		serverip = "127.0.0.1";
	}

	// Get environment variable indicating the port of the server
	serverport = getenv("serverport15440");
	if (serverport)
		fprintf(stderr, "Got environment variable serverport15440: %s\n", serverport);
	else
	{
		fprintf(stderr, "Environment variable serverport15440 not found.  Using 15440\n");
		serverport = "15440";
	}
	port = (unsigned short)atoi(serverport);

	// Create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP/IP socket
	if (sockfd < 0)
	{
		err(1, 0);							   // in case of error
		fprintf(stderr, "Error: sockfd<0 \n"); // added
	}

	// setup address structure to point to server
	memset(&srv, 0, sizeof(srv));			   // clear it first
	srv.sin_family = AF_INET;				   // IP family
	srv.sin_addr.s_addr = inet_addr(serverip); // IP address of server
	srv.sin_port = htons(port);				   // server port

	// actually connect to the server
	rv = connect(sockfd, (struct sockaddr *)&srv, sizeof(struct sockaddr));
	if (rv < 0)
	{
		err(1, 0);
		fprintf(stderr, "Error: rv<0 \n"); // added
	}
	return sockfd;
}
void sendRequest(char *buf, size_t totalSize, int sockfd)
{
	size_t totalSent = 0;
	while (totalSent < totalSize)
	{
		totalSent += send(sockfd, buf + totalSent, totalSize - totalSent, 0); // changed!
	}
	fprintf(stderr, "totalSent: %ld \n", totalSent);
}

void send_recv(char *name, ssize_t totalSize)
{
	// connect to server - code from clinet.c line 13-51
	char *serverip;
	char *serverport;
	unsigned short port;
	// char *msg = "Hello from client";
	// char buf[MAXMSGLEN + 1];
	int sockfd, rv;
	struct sockaddr_in srv;

	// Get environment variable indicating the ip address of the server
	serverip = getenv("server15440");
	if (serverip)
		fprintf(stderr, "Got environment variable server15540: %s\n", serverip);
	else
	{
		fprintf(stderr, "Environment variable server15440 not found.  Using 127.0.0.1\n");
		serverip = "127.0.0.1";
	}

	// Get environment variable indicating the port of the server
	serverport = getenv("serverport15440");
	if (serverport)
		fprintf(stderr, "Got environment variable serverport15440: %s\n", serverport);
	else
	{
		fprintf(stderr, "Environment variable serverport15440 not found.  Using 15440\n");
		serverport = "15440";
	}
	port = (unsigned short)atoi(serverport);

	// Create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP/IP socket
	if (sockfd < 0)
	{
		err(1, 0);							   // in case of error
		fprintf(stderr, "Error: sockfd<0 \n"); // added
	}

	// setup address structure to point to server
	memset(&srv, 0, sizeof(srv));			   // clear it first
	srv.sin_family = AF_INET;				   // IP family
	srv.sin_addr.s_addr = inet_addr(serverip); // IP address of server
	srv.sin_port = htons(port);				   // server port

	// actually connect to the server
	rv = connect(sockfd, (struct sockaddr *)&srv, sizeof(struct sockaddr));
	if (rv < 0)
	{
		err(1, 0);
		fprintf(stderr, "Error: rv<0 \n"); // added
	}
	// send
	size_t totalSent = 0;
	while (totalSent < totalSize)
	{
		totalSent += send(sockfd, name + totalSent, totalSize - totalSent, 0); // changed!
	}
	fprintf(stderr, "totalSent: %ld \n", totalSent);

	// send(sockfd, name, strlen(name) + 1, 0);
	// get message back
	char buf[1024];
	fprintf(stderr, "before client got messge back: %s\n", buf);

	size_t recvSize = 2 * sizeof(int);
	receiveHelper(buf, sockfd, recvSize);

	int fd = *(int *)buf;
	int e = *(int *)(buf + sizeof(int));
	fprintf(stderr, "client got messge back fd: %d\n", fd);
	fprintf(stderr, "client got messge back errno: %d\n", e);
	orig_close(sockfd);
}

void sendHelper(char *name)
{

	// connect to server - code from clinet.c line 13-51
	char *serverip;
	char *serverport;
	unsigned short port;
	// char *msg = "Hello from client";
	// char buf[MAXMSGLEN + 1];
	int sockfd, rv;
	struct sockaddr_in srv;

	// Get environment variable indicating the ip address of the server
	serverip = getenv("server15440");
	if (serverip)
		fprintf(stderr, "Got environment variable server15540: %s\n", serverip);
	else
	{
		fprintf(stderr, "Environment variable server15440 not found.  Using 127.0.0.1\n");
		serverip = "127.0.0.1";
	}

	// Get environment variable indicating the port of the server
	serverport = getenv("serverport15440");
	if (serverport)
		fprintf(stderr, "Got environment variable serverport15440: %s\n", serverport);
	else
	{
		fprintf(stderr, "Environment variable serverport15440 not found.  Using 15440\n");
		serverport = "15440";
	}
	port = (unsigned short)atoi(serverport);

	// Create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP/IP socket
	if (sockfd < 0)
	{
		err(1, 0);							   // in case of error
		fprintf(stderr, "Error: sockfd<0 \n"); // added
	}

	// setup address structure to point to server
	memset(&srv, 0, sizeof(srv));			   // clear it first
	srv.sin_family = AF_INET;				   // IP family
	srv.sin_addr.s_addr = inet_addr(serverip); // IP address of server
	srv.sin_port = htons(port);				   // server port

	// actually connect to the server
	rv = connect(sockfd, (struct sockaddr *)&srv, sizeof(struct sockaddr));
	if (rv < 0)
	{
		err(1, 0);
		fprintf(stderr, "Error: rv<0 \n"); // added
	}
	// send
	// size_t totalSent = 0;
	// while (totalSent < totalSize)
	// {
	// 	totalSent += send(sockfd, name + totalSent, totalSize - totalSent, 0); // changed!
	// }
	// fprintf(stderr, "totalSent: %ld \n", totalSent);

	// send(sockfd, name, strlen(name) + 1, 0);
	send(sockfd, name, strlen(name) + 1, 0);

	// get message back
	// char buf[1024];
	// fprintf(stderr, "before client got messge back: %s\n", buf);

	// size_t recvSize = 2 * sizeof(int);
	// receiveHelper(buf, sockfd, recvSize);

	// rv = recv(sockfd, buf, MAXMSGLEN, 0); // get message
	// if (rv < 0)
	// 	err(1, 0); // in case something went wrong
	// buf[rv] = 0;   // null terminate string to print

	// int fd = *(int *)buf;
	// int e = *(int *)(buf + sizeof(int));
	// fprintf(stderr, "client got messge back fd: %d\n", fd);
	// fprintf(stderr, "client got messge back errno: %d\n", e);

	orig_close(sockfd);
}

/* design:
void pack_request();
void parse_response();
*/

int (*orig_open)(const char *pathname, int flags, ...); // mode_t mode is needed when flags includes O_CREAT

// This is our replacement for the open function from libc.
int open(const char *pathname, int flags, ...)
{
	char buf[1024];
	mode_t m = 0;
	if (flags & O_CREAT)
	{
		va_list a;
		va_start(a, flags);
		m = va_arg(a, mode_t);
		va_end(a);
	}
	// we just print a message, then call through to the original open function (from libc)

	int op = 0;
	size_t n = strlen(pathname);
	// total size of msg: size_t total, int op, int flag, size_t n, char *path, mode_t m
	size_t totalSize = 2 * sizeof(int) + 2 * sizeof(size_t) + n + sizeof(mode_t); // changed
	fprintf(stderr, "pathname: %s\n", pathname);
	// snprintf(buf, totalSize, "%zu%d%d%zu%s%o", totalSize, op, flags, n, pathname, m);
	int i = 0;
	memcpy(buf, &totalSize, sizeof(size_t));
	fprintf(stderr, "open() called by path 1: %s \n buf: %s with total size %ld\n", pathname, buf, totalSize);
	i += sizeof(size_t);
	memcpy(buf + i, &op, sizeof(int)); // 1. op
	i += sizeof(int);
	memcpy(buf + i, &flags, sizeof(int)); // 2. flags
	i += sizeof(int);
	memcpy(buf + i, &n, sizeof(size_t)); // 3. n
	i += sizeof(size_t);

	memcpy(buf + i, pathname, n); // 4. pathname
	i += n;
	memcpy(buf + i, &m, sizeof(mode_t)); // 5. m
	// buf[i] = '\0';

	fprintf(stderr, "m sent: %o\n", *(mode_t *)(buf + i));
	fprintf(stderr, "open() called by path: %s \n mode: %o\n", pathname, m);
	// char *msg = "open";

	fprintf(stderr, "total size before send:  %ld\n", totalSize);
	// send_recv(buf, totalSize);
	int sockfd = openSocket();
	sendRequest(buf, totalSize, sockfd);

	// get message back
	char buf2[1024];
	size_t recvSize = 2 * sizeof(int);
	receiveHelper(buf2, sockfd, recvSize);
	orig_close(sockfd);

	int fd = *(int *)buf2;
	int e = *(int *)(buf2 + sizeof(int));
	fprintf(stderr, "client got messge back fd: %d\n", fd);
	fprintf(stderr, "client got messge back errno: %d\n", e);
	errno = e;
	return fd;

	// return orig_open(pathname, flags, m);
}

int close(int fd)
{
	fprintf(stderr, "\nclose() called\n");

	char buf[1024];
	int op = 1;
	size_t totalSize = sizeof(size_t) + 2 * sizeof(int);

	// snprintf(buf, totalSize, "%zu%d%d%zu%s%o", totalSize, op, flags, n, pathname, m);
	int i = 0;
	memcpy(buf + i, &totalSize, sizeof(size_t)); // changed!
	i += sizeof(size_t);

	memcpy(buf + i, &op, sizeof(int));
	i += sizeof(int);
	// fd += FDADD;
	memcpy(buf + i, &fd, sizeof(int));

	int sockfd = openSocket();
	sendRequest(buf, totalSize, sockfd);

	fprintf(stderr, "close() called  with total size: %ld\n, fd: %d\n", totalSize, fd);

	// get message back
	char buf2[1024];
	size_t recvSize = 2 * sizeof(int);
	receiveHelper(buf2, sockfd, recvSize);
	orig_close(sockfd);

	int returned = *(int *)buf2;
	int e = *(int *)(buf2 + sizeof(int));
	fprintf(stderr, "client got messge back close returned: %d\n", returned);
	fprintf(stderr, "client got messge back errno: %d\n", e);
	errno = e;
	return returned;
}

ssize_t (*orig_read)(int fildes, void *buf, size_t nbyte);

ssize_t read(int fildes, void *buf, size_t nbyte)
{
	fprintf(stderr, "\nread() called\n");

	int op = 3;
	size_t totalSize = 2 * sizeof(size_t) + 2 * sizeof(int);
	char buf1[totalSize]; // to hold request msg to server

	// snprintf(buf, totalSize, "%zu%d%d%zu%s%o", totalSize, op, flags, n, pathname, m); 1. totalSize 2. op 3.fd, 4.size_t count
	int i = 0;
	memcpy(buf1 + i, &totalSize, sizeof(size_t)); // changed!
	i += sizeof(size_t);

	memcpy(buf1 + i, &op, sizeof(int));
	i += sizeof(int);
	// fd += FDADD;
	memcpy(buf1 + i, &fildes, sizeof(int));
	i += sizeof(int);

	memcpy(buf1 + i, &nbyte, sizeof(size_t));
	fprintf(stderr, "read() called  with total size: %ld\n, fd: %d\n: %ld\n, nbyte: %zu\n", totalSize, fildes, nbyte);

	int sockfd = openSocket();
	sendRequest(buf1, totalSize, sockfd);

	// get message back
	char buf2[1024]; // to hold the first 2 variables received from server
	size_t recvSize = sizeof(size_t) + sizeof(int);
	receiveHelper(buf2, sockfd, recvSize); // receive first 2 variables

	// ssize_t returned= *(ssize_t *)buf2; //returned read_nbytes
	ssize_t returned;
	memcpy(&returned, buf2, sizeof(size_t));
	int e = *(int *)(buf2 + sizeof(ssize_t));
	fprintf(stderr, "client got messge back read returned: %zu\n", returned);
	fprintf(stderr, "client got messge back errno: %d\n", e);
	errno = e;

	// char readContent[nbyte+1]; //to hold the read content
	char buf3[returned + 1];
	receiveHelper(buf3, sockfd, returned);
	buf3[returned] = 0;
	// receiveHelper(buf, sockfd, returned + 1); // returned is read bytes
	memcpy(buf, buf3, nbyte + 1);

	fprintf(stderr, "client got messge back read content: %s\n", (char *)buf);
	fprintf(stderr, "client got messge back read content buf3: %s\n", buf3);

	orig_close(sockfd);
	return returned;
}

ssize_t (*orig_write)(int fildes, const void *buf, size_t nbyte);

ssize_t write(int fildes, const void *buf, size_t nbyte)
{
	fprintf(stderr, "\nwrite() called\n");
	// char *msg = "write";
	// sendHelper(msg);

	int op = 2;
	// size_t n = strlen(pathname);
	//  total size of msg: size_t total, int op, int fd, size_t nbyte, buf,
	size_t totalSize = 2 * sizeof(int) + 2 * sizeof(size_t) + nbyte; // changed
	char bufSend[totalSize];
	fprintf(stderr, "buf: %s\n", (char *)buf);
	// snprintf(buf, totalSize, "%zu%d%d%zu%s%o", totalSize, op, flags, n, pathname, m);
	int i = 0;
	memcpy(bufSend + i, &totalSize, sizeof(size_t));
	fprintf(stderr, "open() called by buf: %s with total size %ld\n", (char *)buf, totalSize);
	i += sizeof(size_t);
	memcpy(bufSend + i, &op, sizeof(int));
	i += sizeof(int);
	memcpy(bufSend + i, &fildes, sizeof(int));
	i += sizeof(int);
	memcpy(bufSend + i, &nbyte, sizeof(size_t));
	i += sizeof(size_t);
	memcpy(bufSend + i, buf, nbyte);

	int sockfd = openSocket();
	sendRequest(bufSend, totalSize, sockfd);

	// get message back
	char buf2[1024];
	size_t recvSize = 2 * sizeof(int);
	receiveHelper(buf2, sockfd, recvSize);
	orig_close(sockfd);

	//  sendHelper(msg, totalSize);
	fprintf(stderr, "write() called fd %d with %ld bytes\n", fildes, nbyte);
	int returned = *(int *)buf2;
	int e = *(int *)(buf2 + sizeof(int));
	fprintf(stderr, "client got messge back write returned: %d\n", returned);
	fprintf(stderr, "client got messge back errno: %d\n", e);
	errno = e;
	return returned;
	// return orig_write(fildes, buf, nbyte);
}

ssize_t (*orig_lseek)(int fildes, off_t offset, int whence);

ssize_t lseek(int fildes, off_t offset, int whence)
{
	// char *msg = "lseek";

	// strcpy(msg, "lseek");
	// send_recv();
	// send(sockfd, msg, strlen(msg) + 1, 0);
	// sendHelper(msg);
	// sendHelper(msg, totalSize);

	fprintf(stderr, "lseek() called fd %d with %ld offset\n", fildes, offset);
	return orig_lseek(fildes, offset, whence);
}

int (*orig_stat)(const char *restrict pathname,
				 struct stat *restrict statbuf);

int stat(const char *restrict pathname, struct stat *restrict statbuf)
{
	// char *msg = "stat";

	// strcpy(msg, "stat");
	// send_recv();
	// send(sockfd, msg, strlen(msg) + 1, 0);
	// sendHelper(msg);
	// sendHelper(msg, totalSize);

	fprintf(stderr, "stat() called path %s\n", pathname);
	return orig_stat(pathname, statbuf);
}

int (*orig_unlink)(const char *pathname);

int unlink(const char *pathname)
{

	// char *msg = "unlink";

	// send_recv();
	// send(sockfd, msg, strlen(msg) + 1, 0);
	// sendHelper(msg);
	// sendHelper(msg, totalSize);
	fprintf(stderr, "unlink() called path %s \n", pathname);
	return orig_unlink(pathname);
}

ssize_t (*orig_getdirentries)(int fd, char *buf, size_t nbyte,
							  off_t *restrict basep);

ssize_t getdirentries(int fd, char *buf, size_t nbyte,
					  off_t *restrict basep)
{

	// char *msg = "getdirentries";
	//  send_recv();
	//   fprintf(stderr, "read\n");
	//  send(sockfd, msg, strlen(msg) + 1, 0);
	// sendHelper(msg);
	//  sendHelper(msg, totalSize);
	fprintf(stderr, "getdirentries() called fd %d with %ld bytes\n", fd, nbyte);
	return orig_getdirentries(fd, buf, nbyte, basep);
}

struct dirtreenode
{
	const char *name; // name of the directory
	int num_subdirs;  // number of subdirectories struct dirtreenode **subdirs; // pointer to array of
};					  // dirtreenode pointers, one for each subdirectory

struct dirtreenode *(*orig_getdirtree)(const char *path);

struct dirtreenode *getdirtree(const char *path)
{
	// char *msg = "getdirtree";

	// send(sockfd, msg, strlen(msg) + 1, 0);
	// sendHelper(msg);
	// sendHelper(msg, totalSize);
	// send_recv();

	fprintf(stderr, "getdirtree() called path %s \n", path);
	return orig_getdirtree(path);
}

void (*orig_freedirtree)(struct dirtreenode *dt);
void freedirtree(struct dirtreenode *dt)
{
	// char *msg = "freedirtree";

	// send(sockfd, msg, strlen(msg) + 1, 0);
	// sendHelper(msg);
	// send_recv();

	fprintf(stderr, "freedirtree() called  \n");
	return orig_freedirtree(dt);
}

// This function is automatically called when program is started
void _init(void)
{
	// set function pointer orig_open to point to the original open function
	orig_open = dlsym(RTLD_NEXT, "open");
	orig_read = dlsym(RTLD_NEXT, "read");
	orig_close = dlsym(RTLD_NEXT, "close");
	orig_write = dlsym(RTLD_NEXT, "write");
	orig_lseek = dlsym(RTLD_NEXT, "lseek");
	orig_stat = dlsym(RTLD_NEXT, "stat");
	orig_unlink = dlsym(RTLD_NEXT, "unlink");
	orig_getdirentries = dlsym(RTLD_NEXT, "getdirentries");
	orig_getdirtree = dlsym(RTLD_NEXT, "getdirtree");
	orig_freedirtree = dlsym(RTLD_NEXT, "freedirtree");
}