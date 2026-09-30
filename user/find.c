#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "user/user.h"

static char *target;
static char *command[MAXARG];
static int ncommand;

static int
match(char *path)
{
  char *name = path;
  for (char *p = path; *p; p++)
    if (*p == '/' && p[1])
      name = p + 1;
  if (strcmp(name, target) != 0)
    return 0;
  if (ncommand == 0) {
    printf("%s\n", path);
    return 0;
  }

  command[ncommand] = path;
  command[ncommand + 1] = 0;
  int pid = fork();
  if (pid < 0) {
    fprintf(2, "find: fork failed\n");
    return 1;
  }
  if (pid == 0) {
    exec(command[0], command);
    fprintf(2, "find: exec %s failed\n", command[0]);
    exit(1); // A failed exec must never resume the parent's traversal.
  }
  int status;
  if (wait(&status) < 0) {
    fprintf(2, "find: wait failed\n");
    return 1;
  }
  return status != 0;
}

static int
find(char *path)
{
  struct stat st;
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return 1;
  }
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return 1;
  }
  if (st.type != T_DIR) {
    close(fd);
    return match(path);
  }

  // Snapshot the directory, then close it before recursion and fork.
  // Thus a deep tree does not exhaust the 16 descriptors per process.
  struct dirent *entries = malloc(st.size);
  if (entries == 0) {
    fprintf(2, "find: out of memory\n");
    close(fd);
    return 1;
  }
  int n = read(fd, entries, st.size);
  close(fd);
  if (n < 0 || (uint)n != st.size || n % sizeof(*entries) != 0) {
    fprintf(2, "find: cannot read directory %s\n", path);
    free(entries);
    return 1;
  }

  int failed = match(path);
  int length = strlen(path);
  int slash = length > 0 && path[length - 1] != '/';
  for (int i = 0; i < n / sizeof(*entries); i++) {
    if (entries[i].inum == 0)
      continue;
    char name[DIRSIZ + 1];
    memmove(name, entries[i].name, DIRSIZ);
    name[DIRSIZ] = '\0';
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
      continue;
    int size = length + slash + strlen(name) + 1;
    if (size > MAXPATH) {
      fprintf(2, "find: path too long: %s/%s\n", path, name);
      failed = 1;
      continue;
    }
    // Heap storage keeps recursive stack frames small.
    char *child = malloc(size);
    if (child == 0) {
      fprintf(2, "find: out of memory\n");
      failed = 1;
      break;
    }
    strcpy(child, path);
    if (slash)
      child[length] = '/';
    strcpy(child + length + slash, name);
    failed |= find(child);
    free(child);
  }
  free(entries);
  return failed;
}

int
main(int argc, char *argv[])
{
  if (argc < 3 || (argc > 3 && (argc < 5 || strcmp(argv[3], "-exec") != 0))) {
    fprintf(2, "Usage: find path name [-exec cmd [args ...]]\n");
    exit(1);
  }
  if (argc > 3) {
    ncommand = argc - 4;
    if (ncommand + 2 > MAXARG) {
      fprintf(2, "find: too many command arguments\n");
      exit(1);
    }
    for (int i = 0; i < ncommand; i++)
      command[i] = argv[i + 4];
  }
  target = argv[2];
  if (strlen(argv[1]) >= MAXPATH) {
    fprintf(2, "find: path too long\n");
    exit(1);
  }
  // Normalize a trailing slash, retaining the root directory itself.
  char path[MAXPATH];
  strcpy(path, argv[1]);
  int length = strlen(path);
  while (length > 1 && path[length - 1] == '/')
    path[--length] = '\0';
  exit(find(path));
}
