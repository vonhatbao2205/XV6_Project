#!/usr/bin/env python3
"""Follow secret's page through the kalloc free list in a KTRACE log.

Usage (inside nix-shell, from the instrumented xv6 tree):
    python3 analyze_trace.py <trace-log> <kernel/kernel>

Log lines written by the instrumented kernel:
    KT A|F <pa> <caller> <pid> <name> [SECRET]   kalloc() / kfree()
    KT U <oldsz> <newsz> <pid> <name>             uvmalloc() call
    KT M <va> <pa>                                uvmalloc() mapped a page
"""
import subprocess
import sys

PGSIZE = 4096
log, kernel = sys.argv[1], sys.argv[2]

events = []
for lineno, line in enumerate(open(log, errors="replace"), 1):
    f = line.split()
    if not f or f[0] != "KT":
        continue
    # The first process has an empty name until it execs /init.
    name = f[5] if len(f) > 5 and f[5] != "SECRET" else "(none)"
    if f[1] in ("A", "F"):
        events.append(dict(op=f[1], pa=int(f[2], 16), ra=f[3], pid=int(f[4]),
                           name=name, secret="SECRET" in f[5:], line=lineno))
    elif f[1] == "U":
        events.append(dict(op="U", old=int(f[2], 16), new=int(f[3], 16),
                           pid=int(f[4]), name=name))
    elif f[1] == "M":
        events.append(dict(op="M", va=int(f[2], 16), pa=int(f[3], 16),
                           line=lineno))

# Resolve each caller address to the kernel function that called kalloc/kfree.
ras = sorted({e["ra"] for e in events if e["op"] in "AF"})
out = subprocess.run(["riscv64-none-elf-addr2line", "-f", "-s", "-e", kernel] + ras,
                     capture_output=True, text=True, check=True).stdout.split("\n")
func = {ra: out[2 * i] for i, ra in enumerate(ras)}

secret_pa = next(e["pa"] for e in events if e.get("secret"))
print("secret page: pa %#x" % secret_pa)

# Check the LIFO property on every page freed inside the trace, and count how
# many free pages lie above secret's page after every event.
stack, depth, lifo_ok = [], None, True
for e in events:
    if e["op"] == "F":
        stack.append(e["pa"])
        if e["pa"] == secret_pa:
            depth = 0
        elif depth is not None:
            depth += 1
    elif e["op"] == "A":
        if stack:
            lifo_ok &= stack.pop() == e["pa"]
        if e["pa"] == secret_pa:
            # Before secret ran, the page came from the boot-time free list.
            assert depth in (0, None), "secret page allocated while not on top"
            depth = None
        elif depth is not None:
            depth -= 1
    e["depth"] = depth
print("every allocation took the most recently freed page (LIFO):", lifo_ok)

# Group consecutive alloc/free events of the same op, process and caller.
print("\nstep  op  pid name      caller              pages  pages above secret")
rows = []
for e in events:
    if e["op"] not in "AF":
        continue
    key = (e["op"], e["pid"], e["name"], func[e["ra"]])
    if rows and rows[-1][0] == key:
        rows[-1][1] += 1
        rows[-1][2] = e["depth"]
        rows[-1][3] |= e["pa"] == secret_pa
    else:
        rows.append([key, 1, e["depth"], e["pa"] == secret_pa])
started = False
for i, (key, n, d, has_secret) in enumerate(rows):
    started |= has_secret
    if not started:
        continue
    op, pid, name, fn = key
    mark = "  <- secret page" if has_secret else ""
    print("%4d  %s  %3d %-9s %-18s %5d  %s%s" %
          (i, op, pid, name, fn, n, "-" if d is None else d, mark))

# Every event that touches secret's page, with its line in the log.
print("\nline  op  page        caller        pid name     pages above secret")
for e in events:
    if e["op"] in "AF" and e["pa"] == secret_pa:
        d = e["depth"]
        print("%4d  %s   %#x  %-12s %3d %-8s %s" % (
            e["line"], e["op"], e["pa"], func[e["ra"]], e["pid"], e["name"],
            "-" if d is None else d))

# Index of secret's page inside each sbrk(32 * PGSIZE) region of attack.
print()
cur = None
for e in events:
    if e["op"] == "U":
        cur = e if (e["name"] == "attack" and
                    e["new"] - e["old"] == 32 * PGSIZE) else None
    elif e["op"] == "M" and cur and e["pa"] == secret_pa:
        print("line %d: attack pid %d: sbrk region starts at va %#x; secret "
              "page mapped at va %#x = index %d" % (
                  e["line"], cur["pid"], cur["old"], e["va"],
                  (e["va"] - cur["old"]) // PGSIZE))
