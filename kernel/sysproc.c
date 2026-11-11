#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#ifdef PGTBL_SOL
#include "riscv.h"
#endif
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > UTOP)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
uint64
sys_vmprint(void)
{
  struct proc *p;

  p = myproc();
  vmprint(p->pagetable);
  return 0;
}
#endif

#ifdef LAB_PGTBL
// Upper limit on the pages one pgaccess() call may scan. The bitmask
// for that many pages (512 bytes) lives on the 4 KB kernel stack.
#define PGACCESS_MAX 4096

// pgaccess(base, npages, mask): report which of the npages user pages
// starting at the page that contains base have been accessed since
// the previous check. Bit i of the mask (byte i/8, bit i%8) is set if
// page i was accessed; the first page is the least significant bit.
// Returns 0, or -1 if an argument is invalid or a page is not mapped.
uint64
sys_pgaccess(void)
{
  uint64 base, umask;
  int npages;
  uchar mask[PGACCESS_MAX / 8];
  struct proc *p = myproc();

  argaddr(0, &base);
  argint(1, &npages);
  argaddr(2, &umask);

  if (npages <= 0 || npages > PGACCESS_MAX)
    return -1;
  base = PGROUNDDOWN(base);
  // walk() panics on va >= MAXVA, so the whole range must lie below it.
  if (base >= MAXVA || npages > (MAXVA - base) / PGSIZE)
    return -1;

  int nbytes = (npages + 7) / 8; // copy out exactly this many bytes
  memset(mask, 0, nbytes);

  // Pass 1: every page must be a valid user page; collect the A bits.
  // Nothing is changed yet, so a failed call leaves the bits intact.
  for (int i = 0; i < npages; i++) {
    pte_t *pte = walk(p->pagetable, base + (uint64)i * PGSIZE, 0);
    if (pte == 0 || (*pte & PTE_V) == 0 || (*pte & PTE_U) == 0)
      return -1;
    if (*pte & PTE_A)
      mask[i / 8] |= 1 << (i % 8);
  }

  // Pass 2: copy the mask to user space; fails for a bad buffer.
  if (copyout(p->pagetable, p->sz, umask, (char *)mask, nbytes) < 0)
    return -1;

  // Pass 3: clear the reported A bits so the next call only sees new
  // accesses. No sfence.vma is needed here: the trampoline flushes the
  // TLB when it switches back to this page table (userret), so the
  // next user access walks the page table again and re-sets PTE_A.
  for (int i = 0; i < npages; i++) {
    if (mask[i / 8] & (1 << (i % 8))) {
      pte_t *pte = walk(p->pagetable, base + (uint64)i * PGSIZE, 0);
      *pte &= ~PTE_A;
    }
  }
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

#ifdef LAB_LOCK
uint64
sys_cpupin(void)
{
  struct proc *p = myproc();
  int cpu;

  argint(0, &cpu);
  if (cpu < 0 || cpu >= NCPU)
    return -1;
  acquire(&p->lock);
  p->pincpu = &cpus[cpu];
  release(&p->lock);
  return 0;
}
#endif
