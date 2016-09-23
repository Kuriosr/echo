#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>

#define ECHO_SERVER_PORT 7791
#define MAX_LINE 1024
#define max(a,b) ( ((a)>(b)) ? (a):(b) )

void str_cli(FILE * fp, int sockfd)
{
    char sendline[MAX_LINE], receiveline[MAX_LINE];
    fd_set rset;
    FD_ZERO(&rset);

    int stdineof = 0;
    while(true)
    {
        if (0 == stdineof)
            FD_SET(fileno(fp), &rset);

        FD_SET(sockfd, &rset);
        int maxfdp = max(fileno(fp), sockfd) + 1;
        select(maxfdp, &rset, NULL, NULL, NULL);
        if (FD_ISSET(fileno(fp), &rset))
        {
            if (NULL == fgets(sendline, MAX_LINE, fp))
            {
                stdineof = 1;
                shutdown(sockfd, SHUT_WR);
                FD_CLR(fileno(fp), &rset);
                continue;
            }
            write(sockfd, sendline, strlen(sendline));
        }
        if (FD_ISSET(sockfd, &rset))
        {
            size_t readnum = read(sockfd, receiveline, MAX_LINE);
            if (0 == readnum)
            {
                if (1 != stdineof)
                {
                    printf("server terminated prematurely\n");
                }
                return;
            }
            receiveline[readnum] = '\0';
            fputs(receiveline, stdout);
        }
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
    str_cli(stdin, clientfd);
    // some text file can be test
    //FILE * p = fopen("doc.txt", "r");
    //if (NULL == p)
    //{
    //    printf("Error Open File\n");
    //    str_cli(p, clientfd);
    //}
    return 0;
}
