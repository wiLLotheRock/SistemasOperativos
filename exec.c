#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"

int
exec(char *path, char **argv)
{
  char *s, *last;
  int i, off;
  uint argc, sz, sp, ustack[3+MAXARG+1];
  struct elfhdr elf;
  struct inode *ip;
  struct proghdr ph;
  pde_t *pgdir, *oldpgdir;
  struct proc *curproc = myproc();

  begin_op();

  if((ip = namei(path)) == 0){
    end_op();
    // DEBUG: Mensaje de diagnóstico para identificar archivos no encontrados durante exec()
    cprintf("exec: fail to find file %s\n", path);
    return -1;
  }
  ilock(ip);
  pgdir = 0;

  // Check ELF header
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) != sizeof(elf)){
    // DEBUG: Error al leer el encabezado ELF del ejecutable
    cprintf("exec: fail to read ELF header\n");
    goto bad;
  }
  if(elf.magic != ELF_MAGIC){
    // DEBUG: El número mágico ELF es inválido (archivo no es un ejecutable ELF válido)
    cprintf("exec: bad ELF magic %x\n", elf.magic);
    goto bad;
  }

  if((pgdir = setupkvm()) == 0){
    // DEBUG: Falló la configuración de la tabla de páginas del kernel
    cprintf("exec: setupkvm failed\n");
    goto bad;
  }

  // Load program into memory.
  sz = 0;
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph)){
      // DEBUG: Error al leer el encabezado de programa (program header) del ELF
      cprintf("exec: readi phdr failed\n");
      goto bad;
    }
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz){
      // DEBUG: Tamaño en memoria menor que tamaño en archivo (ELF malformado)
      cprintf("exec: ph.memsz < ph.filesz\n");
      goto bad;
    }
    if(ph.vaddr + ph.memsz < ph.vaddr){
      // DEBUG: Desbordamiento aritmético en la dirección virtual
      cprintf("exec: ph.vaddr overflow\n");
      goto bad;
    }
    if((sz = allocuvm(pgdir, sz, ph.vaddr + ph.memsz)) == 0){
      // DEBUG: Falló la asignación de memoria virtual para el proceso
      cprintf("exec: allocuvm failed\n");
      goto bad;
    }
    if(ph.vaddr % PGSIZE != 0){
      // DEBUG: CRÍTICO - Dirección virtual no alineada a página (4KB)
      // Este error ocurre cuando .note.gnu.property causa direcciones no alineadas
      cprintf("exec: ph.vaddr %x not aligned to PGSIZE %x\n", ph.vaddr, PGSIZE);
      goto bad;
    }
    if(loaduvm(pgdir, (char*)ph.vaddr, ip, ph.off, ph.filesz) < 0){
      // DEBUG: Falló la carga del segmento del programa en memoria
      cprintf("exec: loaduvm failed\n");
      goto bad;
    }
  }
  iunlockput(ip);
  end_op();
  ip = 0;

  // Allocate two pages at the next page boundary.
  // Make the first inaccessible.  Use the second as the user stack.
  sz = PGROUNDUP(sz);
  if((sz = allocuvm(pgdir, sz, sz + 2*PGSIZE)) == 0)
    goto bad;
  clearpteu(pgdir, (char*)(sz - 2*PGSIZE));
  sp = sz;

  // Push argument strings, prepare rest of stack in ustack.
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    sp = (sp - (strlen(argv[argc]) + 1)) & ~3;
    if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[3+argc] = sp;
  }
  ustack[3+argc] = 0;

  ustack[0] = 0xffffffff;  // fake return PC
  ustack[1] = argc;
  ustack[2] = sp - (argc+1)*4;  // argv pointer

  sp -= (3+argc+1) * 4;
  if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0)
    goto bad;

  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(curproc->name, last, sizeof(curproc->name));

  // Commit to the user image.
  oldpgdir = curproc->pgdir;
  curproc->pgdir = pgdir;
  curproc->sz = sz;
  curproc->tf->eip = elf.entry;  // main
  curproc->tf->esp = sp;
  switchuvm(curproc);
  freevm(oldpgdir);
  return 0;

 bad:
  // DEBUG: Marca general de error - exec() falló en algún punto de las validaciones anteriores
  cprintf("exec: failed at bad label\n");
  if(pgdir)
    freevm(pgdir);
  if(ip){
    iunlockput(ip);
    end_op();
  }
  return -1;
}
