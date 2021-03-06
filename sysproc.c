#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr,newsz;
  int n;
  struct proc *p;

  if(argint(0, &n) < 0)
    return -1;
  p = myproc();
  addr = p->sz;
  newsz = addr + n;
  // 不能够超过kernel virtual address
  if(newsz >= KERNBASE)
	  return -1;
  // 只改變adress而不分配Memory
  p->sz = newsz ;
//  if(growproc(n) < 0)
//    return -1;
  cprintf("pid %d sbrk memory but not allocate physical memory\n",p->pid);
  return addr;
}

int
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

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_cps( void)
{
	return cps();
}

int
sys_chpr(void)
{
	int pid,pr;
	if(argint(0,&pid)<0)
		return -1;
	if(argint(1,&pr)<0)
		return -1;

	return chpr(pid,pr);
}
