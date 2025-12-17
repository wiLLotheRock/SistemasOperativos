#include "types.h"
#include "stat.h"
#include "user.h"
#include "sysinfo.h"

int
main(int argc, char *argv[])
{
  struct sysinfo info;
  
  if(sysinfo(&info) < 0){
    printf(2, "uptime: error obteniendo informacion del sistema\n");
    exit();
  }
  
  // Convertir ticks a segundos (asumiendo 100 ticks por segundo)
  int seconds = info.uptime_ticks / 100;
  int minutes = seconds / 60;
  int hours = minutes / 60;
  
  printf(1, "=== Estado del Sistema XV6 ===\n");
  printf(1, "Tiempo de ejecucion:\n");
  printf(1, "  %d ticks\n", info.uptime_ticks);
  printf(1, "  %d segundos (%d:%02d:%02d)\n", 
         seconds, hours, minutes % 60, seconds % 60);
  
  printf(1, "\nProcesos:\n");
  printf(1, "  Total activos: %d\n", info.num_processes);
  printf(1, "  Running:       %d\n", info.num_running);
  printf(1, "  Runnable:      %d\n", info.num_runnable);
  printf(1, "  Sleeping:      %d\n", info.num_sleeping);
  printf(1, "  Zombie:        %d\n", info.num_zombie);
  
  printf(1, "\nPlanificacion:\n");
  printf(1, "  Cambios de contexto: %d\n", info.context_switches);
  
  exit();
}
