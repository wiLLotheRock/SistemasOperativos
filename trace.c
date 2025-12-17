#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"

int
main(int argc, char *argv[])
{
  int i;
  char *nargv[MAXARG];

  if(argc < 2){
    printf(2, "Uso: trace <comando> [argumentos...]\n");
    printf(2, "  Ejecuta el comando con trazado de syscalls activado\n");
    exit();
  }

  // Activar el trazado de syscalls
  if(trace(1) < 0){
    printf(2, "trace: error al activar el trazado\n");
    exit();
  }

  // Preparar argumentos para exec
  for(i = 1; i < argc && i < MAXARG; i++){
    nargv[i-1] = argv[i];
  }
  nargv[i-1] = 0;

  // Ejecutar el comando con trazado activo
  exec(nargv[0], nargv);
  
  // Si exec falla
  printf(2, "trace: exec %s failed\n", nargv[0]);
  exit();
}
