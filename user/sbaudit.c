// Additional regression tests; build with make SANDBOX_AUDIT=1 qemu.
#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/fcntl.h"
#include "kernel/syscall.h"
#include "kernel/riscv.h"
#include "user/user.h"

#define OPEN (1U << SYS_open)
#define EXEC (1U << SYS_exec)
#define PID  (1U << SYS_getpid)

static void
check(int ok, char *why)
{
  if (!ok) {
    printf("sbaudit: FAIL %s\n", why);
    exit(1);
  }
}

static int
canopen(char *path)
{
  int fd = open(path, O_RDONLY);
  if (fd >= 0)
    close(fd);
  return fd >= 0;
}

static void
run(int test)
{
  char path[MAXPATH + 1];
  char *bad = (char *)~(uint64)0;

  switch (test) {
  case 0:
    check(interpose(OPEN, bad) == -1, "invalid interpose pointer");
    check(canopen("README"), "invalid call must not install mask");
    memset(path, 'x', sizeof(path));
    check(interpose(OPEN, path) == -1, "unterminated pathname");
    check(canopen("README"), "long call must not install mask");
    path[MAXPATH - 1] = 0;
    check(interpose(0, path) == 0, "127-byte pathname");
    path[MAXPATH - 1] = 'x';
    path[MAXPATH] = 0;
    check(interpose(0, path) == -1, "128-byte pathname");
    break;
  case 1:
    check(interpose(OPEN, "README") == 0, "install pathname");
    check(canopen("README"), "exact pathname");
    check(!canopen("./README"), "no pathname normalization");
    check(open(bad, O_RDONLY) == -1, "invalid masked open pointer");
    check(interpose(PID, bad) == -1, "invalid update");
    check(getpid() > 0 && canopen("README"), "update is atomic");
    check(interpose(0, "exec.sh") == -1, "cannot replace exception");
    check(canopen("README") && !canopen("exec.sh"), "old policy intact");
    check(interpose(PID, "README") == 0, "accumulate mask");
    check(getpid() == -1 && canopen("README"), "combined policy");
    check(interpose(0, "-") == 0, "remove exception");
    check(!canopen("README"), "narrowed policy");
    check(interpose(0, "README") == -1, "cannot restore exception");
    break;
  case 2: {
    char *mem = sbrk(2 * PGSIZE);
    check(mem != SBRK_ERROR, "allocate cross-page buffer");
    char *cross = (char *)PGROUNDUP((uint64)mem + 1) - 3;
    strcpy(cross, "README");
    check(interpose(OPEN, cross) == 0, "cross-page interpose");
    cross[0] = 'x';
    check(canopen("README"), "kernel owns pathname copy");
    strcpy(cross, "README");
    check(canopen(cross), "cross-page open");
    break;
  }
  case 3: {
    char *args[] = {"sbaudit", "afterexec", 0};
    check(interpose(OPEN | EXEC, "sbaudit") == 0, "exec sandbox");
    check(exec(bad, args) == -1, "invalid masked exec pointer");
    check(exec("echo", args) == -1, "deny other executable");
    exec("sbaudit", args);
    check(0, "allowed exec failed");
    break;
  }
  case 4:
    check(interpose(OPEN, "-") == 0, "deny create");
    check(open("sb-created", O_CREATE | O_RDWR) == -1, "create rejected");
    break;
  case 5:
    check(interpose((int)(OPEN | (1U << 31)), "-") == 0, "high mask bit");
    check(!canopen("README") && getpid() > 0, "unsigned mask");
    break;
  }
  exit(0);
}

int
main(int argc, char **argv)
{
  if (argc == 2 && strcmp(argv[1], "afterexec") == 0) {
    check(!canopen("README") && canopen("sbaudit"), "policy survives exec");
    exit(0);
  }
  unlink("sb-created");
  for (int i = 0; i < 6; i++) {
    int pid = fork(), status = -1;
    check(pid >= 0, "fork");
    if (pid == 0)
      run(i);
    check(wait(&status) == pid && status == 0, "child regression");
    check(canopen("README") && getpid() > 0, "parent unaffected");
    printf("sbaudit: case %d OK\n", i);
  }
  check(!canopen("sb-created"), "denied open had no create side effect");
  // Reuse process slots more than NPROC times, checking every new child.
  for (int i = 0; i < NPROC + 8; i++) {
    int pid = fork(), status = -1;
    check(pid >= 0, "reuse fork");
    if (pid == 0) {
      check(canopen("README") && getpid() > 0, "no stale child sandbox");
      check(interpose(OPEN | PID, "-") == 0, "reuse sandbox");
      exit(0);
    }
    check(wait(&status) == pid && status == 0, "reuse child");
  }
  printf("sbaudit: ALL PASSED\n");
  exit(0);
}
