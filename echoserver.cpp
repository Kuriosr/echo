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

void str_echo(int sockfd)
{
    int n;
    char buf[MAX_LINE];
    while((n = read(sockfd, buf, MAX_LINE)) > 0)
    {
        write(sockfd, buf, n);
    }
    if (n < 0)
    {
        printf("strecho : read error");
    }
}

void sig_child(int signo)
{
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    return;
}

int main()
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in stServerAddr;
    memset(&stServerAddr, 0, sizeof(stServerAddr));
    stServerAddr.sin_family = AF_INET;
    stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    stServerAddr.sin_port = htons(ECHO_SERVER_PORT);
    if (0 != bind(listenfd, (struct sockaddr *)&stServerAddr, sizeof(stServerAddr)))
    {
        printf("Bind Socket Error");
        return -1;
    }
    listen(listenfd, 5);
    signal(SIGCHLD, sig_child);
    int connfd;
    struct sockaddr_in stClientAddr;
    socklen_t clilen;
    while(true)
    {
        clilen = sizeof(stClientAddr);
        if ((connfd = accept(listenfd, (struct sockaddr *)&stClientAddr, &clilen)) < 0)
        {
            if (errno == EINTR)
                continue;
            else
                printf("accept error\n");
        }
        if (fork() == 0)
        {
            close(listenfd);
            str_echo(connfd);
            return 0;
        }
        close(connfd);
    }
    return 0;
}

