#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

char *get_name(char *s) {
  char *p = s;
  while (*p)
    ++p;
  while (p >= s && *p != '/')
    --p;
  return p + 1;
}

void find(char *path, char *name) {
  // printf("find in path %s, name %s\n", path, name);
  int fd;
  if ((fd = open(path, 0)) < 0) {
    printf("find: cannot open %s\n", path);
    return;
  }
  struct stat st;
  if (fstat(fd, &st) < 0) {
    printf("find: cannot stat %s\n", path);
    close(fd);
    return;
  }
  if (strcmp(name, get_name(path)) == 0)
    printf("%s\n", path);
  if(st.type == T_DIR)
  {
    char buf[512], *p;
    struct dirent de;
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
      fprintf(2, "find: path too long\n");
      return;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0)
        continue;
      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      find(buf, name);
    }
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("usage: find path name\n");
    exit(0);
  }
  find(argv[1], argv[2]);
  exit(0);
}
