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

#define MAXMSGLEN 100

/*
void read_parse_request_to_buf();
void parse_add_headers(parser_t *parser, char *buf_global, char *buf_header,
					   const char *host, const char *port)
void retrieve_request_to_buf();
int send_response_to_client();
int fd = open(pathname, flags, ...);

...

void *thread()
*/
size_t totalReceive = 0;
size_t totalSize = 0;
void receiveHelper(char *buf, int sessfd)
{

	memcpy(&totalSize, buf, sizeof(size_t));
	totalReceive += sizeof(size_t);

	while (totalReceive < totalSize)
	{
		totalReceive += recv(sessfd, buf + totalReceive, totalSize - totalReceive, 0);
	}
	fprintf(stderr, "totalSize: %ld\ntotalReceive: %ld\n", totalSize, totalReceive);
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

void openHelper(char *buf)
{

	int i = 0;
	// size_t totalSize;
	int op;
	int flags;
	size_t n;
	mode_t m;

	// memcpy(&totalSize, buf, sizeof(size_t));
	i += sizeof(size_t); // offset for totalSize at the beginning

	memcpy(&op, buf + i, sizeof(int));
	i += sizeof(int);
	memcpy(&flags, buf + i, sizeof(int));
	i += sizeof(int);
	memcpy(&n, buf + i, sizeof(size_t));
	i += sizeof(size_t);
	char pathname[1024]; // changed
	memcpy(pathname, buf + i, n);
	i += n;
	memcpy(&m, buf + i, sizeof(mode_t));

	// fprintf(stderr, "totalSize: %ld\n", totalSize);
	fprintf(stderr, "op: %d\n", op);
	fprintf(stderr, "flags: %d\n", flags);
	fprintf(stderr, "n: %zu\n", n);
	fprintf(stderr, "pathname: %s\n", pathname);
	fprintf(stderr, "flags: %d\n", flags);
	fprintf(stderr, "m: %o\n", m);
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
		fprintf(stderr, "total receive before first loop:  %ld\n", totalReceive);
		while (totalReceive < sizeof(size_t))
		{
			fprintf(stderr, "here:130 ");

			totalReceive += recv(sessfd, buf + totalReceive, sizeof(size_t) - totalReceive, 0);
		}

		memcpy(&totalSize, buf, sizeof(size_t));
		fprintf(stderr, "total size after first loop:  %ld\n, totalReceive: %ld", totalSize, totalReceive);

		// totalReceive += sizeof(size_t);
		while (totalReceive < totalSize)

		{
			fprintf(stderr, "here:144 ");
			totalReceive += recv(sessfd, buf + totalReceive, totalSize - totalReceive, 0);
		}
		fprintf(stderr, "totalSize: %ld\ntotalReceive: %ld\n", totalSize, totalReceive);

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
		char pathname[n]; // changed so print correctly!
		memcpy(pathname, buf + i, n);
		// pathname[3]="\0";
		i += n;
		memcpy(&m, buf + i, sizeof(mode_t));

		// // fprintf(stderr, "totalSize: %ld\n", totalSize);
		fprintf(stderr, "op: %d\n", op);
		fprintf(stderr, "flags: %d\n", flags);
		fprintf(stderr, "n: %zu\n", n);
		fprintf(stderr, "pathname: %s\n", pathname);
		fprintf(stderr, "m: %o\n", m);

		// openHelper(buf);

		int fd = open(pathname, flags, m) + MAXMSGLEN;
		int e;
		if (fd == -1)
		{
			e = errno; // errno is set by function call
		}

		else
		{
			fd += 100;
		}

		memcpy(buf, &fd, sizeof(int));
		memcpy(buf + sizeof(int), &e, sizeof(int));
		fprintf(stderr, "msg returned fd: %d\n", fd);
		send(sessfd, buf, sizeof(int), 0);

		// when close sessfd??

		//  either client closed connection, or error
		if (rv < 0)
			err(1, 0);
		close(sessfd); // close one client
	}
	// close socket
	close(sockfd);

	return 0;
}
