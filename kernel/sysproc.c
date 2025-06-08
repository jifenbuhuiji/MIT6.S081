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

extern  pte_t *
walk(pagetable_t pagetable, uint64 va, int alloc);

int
sys_pgaccess(void)
{
  struct proc* p = myproc();
  uint64 base;
  int len;
  uint64 abits;
  argaddr(0, &base);
  argint(1, &len);
  argaddr(2, &abits);
  int size = (len % 8 == 0) ? len / 8 : len / 8 + 1;
  uint8 buff[1024] = {0};
  // for(int i = 0; i < size; i++)
  //   buff[i] = 0;
  for(int i = 0; i < len; i++)
  {
    uint64 addr = base + i * PGSIZE;
    pte_t *pte = walk(p->pagetable, addr, 0);
    if(pte && (*pte & PTE_A))
    {
      buff[i / 8] |= 1 << (i % 8);
      *pte &= ~PTE_A;
    }
  }
  if(copyout(p->pagetable, abits, (char*)buff, size) != 0)
  {
    printf("copy panic\n");
  }
  return 0;
}


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


