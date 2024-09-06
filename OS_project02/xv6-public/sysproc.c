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

// make function yield to system call
int
sys_yield(void)
{
  yield();
  return 0;
}

// print ticks, pid, name of the current process
int
sys_print_current_process(void)
{
  cprintf("ticks = %d, pid = %d, name = %s\n", sys_uptime(), sys_getpid(), myproc()->name);
  return 0;
}

// wrapper function for int getlev(void) in proc.c
int 
sys_getlev(void)
{
  return getlev();
}

// wrapper function for int setpriority(int pid, int priority) in proc.c
int
sys_setpriority(void) 
{
  int pid, priority;
  argint(0, &pid);
  argint(1, &priority);
  return setpriority(pid, priority);
}

// wrapper function for setmonopoly in proc.c
int
sys_setmonopoly(void) 
{
  int pid, password;
  argint(0, &pid);
  argint(1, &password);
  return setmonopoly(pid, password);
}

// wrapper function for monopolize
int 
sys_monopolize(void) 
{
  monopolize();
  return 0;
}

// wrapper function for unmonopolize
int
sys_unmonopolize(void)
{
  unmonopolize();
  return 0;
}