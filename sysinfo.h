// Estructura para estadísticas del sistema
struct sysinfo {
  int num_processes;     // Número de procesos activos (no UNUSED)
  int num_runnable;      // Número de procesos RUNNABLE
  int num_sleeping;      // Número de procesos SLEEPING
  int num_running;       // Número de procesos RUNNING
  int num_zombie;        // Número de procesos ZOMBIE
  int context_switches;  // Contador de cambios de contexto
  int uptime_ticks;      // Tiempo de ejecución del sistema en ticks
};

// Estructura para información de un proceso individual
struct procinfo {
  int pid;               // Process ID
  int state;             // Estado del proceso (enum procstate)
  uint sz;               // Tamaño de la memoria del proceso
  char name[16];         // Nombre del proceso
};
