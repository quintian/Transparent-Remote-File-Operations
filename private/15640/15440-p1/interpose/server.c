#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>

#define MAXMSGLEN 100
#define FDADD 1000

/*

*/
size_t receiveTotalSize(int sessfd)
{
	char buf[sizeof(size_t)];
	size_t totalSize;
	size_t totalReceive = 0;
	while (totalReceive < sizeof(size_t))
	{
		// fprintf(stderr, "here:130 \n");

		totalReceive += recv(sessfd, buf + totalReceive, sizeof(size_t) - totalReceive, 0);
	}

	memcpy(&totalSize, buf, sizeof(size_t));
	fprintf(stderr, "total size after first loop:  %ld\n, totalReceive: %ld", totalSize, totalReceive);
	return totalSize - sizeof(size_t);
}
// receive the full length msg from client
void receiveHelper(char *buf, int sessfd, size_t totalSize)
{
	// size_t totalSize;
	size_t totalReceive = 0;
	// while (totalReceive < sizeof(size_t))
	// {
	// 	totalReceive += recv(sessfd, buf + totalReceive, sizeof(size_t) - totalReceive, 0);
	// }
	// memcpy(&totalSize, buf, sizeof(size_t));

	// totalReceive=0;
	while (totalReceive < totalSize)
	{
		// fprintf(stderr, "here:144 \n");
		totalReceive += recv(sessfd, buf + totalReceive, totalSize - totalReceive, 0);
	}
	fprintf(stderr, "totalSize: %ld\ntotalReceive: %ld\n", totalSize, totalReceive);
	// return buf;
}

void recvVariable(char *buf, int sessfd, size_t vSize)
{
	// memcpy(&totalSize, buf, sizeof(size_t));
	// totalReceive += sizeof(size_t);
	size_t totalReceive = 0;

	while (totalReceive < vSize)
	{
		totalReceive += recv(sessfd, buf + totalReceive, vSize - totalReceive, 0);
	}
	fprintf(stderr, "vSize: %ld\ntotalReceive: %ld\n", vSize, totalReceive);
}

void openHelper(char *buf, int sessfd)
{
	int i = 0;
	// size_t totalSize;
	int op;
	int flags;
	size_t n;
	mode_t m;

	// //op=*(int*)buf,
	// memcpy(&totalSize,buf, sizeof(size_t));
	// i += sizeof(size_t); // offset for totalSize at the beginning

	// memcpy(&op, buf + i, sizeof(int));
	op = *(int *)(buf + i); // 1. op
	i += sizeof(int);
	memcpy(&flags, buf + i, sizeof(int)); // 2. flags
	i += sizeof(int);
	memcpy(&n, buf + i, sizeof(size_t)); // 3.n
	i += sizeof(size_t);

	char pathname[n + 1];		  // changed so print correctly!
	memcpy(pathname, buf + i, n); // 4. pathname
	pathname[n] = 0;			  // need 0 for function argument
	i += n;

	// memcpy(&m, buf + i, sizeof(mode_t)); //5. m
	m = *(mode_t *)(buf + i);

	// // fprintf(stderr, "totalSize: %ld\n", totalSize);
	fprintf(stderr, "op: %d\n", op);
	fprintf(stderr, "flags: %d\n", flags);
	fprintf(stderr, "n: %zu\n", n);
	fprintf(stderr, "pathname: %s\n", pathname);
	fprintf(stderr, "m: %o\n", m);

	// call open() and put result in buf, send back to client

	int fd = open(pathname, flags, m);
	int e = errno;

	// if (fd != -1)
	// {
	// 	fd += FDADD;
	// }
	char buf2[2 * sizeof(int)]; // buf2 to hold msg back to clent
	memcpy(buf2, &fd, sizeof(int));
	memcpy(buf2 + sizeof(int), &e, sizeof(int));
	fprintf(stderr, "msg returned fd, e: %d, %d\n", fd, e);
	send(sessfd, buf2, sizeof(int) * 2, 0);
}

void closeHelper(char *buf, int sessfd)
{
	int i = 0;
	// size_t totalSize;
	int op;
	int fd;

	// //op=*(int*)buf,
	// memcpy(&totalSize,buf, sizeof(size_t));
	// i += sizeof(size_t); // offset for totalSize at the beginning

	// memcpy(&op, buf + i, sizeof(int));
	op = *(int *)(buf + i);
	i += sizeof(int);
	memcpy(&fd, buf + i, sizeof(int));
	// fd -= FDADD;

	// // fprintf(stderr, "totalSize: %ld\n", totalSize);
	fprintf(stderr, "op: %d\n", op);
	fprintf(stderr, "fd: %d\n", fd);

	// call open() and put result in buf, send back to client

	int closeReturn = close(fd);
	int e = errno;

	char buf2[2 * sizeof(int)]; // buf2 to hold msg back to clent
	memcpy(buf2, &closeReturn, sizeof(int));
	memcpy(buf2 + sizeof(int), &e, sizeof(int));
	fprintf(stderr, "msg returned fd, e: %d, %d\n", closeReturn, e);
	send(sessfd, buf2, sizeof(int) * 2, 0);
}
// test write(): 440write, then on client side, type text to write, then control d, control c
void writeHelper(char *buf, int sessfd)
{
	int i = 0;
	// size_t totalSize;
	int op;
	int fd;
	size_t nbyte;
	// void *bufToWrite;

	// //op=*(int*)buf,
	// memcpy(&totalSize,buf, sizeof(size_t));
	// i += sizeof(size_t); // offset for totalSize at the beginning

	// memcpy(&op, buf + i, sizeof(int));
	op = *(int *)(buf + i);
	i += sizeof(int);
	memcpy(&fd, buf + i, sizeof(int));
	// fd -= FDADD;
	i += sizeof(int);
	memcpy(&nbyte, buf + i, sizeof(size_t));
	i += sizeof(size_t);
	char bufToWrite[nbyte + 1];
	memcpy(bufToWrite, buf + i, nbyte);
	// memcpy(pathname, buf + i, n);
	bufToWrite[nbyte] = 0;
	fprintf(stderr, "bufToWrite: %s\n", bufToWrite);
	fprintf(stderr, "op: %d\n", op);
	fprintf(stderr, "fd: %d\n", fd);

	// call open() and put result in buf, send back to client

	int writeReturn = write(fd, bufToWrite, nbyte);
	int e = errno;

	char buf2[2 * sizeof(int)]; // buf2 to hold msg back to clent

	memcpy(buf2, &writeReturn, sizeof(int));
	memcpy(buf2 + sizeof(int), &e, sizeof(int));
	fprintf(stderr, "msg returned fd, e: %d, %d\n", writeReturn, e);
	send(sessfd, buf2, sizeof(int) * 2, 0);
}

int main(int argc, char **argv)
{
	// char *msg = "Hello from server";
	// char buf[MAXMSGLEN + 1];
	// char buf[2000];
	char *serverport;
	unsigned short port;
	int sockfd, sessfd, rv;
	struct sockaddr_in srv, cli;
	socklen_t sa_size;

	// Get environment variable indicating the port of the server
	serverport = getenv("serverport15440");
	if (serverport)
		port = (unsigned short)atoi(serverport);
	else
		port = 15440;

	// Create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP/IP socket
	if (sockfd < 0)
		err(1, 0); // in case of error

	// setup address structure to indicate server port
	memset(&srv, 0, sizeof(srv));			 // clear it first
	srv.sin_family = AF_INET;				 // IP family
	srv.sin_addr.s_addr = htonl(INADDR_ANY); // don't care IP address
	srv.sin_port = htons(port);				 // server port

	// bind to our port
	rv = bind(sockfd, (struct sockaddr *)&srv, sizeof(struct sockaddr));
	if (rv < 0)
		err(1, 0);

	// start listening for connections
	rv = listen(sockfd, 5);
	if (rv < 0)
		err(1, 0);

	// main server loop, handle clients one at a time, quit after 10 clients
	while (1)
	{
		// wait for next client, get session socket
		sa_size = sizeof(struct sockaddr_in);
		sessfd = accept(sockfd, (struct sockaddr *)&cli, &sa_size);
		if (sessfd < 0)
			err(1, 0);

		// receive requests and send replies to this client
		size_t totalSize = receiveTotalSize(sessfd);
		char buf[totalSize];
		receiveHelper(buf, sessfd, totalSize);
		// get operation code, then handle the request
		int op = *(int *)(buf);
		switch (op)
		{
		case 0:
			openHelper(buf, sessfd);
			break;
		case 1:
			closeHelper(buf, sessfd);
			break;
		case 2:
			writeHelper(buf, sessfd);
			break;
		default:
			break;
		}

		//  either client closed connection, or error
		if (rv < 0)
			err(1, 0);
		close(sessfd); // close one client
	}
	// close socket
	close(sockfd);

	return 0;
}
