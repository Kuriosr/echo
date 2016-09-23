/*
 * Author : watxon75@gmail.com
 * Last modified : 2016-09-23 15:52
 * Filename : echoserver_select.cpp
 * Description : echoserver by select
*/
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#define ECHO_SERVER_PORT 7791
#define MAX_LINE 1024
#define MAX_FDSET 32 

int InitListen()
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in stServerAddr;
    memset(&stServerAddr, 0, sizeof(stServerAddr));
    stServerAddr.sin_family = AF_INET;
    stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    stServerAddr.sin_port = htons(ECHO_SERVER_PORT);
    if (0 != bind(listenfd, (struct sockaddr *)&stServerAddr, sizeof(stServerAddr)))
    {
        printf("Bind Socket Error\n");
        return -1;
    }
    listen(listenfd, 5);
    return listenfd;
}

int main()
{
    int connfd = 0;
    char receiveline[MAX_LINE];
    int listenfd = InitListen();
    if (listenfd <= 0)
        return -1;
    int connectfd[MAX_FDSET];
    for (int i = 0; i < MAX_FDSET; ++i)
        connectfd[i] = -1;
    struct sockaddr_in stClientAddr;
    socklen_t clilen;
    fd_set allSet;
    FD_ZERO(&allSet);
    FD_SET(listenfd, &allSet);
    int maxfd = listenfd;
    int i, maxi = 0;
    while(true)
    {
        fd_set readSet = allSet;

        int selectRes = 0;
        selectRes = select(maxfd + 1, &readSet, NULL, NULL, NULL);
        if (selectRes < 0)
        {
            printf("select Error\n");
            return -1;
        }
        if (FD_ISSET(listenfd, &readSet))
        {
            clilen = sizeof(stClientAddr);
            connfd = accept(listenfd, (struct sockaddr *)&stClientAddr, &clilen);
            for (i = 0; i < MAX_FDSET; ++i)
            {
                if (connectfd[i] < 0)
                {
                    connectfd[i] = connfd;
                    break;
                }
            }
            if (i == MAX_FDSET)
            {
                printf("Too Many Clients\n");
                return -1;
            }
            maxi = (i > maxi) ? i : maxi;
            FD_SET(connectfd[i], &allSet);
            maxfd = (connectfd[i] > maxfd) ? connectfd[i] : maxfd;
            if (--selectRes <= 0)
                continue;
        }
        for (i = 0; i <= maxi; ++i)
        {
            if (connectfd[i] > 0 && FD_ISSET(connectfd[i], &readSet))
            {
                size_t readnum = read(connectfd[i], receiveline, MAX_LINE);
                if (0 == readnum)
                {
                    close(connectfd[i]);
                    FD_CLR(connectfd[i], &allSet);
                    connectfd[i] = -1;
                    continue;
                }
                write(connectfd[i], receiveline, readnum);
                if (--selectRes <= 0)
                    break;
            }
        }
    }
    return 0;
}

