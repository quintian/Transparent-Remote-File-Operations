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
question: 1. for send, recv, better to use the same buf? 2. seperate connect, send, recv on client side? 3. when to close session fd on server? 4. rv < 0, err()? 5. fork() for cp 2?
5. what to return for client open() client?
6. recv all or one variable once?

...

void *thread()
*/

size_t totalSize = 0;
// receive the full length msg from client
void receiveHelper(char *buf, int sessfd)
{
	size_t totalReceive = 0;
	while (totalReceive < sizeof(size_t))
	{
		fprintf(stderr, "here:130 ");

		totalReceive += recv(sessfd, buf + totalReceive, sizeof(size_t) - totalReceive, 0);
	}

	memcpy(&totalSize, buf, sizeof(size_t));
	fprintf(stderr, "total size after first loop:  %ld\n, totalReceive: %ld", totalSize, totalReceive);

	while (totalReceive < totalSize)

	{
		fprintf(stderr, "here:144 ");
		totalReceive += recv(sessfd, buf + totalReceive, totalSize - totalReceive, 0);
	}
	fprintf(stderr, "totalSize: %ld\ntotalReceive: %ld\n", totalSize, totalReceive);
	// size_t totalReceive = 0;
	// memcpy(&totalSize, buf, sizeof(size_t));
	// totalReceive += sizeof(size_t);

	// while (totalReceive < totalSize)
	// {
	// 	totalReceive += recv(sessfd, buf + totalReceive, totalSize - totalReceive, 0);
	// }
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
	i += sizeof(size_t); // offset for totalSize at the beginning

	// memcpy(&op, buf + i, sizeof(int));
	op = *(int *)(buf + i);
	i += sizeof(int);
	memcpy(&flags, buf + i, sizeof(int));
	i += sizeof(int);
	memcpy(&n, buf + i, sizeof(size_t));
	i += sizeof(size_t);
	char pathname[n + 1]; // changed so print correctly!
	memcpy(pathname, buf + i, n);
	fprintf(stderr, "pathname: %s\n", pathname);
	pathname[n] = 0; // need 0 for function argument
	i += n;
	memcpy(&m, buf + i, sizeof(mode_t));

	// // fprintf(stderr, "totalSize: %ld\n", totalSize);
	fprintf(stderr, "op: %d\n", op);
	fprintf(stderr, "flags: %d\n", flags);
	fprintf(stderr, "n: %zu\n", n);
	fprintf(stderr, "pathname: %s\n", pathname);
	fprintf(stderr, "m: %o\n", m);

	// call open() and put result in buf, send back to client

	int fd = open(pathname, flags, m);
	int e = errno;

	if (fd != -1)
	{
		fd += FDADD;
	}

	memcpy(buf, &fd, sizeof(int));
	memcpy(buf + sizeof(int), &e, sizeof(int));
	fprintf(stderr, "msg returned fd, e: %d, %d\n", fd, e);
	send(sessfd, buf, sizeof(int) * 2, 0);
}

int main(int argc, char **argv)
{
	// char *msg = "Hello from server";
	char buf[MAXMSGLEN + 1];
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

		// get messages and send replies to this client, until it goes away
		// while ((rv = recv(sessfd, buf, MAXMSGLEN, 0)) > 0)
		// {
		// 	buf[rv] = 0; // null terminate string to print

		// 	fprintf(stderr, "received msg: %s\n", buf);
		// 	// receiveHelper(buf, sessfd); // 2 while loop??

		// 	// send reply
		// 	// printf("server replying to client: %s\n", msg);
		// 	// printf("server replying to client: %s\n", buf); //changed!!
		// 	// send(sessfd, msg, strlen(msg), 0); // should check return value
		// }

		receiveHelper(buf, sessfd);

		int op = *(int *)(buf + sizeof(size_t));
		switch (op)
		{
		case 0:
			openHelper(buf, sessfd);
			break;
		case 1:

			break;
		default:
			printf("Invalid op");
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
