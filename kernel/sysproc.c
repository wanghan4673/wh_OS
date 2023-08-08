#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
extern pte_t *walk(pagetable_t, uint64, int);

// 系统调用函数，用于检查访问页表项并清除访问位
int sys_pgaccess(void)
{
  // lab pgtbl: your code here.

  uint64 srcva, st;
  int len;
  uint64 buf = 0;
  struct proc *p = myproc();
  acquire(&p->lock);
  // 获取参数
  argaddr(0, &srcva);
  argint(1, &len);
  argaddr(2, &st);
  // 检查长度是否合法
  if ((len > 64) || (len < 1))
    return -1;
  pte_t *pte;
  for (int i = 0; i < len; i++)
  {
    // 遍历页表，查找虚拟地址对应的页表项
    pte = walk(p->pagetable, srcva + i * PGSIZE, 0);
    // 如果页表项为空，则返回错误
    if(pte == 0){
      return -1;
    }
    // 如果页表项的有效位为0，则返回错误
    if((*pte & PTE_V) == 0){
      return -1;
    }
    // 如果页表项的用户位为0，则返回错误
    if((*pte & PTE_U) == 0){
      return -1;
    }
    // 如果页表项的访问位为1，则清除访问位，并设置相应的buf位为1
    if(*pte & PTE_A){
      *pte = *pte & ~PTE_A;
      buf |= (1 << i);  
    }
  }
  release(&p->lock);
  // 将buf复制到用户空间
  copyout(p->pagetable, st, (char *)&buf, ((len -1) / 8) + 1);
  return 0;
}
#endif


uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
