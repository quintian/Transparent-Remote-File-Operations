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
#include <sys/stat.h>
#include <dirent.h>
#include "../include/dirtree.h"

#define MAXMSGLEN 100
#define FDADD 1000

/*

*/
size_t receiveTotalSize(int sessfd)
{
	char buf[sizeof(size_t)];
	size_t totalSize;
	size_t totalReceive = 0;
	// size_t recv_once;
	while (totalReceive < sizeof(size_t))
	{
		// fprintf(stderr, "here:130 \n");
		// recv_once = recv(sessfd, buf + totalReceive, sizeof(size_t) - totalReceive, 0);
		// if (recv_once < 0)
		// {
		// 	break;
		// }
		// else
		// {
		// 	totalReceive += recv_once;
		// }
		totalReceive += recv(sessfd, buf + totalReceive, sizeof(size_t) - totalReceive, 0);
	}

	memcpy(&totalSize, buf, sizeof(size_t));
	fprintf(stderr, "total size after receiveTotalSize:  %ld\n, totalReceive: %ld\n", totalSize, totalReceive);
	return totalSize - sizeof(size_t);
}
// receive the full length msg from client
void receiveHelper(char *buf, int sessfd, size_t totalSize)
{
	// size_t totalSize;
	fprintf(stderr, "\nreceiveHelper() called\n");
	size_t totalReceive = 0;
	// size_t recv_once;

	while (totalReceive < totalSize)
	{
		// fprintf(stderr, "here:144 \n");
		// recv_once = recv(sessfd, buf + totalReceive, totalSize - totalReceive, 0);
		// if (recv_once < 0)
		// {
		// 	break;
		// }
		// else
		// {
		// 	totalReceive += recv_once;
		// }
		totalReceive += recv(sessfd, buf + totalReceive, totalSize - totalReceive, 0);
	}
	fprintf(stderr, "totalSize: %ld\ntotalReceive: %ld\n", totalSize, totalReceive);
	// return buf;
}

void sendHelper(int sessfd, char *buf, size_t totalSize, int flags)
{
	size_t totalSent = 0;
	while (totalSent < totalSize)
	{
		totalSent += send(sessfd, buf + totalSent, totalSize - totalSent, flags); // changed!
	}
	fprintf(stderr, "totalSent: %ld \n", totalSent);
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
	fprintf(stderr, "\nwriteHelper() called\n");
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

	ssize_t writeReturn = write(fd, bufToWrite, nbyte);
	int e = errno;

	char buf2[sizeof(int) + sizeof(ssize_t)]; // buf2 to hold msg back to clent

	memcpy(buf2, &writeReturn, sizeof(int));
	memcpy(buf2 + sizeof(int), &e, sizeof(int));
	fprintf(stderr, "msg returned fd, e: %zd, %d\n", writeReturn, e);
	send(sessfd, buf2, sizeof(int) * 2, 0);
}

void readHelper(char *buf, int sessfd)
{
	fprintf(stderr, "\nreadHelper() called\n");
	int i = 0;

	int op;
	int fd;
	size_t nbyte;

	op = *(int *)(buf + i);
	i += sizeof(int);
	memcpy(&fd, buf + i, sizeof(int));
	// fd -= FDADD;
	i += sizeof(int);
	memcpy(&nbyte, buf + i, sizeof(size_t));
	// i += sizeof(size_t);
	char bufRead[nbyte + 1];

	fprintf(stderr, "op: %d\n", op);
	fprintf(stderr, "fd: %d\n", fd);
	fprintf(stderr, "nbyte: %ld\n", nbyte);

	// call function and put result in buf, send back to client
	ssize_t read_nbyte = read(fd, bufRead, nbyte);
	// size_t read_nbyte = 0;
	//  while (read_nbyte < nbyte)
	//  {
	//  	read_nbyte += read(fd, bufRead + read_nbyte, nbyte - read_nbyte);
	//  	fprintf(stderr, "read_nbyte: %ld\n", read_nbyte);
	//  }
	fprintf(stderr, "read_nbyte: %zd\n", read_nbyte);
	int e = errno;

	bufRead[read_nbyte] = 0;

	char buf2[2 * sizeof(int) + read_nbyte + 1]; // buf2 to hold return msg for clent

	memcpy(buf2, &read_nbyte, sizeof(ssize_t));
	memcpy(buf2 + sizeof(ssize_t), &e, sizeof(int));
	memcpy(buf2 + sizeof(int) + sizeof(ssize_t), bufRead, read_nbyte); // read content is copied
	buf2[sizeof(ssize_t) + sizeof(int) + read_nbyte] = 0;			   // null terminator at the end of buf2

	fprintf(stderr, "returned to client nbytes read, e: %ld, %d\n, read content: %s\n", read_nbyte, e, bufRead);
	sendHelper(sessfd, buf2, sizeof(int) + sizeof(size_t) + read_nbyte, 0);
}

void lseekHelper(char *buf, int sessfd)
{
	fprintf(stderr, "\nlseekHelper() called\n");
	int i = 0;
	// size_t totalSize;
	int op;
	int fd;
	off_t offset;
	int whence;

	op = *(int *)(buf + i);
	i += sizeof(int);
	memcpy(&fd, buf + i, sizeof(int));
	// fd -= FDADD;
	i += sizeof(int);
	memcpy(&offset, buf + i, sizeof(off_t));
	i += sizeof(off_t);
	memcpy(&whence, buf + i, sizeof(int));

	// bufRead[nbyte] = 0;
	// fprintf(stderr, "bufToWrite: %s\n", bufToWrite);
	fprintf(stderr, "op: %d\n", op);
	fprintf(stderr, "fd: %d\n", fd);
	fprintf(stderr, "offset: %zo\n", offset);
	fprintf(stderr, "whence: %d\n", whence);

	// call open() and put result in buf, send back to client

	ssize_t returned_offset = lseek(fd, offset, whence);
	fprintf(stderr, "returned_offset: %zd\n", returned_offset);

	int e = errno;

	char buf2[sizeof(int) + sizeof(ssize_t)]; // buf2 to hold return msg for clent
	memcpy(buf2, &returned_offset, sizeof(ssize_t));
	memcpy(buf2 + sizeof(ssize_t), &e, sizeof(int));

	fprintf(stderr, "returned to client offset: %zd \ne: %d\n ", returned_offset, e);
	sendHelper(sessfd, buf2, sizeof(int) + sizeof(ssize_t), 0);
}

void statHelper(char *buf, int sessfd)
{
	fprintf(stderr, "\nstatHelper() called\n");
	int i = 0;
	int op;
	// int fd;
	size_t pathlen;

	op = *(int *)(buf + i);

	i += sizeof(int);
	memcpy(&pathlen, buf + i, sizeof(size_t));
	fprintf(stderr, "pathlen: %ld\n", pathlen);

	i += sizeof(size_t);
	char pathname[pathlen + 1];
	memcpy(pathname, buf + i, pathlen); // 4. pathname
	pathname[pathlen] = 0;				// add null terminator

	fprintf(stderr, "op: %d\n", op);
	// fprintf(stderr, "fd: %d\n", fd);
	fprintf(stderr, "pathlen: %ld\n", pathlen);
	fprintf(stderr, "pathname: %s\n", pathname);

	// call stat() and put result in statbuf, send back to client

	struct stat *statbuf = malloc(sizeof(struct stat));

	int stat_returned = stat(pathname, statbuf);

	fprintf(stderr, "statbuf: %s\n", (char *)statbuf);
	int e = errno;

	char buf2[2 * sizeof(int) + sizeof(statbuf)]; // buf2 to hold return msg for clent

	memcpy(buf2, &stat_returned, sizeof(int));
	memcpy(buf2 + sizeof(int), &e, sizeof(int));
	memcpy(buf2 + 2 * sizeof(int), statbuf, sizeof(struct stat)); //??
	free(statbuf);

	fprintf(stderr, "returned to client stat_returned, e: %d, %d\n, statbuf: %s\n", stat_returned, e, (char *)statbuf);
	sendHelper(sessfd, buf2, 2 * sizeof(int) + sizeof(struct stat), 0);
}

void unlinkHelper(char *buf, int sessfd)
{
	fprintf(stderr, "\n unlinkHelper() called\n");
	int i = 0;

	int op;
	// int fd;
	size_t pathlen;

	op = *(int *)(buf + i);

	i += sizeof(int);
	memcpy(&pathlen, buf + i, sizeof(size_t));
	fprintf(stderr, "pathlen: %ld\n", pathlen);

	i += sizeof(size_t);
	char pathname[pathlen + 1];
	memcpy(pathname, buf + i, pathlen); // 4. pathname
	pathname[pathlen] = 0;				// add null terminator

	fprintf(stderr, "op: %d\n", op);
	// fprintf(stderr, "fd: %d\n", fd);
	fprintf(stderr, "pathlen: %ld\n", pathlen);
	fprintf(stderr, "pathname: %s\n", pathname);

	// call stat() and put result in statbuf, send back to client

	int unlink_returned = unlink(pathname);

	int e = errno;

	char buf2[2 * sizeof(int)]; // buf2 to hold return msg for clent

	memcpy(buf2, &unlink_returned, sizeof(int));
	memcpy(buf2 + sizeof(int), &e, sizeof(int));

	fprintf(stderr, "returned to client unlink_returned, e: %d, %d\n", unlink_returned, e);
	sendHelper(sessfd, buf2, 2 * sizeof(int), 0);
}

void getdirentriesHelper(char *buf, int sessfd)
{
	fprintf(stderr, "\ngetdirentriesHelper() called\n");
	int i = 0;
	int op;
	int fd;
	size_t nbyte;
	off_t *basep = malloc(sizeof(off_t));

	op = *(int *)(buf + i);
	i += sizeof(int);
	memcpy(&fd, buf + i, sizeof(int));
	// fd -= FDADD;
	i += sizeof(int);
	memcpy(&nbyte, buf + i, sizeof(size_t));

	i += sizeof(size_t);
	memcpy(basep, buf + i, sizeof(off_t));

	char bufRead[nbyte + 1];

	fprintf(stderr, "op: %d\n", op);
	fprintf(stderr, "fd: %d\n", fd);
	fprintf(stderr, "nbyte: %ld\n", nbyte);
	fprintf(stderr, "basep: %zo\n", *(off_t *)basep);

	// call function and put result in buf, send back to client
	ssize_t read_nbyte = getdirentries(fd, bufRead, nbyte, basep);

	fprintf(stderr, "read_nbyte: %zd\n", read_nbyte);
	int e = errno;

	bufRead[read_nbyte] = 0;

	char buf2[2 * sizeof(int) + read_nbyte + 1]; // buf2 to hold return msg for clent

	memcpy(buf2, &read_nbyte, sizeof(ssize_t));
	memcpy(buf2 + sizeof(ssize_t), &e, sizeof(int));
	memcpy(buf2 + sizeof(int) + sizeof(ssize_t), bufRead, read_nbyte); // read content is copied
	buf2[sizeof(ssize_t) + sizeof(int) + read_nbyte] = 0;			   // null terminator at the end of buf2

	fprintf(stderr, "returned to client nbytes read, e: %ld, %d\n, direntries content: %s\n", read_nbyte, e, bufRead);
	sendHelper(sessfd, buf2, sizeof(int) + sizeof(size_t) + read_nbyte, 0);
	free(basep);
}

// struct dirtreenode *getdirtree(const char *path)
// {

// }

// initialize buf, i=0;
int getTreeSize(struct dirtreenode *dirtree)
{
	int totalSize = 0;
	struct dirtreenode *cursor = dirtree;

	totalSize += sizeof(int); // set the room for name_len
	totalSize += strlen(cursor->name);
	totalSize += sizeof(int); // set space for num_subdirs
	if (cursor->num_subdirs > 0)
	{
		for (int i = 0; i < cursor->num_subdirs; i++)
		{
			// int offset = i * sizeof(struct dirtreenode *);
			// cursor = cursor->subdirs + offset;
			// totalSize += strlen(cursor->name);
			totalSize += getTreeSize(cursor->subdirs[i]);
		}
	}
	return totalSize;
}

void deconstruct_tree(struct dirtreenode *dirtree, char *buf, int offset)
{
	// int offset = 0;
	struct dirtreenode *cursor = dirtree;
	int name_len = (int)strlen(cursor->name);
	memcpy(buf + offset, &name_len, sizeof(int)); // copy name_len
	offset += sizeof(int);
	memcpy(buf + offset, cursor->name, strlen(cursor->name));
	offset += strlen(cursor->name);
	memcpy(buf + offset, &cursor->num_subdirs, sizeof(int));

	fprintf(stderr, "tree deconstruct got name: %s \n", cursor->name + '\0');

	if (cursor->num_subdirs > 0)
	{
		for (int i = 0; i < cursor->num_subdirs; i++)
		{
			// int offset
			// cursor = cursor->subdirs + i * sizeof(struct dirtreenode *);
			// totalSize += strlen(cursor->name);
			return deconstruct_tree(cursor->subdirs[i], buf, offset);
		}
	}
	else
	{
		return;
	}
}

void getdirtreeHelper(char *buf, int sessfd)
{
	fprintf(stderr, "\ngetdirtreeHelper() called\n");
	int i = 0;

	int op;
	// int fd;
	size_t pathlen;

	op = *(int *)(buf + i);

	i += sizeof(int);
	memcpy(&pathlen, buf + i, sizeof(size_t));
	fprintf(stderr, "pathlen: %ld\n", pathlen);

	i += sizeof(size_t);
	char pathname[pathlen];
	memcpy(pathname, buf + i, pathlen); // 4. pathname
	// pathname[pathlen] = 0;				// add null terminator

	fprintf(stderr, "op: %d\n", op);
	// fprintf(stderr, "fd: %d\n", fd);
	fprintf(stderr, "pathlen: %ld\n", pathlen);
	fprintf(stderr, "pathname: %s\n", pathname);

	struct dirtreenode *dirtree = getdirtree(pathname);
	int e = errno;

	int treeSize = getTreeSize(dirtree);
	char buf2[treeSize + 2 * sizeof(int)];

	memcpy(buf2, &treeSize, sizeof(int));
	memcpy(buf2 + sizeof(int), &e, sizeof(int));
	deconstruct_tree(dirtree, buf2 + 2 * sizeof(int), 0);

	sendHelper(sessfd, buf2, 2 * sizeof(int) + treeSize, 0);

	fprintf(stderr, "returned to client nbytes in tree, e: %d, %d\n, tree buf: %s\n", treeSize, e, buf2);
	
	freedirtree(dirtree);
}

int main(int argc, char **argv)
{
	// char *msg = "Hello from server";
	// char buf[MAXMSGLEN + 1];

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

		rv = fork();
		if (rv == 0) // child process

		{
			close(sockfd); // child does not need this
			while (1)
			{
				// child process
				fprintf(stderr, "child process \n");

				// do_stuff(sessfd); // handle client session
				//  receive requests and send replies to this client
				size_t totalSize = receiveTotalSize(sessfd);
				if (totalSize < 0)
				{ // added for end of the client
					break;
				}
				char buf[totalSize];
				receiveHelper(buf, sessfd, totalSize);
				// get operation code, then handle the request
				int op = *(int *)(buf);
				switch (op)
				{
				case 0:
					openHelper(buf, sessfd);
					fprintf(stderr, "open \n");
					break;
				case 1:
					closeHelper(buf, sessfd);
					break;
				case 2:
					writeHelper(buf, sessfd);
					break;
				case 3:
					readHelper(buf, sessfd);
					break;
				case 4:
					lseekHelper(buf, sessfd);
					break;
				case 5:
					statHelper(buf, sessfd);
					break;
				case 6:
					unlinkHelper(buf, sessfd);
					break;
				case 7:
					getdirentriesHelper(buf, sessfd);
					break;
				default:
					break;
				}
			}
			//  either client closed connection, or error
			if (rv < 0)
				err(1, 0);
			close(sessfd); // close one client

			exit(0); // then exit
		}
		// close(sessfd); // parent does not need this // should loop back to accept next client
	}
	// close socket
	close(sockfd);

	return 0;
}
