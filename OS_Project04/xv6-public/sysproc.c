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
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  //myproc()->sz += n;
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

// get grand parent pid syscall
int 
sys_getgpid(void)
{
 cprintf("My student id is 2020081958\n");
 cprintf("My pid is %d\n", sys_getpid());
 return myproc()->parent->parent->pid;
}

// wrapper function for int countfp(void) in kalloc.c
int
sys_countfp(void)
{
  return countfp();
}

// wrapper function for int countvp(void) in vm.c
int
sys_countvp(void)
{
  return countvp();
}

// wrapper function for int countpp(void) in vm.c
int 
sys_countpp(void)
{
  return countpp();
}

// wrapper function for int countptp(void) in vm.c
int
sys_countptp(void)
{
  return countptp();
}
