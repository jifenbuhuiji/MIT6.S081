#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

void exec_child(char *program, char **argv)
{
    if(fork() == 0)
    {
        if(exec(program, argv) == -1)
        {
            printf("exce failed\n");
        }
    }
    return;
}

int main(int argc, char *argv[])
{
    char buff[1024];
    char *buff_last = buff, *buff_cur = buff;
    char *arg[128];
    char **args = arg;
    if(argc < 2)
    {
        exit(1);
    }
    for(int i = 1; i < argc; i++)
    {
        *args = argv[i];
        args++;
    }
    char **arg_cur = args;
    while(read(0, buff_last, 1) != 0)
    {
        if(*buff_last == '\n' || *buff_last == ' ')
        {
            int a = *buff_last == '\n' ? 1 : 0;
            *buff_last = '\0';
            *(args ++) = buff_cur;
            buff_cur = (buff_last + 1);
            if(a)
            {
                args = 0;
                exec_child(argv[1], arg);
                args = arg_cur;
            }
        }
        buff_last++;
    }
    if(buff_last != buff_cur || args != arg_cur)
    {
        *buff_last = '\0';
        *(args ++) = buff_cur;
        args = 0;
        exec_child(argv[1], arg);
    }
    wait(0);
    exit(0);
}
    