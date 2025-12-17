#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "sysinfo.h"

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

// Activa o desactiva el trazado de syscalls para el proceso actual
int
sys_trace(void)
{
  int enable;

  if(argint(0, &enable) < 0)
    return -1;
  
  myproc()->trace_syscalls = enable;
  return 0;
}

// Obtiene estadísticas del sistema
int
sys_sysinfo(void)
{
  struct sysinfo *info;
  
  if(argptr(0, (char**)&info, sizeof(*info)) < 0)
    return -1;
  
  return getsysinfo(info);
}

// Obtiene información de procesos
int
sys_getprocs(void)
{
  struct procinfo *procs;
  int max;
  
  if(argptr(0, (char**)&procs, sizeof(*procs)) < 0)
    return -1;
  if(argint(1, &max) < 0)
    return -1;
    
  return getprocs(procs, max);
}

// Obtiene el contador de invocaciones de una syscall
// Si syscall_num == -1, copia todos los contadores
int
sys_syscount(void)
{
  int syscall_num;
  uint *count_ptr;
  
  if(argint(0, &syscall_num) < 0)
    return -1;
  if(argptr(1, (char**)&count_ptr, sizeof(*count_ptr)) < 0)
    return -1;
  
  return getsysccount(syscall_num, count_ptr);
}
