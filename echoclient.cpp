#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/uio.h>
#include <arpa/inet.h>

#define ECHO_SERVER_PORT 7791
#define MAX_LINE 1024

void str_cli(FILE * fp, int sockfd)
{
    char sendline[MAX_LINE], receiveline[MAX_LINE];

    FILE * fRecp = fdopen(sockfd, "r");
    while(fgets(sendline, MAX_LINE, fp) != NULL)
    {
        write(sockfd, sendline, strlen(sendline));

        fgets(receiveline, MAX_LINE, fRecp);
        fputs(receiveline, stdout);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("usage: echoclient <IPAddress>\n");
        return 0;
    }
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in stServerAddr;
    memset(&stServerAddr, 0, sizeof(stServerAddr));
    stServerAddr.sin_family = AF_INET;
    stServerAddr.sin_port = htons(ECHO_SERVER_PORT);
    inet_pton(AF_INET, argv[1], &stServerAddr.sin_addr);

    if (0 != connect(clientfd, (struct sockaddr *)&stServerAddr, sizeof(stServerAddr)))
    {
        printf("Can not connect echoserver\n");
        return -1;
    }
    printf("dev_test branch");
    str_cli(stdin, clientfd);
    return 0;
}
