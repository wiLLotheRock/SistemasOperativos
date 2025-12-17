#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"
#include "sysinfo.h"

// Nombres de los estados de procesos
char *states[] = {
  [0] "unused",
  [1] "embryo",
  [2] "sleeping",
  [3] "runnable",
  [4] "running",
  [5] "zombie"
};

int
main(int argc, char *argv[])
{
  struct procinfo procs[NPROC];
  int num_procs;
  int i;
  
  num_procs = getprocs(procs, NPROC);
  
  if(num_procs < 0){
    printf(2, "ps: error obteniendo informacion de procesos\n");
    exit();
  }
  
  printf(1, "PID\tSTATE\t\tSIZE\tNAME\n");
  printf(1, "---\t-----\t\t----\t----\n");
  
  for(i = 0; i < num_procs; i++){
    char *state = "unknown";
    if(procs[i].state >= 0 && procs[i].state < 6)
      state = states[procs[i].state];
      
    printf(1, "%d\t%s\t\t%d\t%s\n", 
           procs[i].pid, 
           state, 
           procs[i].sz, 
           procs[i].name);
  }
  
  printf(1, "\nTotal de procesos: %d\n", num_procs);
  
  exit();
}
