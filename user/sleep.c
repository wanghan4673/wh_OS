#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int ticks;
  if (argc <= 1)
  {
    printf("Usage: sleep [seconds]\n");
    exit(0);
  }
  ticks = atoi(argv[1]);
  sleep(ticks);
  exit(0);
}