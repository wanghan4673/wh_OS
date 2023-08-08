#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const int MAX = 35;  //筛选从2到35的质数
int prime_pipe[2];  //管道
int fd_r,fd_w;  //读取端口和写入端口的文件描述符
int prime,num;  //质数和数字
int times=1;  //用到了递归 故需要特判第一次

int main(int argc, char *argv[])
{
    if(times==1){
        //第一次执行 将2到35均写入到管道中
        times++;
        pipe(prime_pipe);
        fd_r=prime_pipe[0];
        fd_w=prime_pipe[1];
        for (num = 2; num <= MAX; num++)
        {
            write(fd_w, (void *)&num, sizeof(num));  // 将数字写入管道
        }
        close(fd_w);
    }
    if(fork()==0){
        //子进程
        if (read(fd_r, (void *)&prime, sizeof(prime)))
        {
            printf("prime %d\n", prime);  // 输出质数
            pipe(prime_pipe);  // 创建管道
            fd_w = prime_pipe[1];  // 写入端口
        }
        while (read(fd_r, (void *)&num, sizeof(num)))
        {
            if (num % prime != 0)
            {
                write(fd_w, (void *)&num, sizeof(num));  // 将不会被当前质数整除的数写入管道
            }
        }
        fd_r=prime_pipe[0];
        close(fd_w);
        main(argc,argv);
    }
    else{
        //父进程
        wait((int *)0);  //等待子进程结束
        close(fd_r);
    }
    exit(0);
}