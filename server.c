#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/stat.h>

#define MAXLINE 80

void z_handler(int signum) {
    wait(NULL);
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./server <port>\n");
        return -1;
    }
    int port = atoi(argv[1]);
    int server, new_socket;
    struct sockaddr_in address;
    int set = 1;
    int addrlen = sizeof(address);
    char buffer[80] = {0};
    pid_t pid;

    if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        if ((new_socket = accept(server, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        if ((pid = fork()) == 0)
        {
            char a[MAXLINE];
            FILE *fp;
            read(new_socket, a, MAXLINE);
            struct sockaddr_in sin;
            socklen_t len = sizeof(sin);
            if (getsockname(new_socket, (struct sockaddr *)&sin, &len) == -1) {
                perror("getsockname");
                exit(EXIT_FAILURE);
            } else {
                printf("Got a client: IP address: %s, Port Number: %d\n", inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
            }
            if ((fp = fopen(a, "r"))) {
                write(new_socket, "1", MAXLINE);
            } else {
                write(new_socket, "0", MAXLINE);
                exit(1);
            }

            while (fgets(a, sizeof(a), fp)) 
            {
                if ((write(new_socket, a, MAXLINE)) == -1)
                    printf("Error sending server side\n");
            }
            fclose(fp);
            close(new_socket);
            exit(0);
        } else {
            signal(SIGCHLD, z_handler);
            close(new_socket);
        }
    }
    return 0;
}
