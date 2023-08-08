#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main()
{
  int parentToChild[2];
  int childToParent[2];
  // 创建管道
  if (pipe(parentToChild) < 0 || pipe(childToParent) < 0) {
    printf("Failed to create pipes\n");
    exit(0);
  }
  int pid = fork();
  if (pid < 0) {
    // 失败
    printf("Fork failed\n");
    exit(0);
  }
  else if (pid == 0) {
    // 子进程
    close(parentToChild[1]);  // 关闭子进程中不需要的管道端口
    close(childToParent[0]);
    char byte;
    if (read(parentToChild[0], &byte, 1) != 1) {
      printf("Failed to read byte from parent\n");
      exit(0);
    }
    printf("%d: received ping\n", getpid());
    if (write(childToParent[1], &byte, 1) != 1) {
      printf("Failed to write byte to parent\n");
      exit(0);
    }
    close(parentToChild[0]);
    close(childToParent[1]);
    exit(0);
  } 
  else {
    // 父进程
    close(parentToChild[0]);
    close(childToParent[1]);
    char byte = 'A';
    if (write(parentToChild[1], &byte, 1) != 1) {
      printf("Failed to write byte to child\n");
      exit(0);
    }
    if (read(childToParent[0], &byte, 1) != 1) {
      printf("Failed to read byte from child\n");
      exit(0);
    }
    printf("%d: received pong\n", getpid());
    close(parentToChild[1]);
    close(childToParent[0]);
    exit(0);
  }
}