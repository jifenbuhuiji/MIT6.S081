#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int p[2];
    pipe(p);
    if(fork() != 0)
    {
        char buff[100];
        write(p[1], "a", 1);
        int status;
        wait(&status);
        int pid = getpid();
        read(p[0], buff, 1);
        printf("%d: received pong\n", pid);
        close(p[0]);
        close(p[1]);
        exit(0);
    }
    else
    {
        char buff[100];
        int pid = getpid();
        read(p[0], buff, 1);
        write(p[1], buff, 1);
        printf("%d: received ping\n", pid);
        close(p[0]);
        close(p[1]);
        exit(0);
    }
    exit(0);
}
