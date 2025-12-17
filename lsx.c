#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

char*
fmttype(short type)
{
  switch(type){
    case T_DIR:  return "DIR ";
    case T_FILE: return "FILE";
    case T_DEV:  return "DEV ";
    default:     return "??? ";
  }
}

void
lsx(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "lsx: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "lsx: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    printf(1, "%s  inode:%d  nlink:%d  size:%d  %s\n", 
           fmttype(st.type), st.ino, st.nlink, st.size, fmtname(path));
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "lsx: path too long\n");
      break;
    }
    
    printf(1, "TYPE INODE LINKS SIZE      NAME\n");
    printf(1, "---- ----- ----- --------- ----\n");
    
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "lsx: cannot stat %s\n", buf);
        continue;
      }
      printf(1, "%s  %-5d %-5d %-9d %s\n", 
             fmttype(st.type), st.ino, st.nlink, st.size, fmtname(buf));
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    lsx(".");
    exit();
  }
  for(i=1; i<argc; i++)
    lsx(argv[i]);
  exit();
}
