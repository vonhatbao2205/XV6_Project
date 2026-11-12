// Print how the kernel page table is built (CS333 Lab 3, Task 5).
// ksupernpte() counts the valid leaf PTEs above level 0 (superpages)
// and the valid level-0 PTEs (4 KB pages) of the kernel page table;
// ksuper_test in pgtbltest only checks them against fixed bounds.

#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

int
main(void)
{
  uint64 nsuper = 0, nsmall = 0;

  if (ksupernpte(&nsuper, &nsmall) != 0) {
    printf("kpgstat: ksupernpte failed\n");
    exit(1);
  }
  printf("kernel page table: %ld superpage PTEs (2 MB), %ld level-0 PTEs (4 KB)\n",
         nsuper, nsmall);
  printf("memory mapped: %ld MB by superpages, %ld KB by 4 KB pages\n",
         nsuper * (SUPERPGSIZE >> 20), nsmall * (PGSIZE >> 10));
  // without superpages, each 2 MB leaf would be one level-0
  // page-table page holding 512 PTEs.
  printf("level-0 page-table pages saved: %ld (%ld KB), PTEs saved: %ld\n",
         nsuper, nsuper * (PGSIZE >> 10), nsuper * 512);
  exit(0);
}
