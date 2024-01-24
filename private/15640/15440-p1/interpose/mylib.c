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

#define MAXMSGLEN 100

int op; // operation ID
size_t totalSize = 0;

int (*orig_close)(int fildes);

void sendHelper(char *name, ssize_t totalSize)
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
		totalSent += send(sockfd, name, strlen(name) + 1, 0);
	}

	// send(sockfd, name, strlen(name) + 1, 0);

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

	op = 0;
	size_t n = strlen(pathname);
	// total size of msg: size_t total, int op, int flag, size_t n, char *path, mode_t m
	totalSize = 2 * sizeof(int) + 2 * sizeof(size_t) + n + sizeof(mode_t) + 1; // changed
	fprintf(stderr, "pathname: %s\n", pathname);
	// snprintf(buf, totalSize, "%zu%d%d%zu%s%o", totalSize, op, flags, n, pathname, m);
	int i = 0;
	memcpy(buf + i, &totalSize, sizeof(totalSize));
	fprintf(stderr, "open() called by path 1: %s \n buf: %s with total size %ld\n", pathname, buf, totalSize);
	i += sizeof(size_t);
	memcpy(buf + i, &op, sizeof(int));
	i += sizeof(int);
	memcpy(buf + i, &flags, sizeof(int));
	i += sizeof(int);
	memcpy(buf + i, &n, sizeof(size_t));
	i += sizeof(size_t);
	
	memcpy(buf + i, pathname, n);
	i += n;
	memcpy(buf + i, &m, sizeof(mode_t));
	buf[i] = '\0';
	fprintf(stderr, "message sent to server: %s", buf);

	fprintf(stderr, "open() called by path: %s \n message: %s\n", pathname, buf);
	// char *msg = "open";
	sendHelper(buf, totalSize);

	// free(buf);

	return orig_open(pathname, flags, m);
}

int close(int fildes)
{ // helper funciton to forward requests to server
	// msg = "close";

	char *msg = "close";
	// strcpy(msg, "close");
	// send_recv();
	// send(sockfd, msg, strlen(msg) + 1, 0);
	// sendHelper(msg);
	sendHelper(msg, totalSize);

	fprintf(stderr, "close() called fd %d \n", fildes);
	return orig_close(fildes); // function changed the server
}

ssize_t (*orig_read)(int fildes, void *buf, size_t nbyte);

ssize_t read(int fildes, void *buf, size_t nbyte)
{

	char *msg = "read";
	// send_recv();
	// send(sockfd, msg, strlen(msg) + 1, 0);
	// sendHelper(msg);
	sendHelper(msg, totalSize);

	fprintf(stderr, "read() called with %ld bytes\n", nbyte);
	return orig_read(fildes, buf, nbyte);
}

ssize_t (*orig_write)(int fildes, const void *buf, size_t nbyte);

ssize_t write(int fildes, const void *buf, size_t nbyte)
{
	char *msg = "write";

	// strcpy(msg, "write");
	// send_recv();
	// send(sockfd, msg, strlen(msg) + 1, 0);
	// sendHelper(msg);
	sendHelper(msg, totalSize);
	fprintf(stderr, "write() called fd %d with %ld bytes\n", fildes, nbyte);
	return orig_write(fildes, buf, nbyte);
}

ssize_t (*orig_lseek)(int fildes, off_t offset, int whence);

ssize_t lseek(int fildes, off_t offset, int whence)
{
	char *msg = "lseek";

	// strcpy(msg, "lseek");
	// send_recv();
	// send(sockfd, msg, strlen(msg) + 1, 0);
	// sendHelper(msg);
	sendHelper(msg, totalSize);

	fprintf(stderr, "lseek() called fd %d with %ld offset\n", fildes, offset);
	return orig_lseek(fildes, offset, whence);
}

int (*orig_stat)(const char *restrict pathname,
				 struct stat *restrict statbuf);

int stat(const char *restrict pathname, struct stat *restrict statbuf)
{
	char *msg = "stat";

	// strcpy(msg, "stat");
	// send_recv();
	// send(sockfd, msg, strlen(msg) + 1, 0);
	// sendHelper(msg);
	sendHelper(msg, totalSize);

	fprintf(stderr, "stat() called path %s\n", pathname);
	return orig_stat(pathname, statbuf);
}

int (*orig_unlink)(const char *pathname);

int unlink(const char *pathname)
{

	char *msg = "unlink";

	// send_recv();
	// send(sockfd, msg, strlen(msg) + 1, 0);
	// sendHelper(msg);
	sendHelper(msg, totalSize);
	fprintf(stderr, "unlink() called path %s \n", pathname);
	return orig_unlink(pathname);
}

ssize_t (*orig_getdirentries)(int fd, char *buf, size_t nbyte,
							  off_t *restrict basep);

ssize_t getdirentries(int fd, char *buf, size_t nbyte,
					  off_t *restrict basep)
{

	char *msg = "getdirentries";
	// send_recv();
	//  fprintf(stderr, "read\n");
	// send(sockfd, msg, strlen(msg) + 1, 0);
	// sendHelper(msg);
	sendHelper(msg, totalSize);
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
	char *msg = "getdirtree";

	// send(sockfd, msg, strlen(msg) + 1, 0);
	// sendHelper(msg);
	sendHelper(msg, totalSize);
	// send_recv();

	fprintf(stderr, "getdirtree() called path %s \n", path);
	return orig_getdirtree(path);
}

void (*orig_freedirtree)(struct dirtreenode *dt);
void freedirtree(struct dirtreenode *dt)
{
	char *msg = "freedirtree";

	// send(sockfd, msg, strlen(msg) + 1, 0);
	sendHelper(msg, totalSize);
	// send_recv();

	fprintf(stderr, "freedirtree() called  \n");
	return orig_freedirtree(dt);
}

// This function is automatically called when program is started
void _init(void)
{
	// set function pointer orig_open to point to the original open function
	orig_open = dlsym(RTLD_NEXT, "open");
	// orig_read = dlsym(RTLD_NEXT, "read");

	// orig_write = dlsym(RTLD_NEXT, "write");
	// orig_lseek = dlsym(RTLD_NEXT, "lseek");
	// orig_stat = dlsym(RTLD_NEXT, "stat");
	// orig_unlink = dlsym(RTLD_NEXT, "unlink");
	// orig_getdirentries = dlsym(RTLD_NEXT, "getdirentries");
	// orig_getdirtree = dlsym(RTLD_NEXT, "getdirtree");
	// orig_freedirtree = dlsym(RTLD_NEXT, "freedirtree");
	// orig_close = dlsym(RTLD_NEXT, "close");
}