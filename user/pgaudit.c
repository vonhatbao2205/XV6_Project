// Extra checks for the pgaccess() system call (CS333 Lab 3, Task 3).
// pgtbltest covers the basic cases; this program checks the edges:
// argument limits, bit order, the exact number of bytes written,
// reads as well as writes, clearing PTE_A, keeping PTE_A when the call
// fails, and pages that are not user pages.

#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/memlayout.h"
#include "user/user.h"

#define LIMIT 4096 // PGACCESS_MAX in kernel/sysproc.c

static int nchecks, nfailed;
static uchar mask[LIMIT / 8 + 8];

static void
check(int ok, char *what)
{
  nchecks++;
  if (!ok)
    nfailed++;
  printf("%s %s\n", ok ? "ok  " : "FAIL", what);
}

// Report the A bit of one page (1 or 0), or -1 if pgaccess fails.
static int
accessed(char *va)
{
  uchar m = 0;
  if (pgaccess(va, 1, &m) < 0)
    return -1;
  return m & 1;
}

int
main(void)
{
  // 16 user pages, page aligned, mapped now (sbrk is eager).
  char *raw = sbrk(17 * PGSIZE);
  char *buf = (char *)PGROUNDUP((uint64)raw);
  volatile char sink;

  check(pgaccess(buf, 0, mask) < 0, "npages = 0 is rejected");
  check(pgaccess(buf, -1, mask) < 0, "npages < 0 is rejected");

  // Bit order and size: pages 0 and 8 written, page 3 only read.
  pgaccess(buf, 16, mask); // clear the bits left over from sbrk
  memset(mask, 0xAA, 4);
  buf[0] = 1;
  sink = buf[3 * PGSIZE];
  buf[8 * PGSIZE] = 1;
  check(pgaccess(buf, 9, mask) == 0, "pgaccess on 9 mapped pages");
  check(mask[0] == 0x09, "byte 0 = 0x09: pages 0 and 3, LSB first");
  check(mask[1] == 0x01, "byte 1 = 0x01: page 8");
  check(mask[2] == 0xAA, "only (9+7)/8 = 2 bytes are written");
  check(pgaccess(buf, 9, mask) == 0 && mask[0] == 0 && mask[1] == 0,
        "the A bits were cleared by the previous call");

  // An address inside the first page selects that page.
  buf[2 * PGSIZE] = 1;
  check(pgaccess(buf + 100, 3, mask) == 0 && mask[0] == 0x04,
        "unaligned base is rounded down to its page");

  // A failed call must not lose the A bits.
  buf[5 * PGSIZE] = 1;
  check(pgaccess(buf, 9, (void *)0) < 0, "mask in read-only text is rejected");
  check(pgaccess(buf, 9, (void *)KERNBASE) < 0,
        "mask at a kernel address is rejected");
  char *top = (char *)PGROUNDUP((uint64)sbrk(0)); // first unmapped page
  char *last = top - PGSIZE;                      // last heap page
  check(pgaccess(buf, 9, top) < 0, "mask above p->sz is rejected");
  last[0] = 1;
  check(pgaccess(last, 2, mask) < 0,
        "range running past the heap end is rejected");
  check(accessed(buf + 5 * PGSIZE) == 1 && accessed(last) == 1,
        "A bits survive the failed calls");

  // Pages that are not valid user pages.
  check(pgaccess(top, 1, mask) < 0, "unmapped page above p->sz");
  char *lazy = (char *)PGROUNDUP((uint64)sbrklazy(2 * PGSIZE));
  check(pgaccess(lazy, 1, mask) < 0, "lazy sbrk page not touched yet");
  lazy[0] = 1;
  check(accessed(lazy) == 1, "lazy page after the first touch");
  char *guard = (char *)PGROUNDDOWN((uint64)&raw) - PGSIZE;
  check(pgaccess(guard, 1, mask) < 0, "stack guard page (no PTE_U)");
  check(pgaccess((void *)TRAPFRAME, 1, mask) < 0, "trapframe (no PTE_U)");
  check(pgaccess((void *)TRAMPOLINE, 1, mask) < 0, "trampoline (no PTE_U)");
  check(pgaccess((void *)TRAMPOLINE, 2, mask) < 0,
        "range past MAXVA (no kernel panic)");
  check(pgaccess((void *)-1L, 1, mask) < 0, "va = 2^64 - 1 (no kernel panic)");

  // The page limit, from both sides.
  char *big = (char *)PGROUNDUP((uint64)sbrk((LIMIT + 1) * PGSIZE));
  if (big == (char *)-1) {
    check(0, "sbrk for the limit test");
  } else {
    check(pgaccess(big, LIMIT, mask) == 0, "npages = 4096 (the limit) works");
    check(pgaccess(big, LIMIT + 1, mask) < 0, "npages = 4097 is rejected");
  }

  (void)sink;
  if (nfailed == 0)
    printf("pgaudit: all %d checks passed\n", nchecks);
  else
    printf("pgaudit: %d of %d checks FAILED\n", nfailed, nchecks);
  exit(nfailed != 0);
}
