#include "types.h"
#include "stat.h"
#include "user.h"
#include "syscall.h"

// Nombres de las syscalls (debe coincidir con syscall.c)
char *syscall_names[] = {
  [SYS_fork]    "fork",
  [SYS_exit]    "exit",
  [SYS_wait]    "wait",
  [SYS_pipe]    "pipe",
  [SYS_read]    "read",
  [SYS_kill]    "kill",
  [SYS_exec]    "exec",
  [SYS_fstat]   "fstat",
  [SYS_chdir]   "chdir",
  [SYS_dup]     "dup",
  [SYS_getpid]  "getpid",
  [SYS_sbrk]    "sbrk",
  [SYS_sleep]   "sleep",
  [SYS_uptime]  "uptime",
  [SYS_open]    "open",
  [SYS_write]   "write",
  [SYS_mknod]   "mknod",
  [SYS_unlink]  "unlink",
  [SYS_link]    "link",
  [SYS_mkdir]   "mkdir",
  [SYS_close]   "close",
  [SYS_trace]   "trace",
  [SYS_sysinfo] "sysinfo",
  [SYS_getprocs] "getprocs",
  [SYS_syscount] "syscount",
};

#define MAX_SYSCALLS 25

int
main(int argc, char *argv[])
{
  uint counts[MAX_SYSCALLS];
  uint single_count;
  int syscall_num;
  int i;
  int total_calls = 0;
  
  // Si recibe un parámetro, mostrar solo ese syscall
  if(argc == 2) {
    syscall_num = atoi(argv[1]);
    
    if(syscall_num < 1 || syscall_num >= MAX_SYSCALLS) {
      printf(2, "syscount: número de syscall inválido (debe estar entre 1 y %d)\n", 
             MAX_SYSCALLS - 1);
      exit();
    }
    
    if(syscount(syscall_num, &single_count) < 0) {
      printf(2, "syscount: error al obtener contador\n");
      exit();
    }
    
    printf(1, "Syscall #%d (%s): %d invocaciones\n", 
           syscall_num, 
           syscall_names[syscall_num] ? syscall_names[syscall_num] : "unknown",
           single_count);
  }
  // Sin parámetros, mostrar resumen completo
  else if(argc == 1) {
    if(syscount(-1, counts) < 0) {
      printf(2, "syscount: error al obtener contadores\n");
      exit();
    }
    
    printf(1, "=== CONTADORES DE SYSCALLS ===\n\n");
    printf(1, "NUM  NOMBRE            INVOCACIONES\n");
    printf(1, "---  ----------------  ------------\n");
    
    for(i = 1; i < MAX_SYSCALLS; i++) {
      if(syscall_names[i] && counts[i] > 0) {
        printf(1, "%-3d  %-16s  %d\n", i, syscall_names[i], counts[i]);
        total_calls += counts[i];
      }
    }
    
    printf(1, "\n");
    printf(1, "TOTAL DE INVOCACIONES: %d\n", total_calls);
  }
  // Uso incorrecto
  else {
    printf(2, "Uso:\n");
    printf(2, "  syscount           - Muestra resumen de todas las syscalls\n");
    printf(2, "  syscount <num>     - Muestra contador de una syscall específica\n");
    exit();
  }
  
  exit();
}
