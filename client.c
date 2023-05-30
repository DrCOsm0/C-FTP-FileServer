#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>

#define MAXLINE 80

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Invalid args: ./client [file name] [ip] [port]\n");
        return -1;
    }
    char *file_name = argv[1];
    char *ip = argv[2];
    int port = atoi(argv[3]);
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[80] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address / Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    write(sock, file_name, MAXLINE);
    read(sock, buffer, MAXLINE);
    if(atoi(buffer) == 0)
    {
        printf("File not found\n");
        return -1;
    } 
     else if(atoi(buffer) == 1)
    {
        while(read(sock, buffer, MAXLINE) != 0)            
        {
            printf("%s", buffer);
            bzero(buffer,MAXLINE);
        }
    }
    close(sock);
    return 0;
}
