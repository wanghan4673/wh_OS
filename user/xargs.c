#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

char args[MAXARG][512];
char *pass_args[MAXARG];
int preargnum, argnum;
char ch;
char arg_buf[512];
int n;

int readline()
{
    argnum = preargnum;  //将已有的参数个数保存到argnum中
    memset(arg_buf, 0, sizeof(arg_buf));  //清空参数缓冲区arg_buf
    for (;;)
    {
        n = read(0, &ch, sizeof(ch));  //从标准输入读取一个字符
        if (n == 0)
        {
            return 0;
            break;
        }
        else if (n < 0)
        {
            fprintf(2, "read error\n");
            exit(1);
        }
        else
        {
            if (ch == '\n')  //如果读取到换行符，则表示一行参数读取完成
            {
                memcpy(args[argnum], arg_buf, strlen(arg_buf) + 1);  // 参数拷贝到args数组中
                argnum++;  // 参数个数增加
                return 1;  // 返回1表示读取到了一行数据
            }
            else if (ch == ' ')  //如果读取到空格符，则表示一个参数读取完成
            {
                memcpy(args[argnum], arg_buf, strlen(arg_buf) + 1);  //将参数拷贝到args数组中
                argnum++;  //参数个数增加
                memset(arg_buf, 0, sizeof(arg_buf));  //清空参数缓冲区arg_buf
            }
            else
            {
                arg_buf[strlen(arg_buf)] = ch;  //将字符添加到参数缓冲区arg_buf中
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("usage: xargs [command] [arg1] [arg2] ... [argn]\n");
        exit(0);
    }
    preargnum = argc - 1;
    for (int i = 0; i < preargnum; i++)
        memcpy(args[i], argv[i + 1], strlen(argv[i + 1]));
    while (readline())
    {
        if (fork() == 0)
        {
            *args[argnum] = 0;
            int i = 0;
            while (*args[i])
            {
                pass_args[i] = (char *)&args[i];
                i++;
            }
            *pass_args[argnum] = 0;
            exec(pass_args[0], pass_args);
            printf("exec error\n");
            exit(0);
        }
        else
        {
            wait((int *)0);
        }
    }
    exit(0);
}