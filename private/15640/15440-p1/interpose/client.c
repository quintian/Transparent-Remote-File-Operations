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

void do_this(int i)
{
    char path[100];
    sprintf(path, "file_%d.txt", i);
    int fd = open(path, O_RDWR, 0);
    if (i % 2 == 0)
        sleep(1);
    else
        sleep(2);
    char msg[100];
    sprintf(msg, "Hello World %d!", i);
    printf("%s\n", msg);
    write(fd, msg, strlen(msg));
    close(fd);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Expected an id argument\n");
        return -1;
    }
    char *endptr;
    long rv = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0')
    {
        printf("Invalid id argument\n");
        return -1;
    }
    do_this(rv);
    return 0;
}

// int main()
// {
//     int num_clients = 10;
//     pid_t pids[num_clients];
//     for (int i = 0; i < num_clients; i++)
//     {

//             pids[i] = fork();
//             if (pids[i] == 0)
//             {
//                 doThis(i);
//                 return 0;
//             }
//         }
//         for (int i = 0; i < num_clients; i++)
//         {
//             waitpid(pids[i], NULL, 0);
//         }
//         return 0;

//     }

// void doThis(int i)
//         {
//             char path[100];
//             sprintf(path, "file_%d.txt", i);
//             int fd = open(path, ...);
//             if (i % 2 == 0)
//                 sleep(1);
//             else
//                 sleep(2);
//         char msg[100];
// sprintf(msg, "Hello World %d!", i);
// printf("%s\n", msg);
// write(fd, msg, strlen(msg));
// close(fd);
//         }