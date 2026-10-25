#!/usr/bin/env python3
"""Boot the instrumented kernel and run: secret xyzzy; attack; attack.

Usage (inside nix-shell, from the instrumented xv6 tree):
    python3 run_trace.py <output-log>
"""
import sys

import pexpect

out = open(sys.argv[1], "w")
q = pexpect.spawn("make qemu", encoding="utf-8", timeout=120)
q.logfile_read = out
q.expect("init: starting sh")
for cmd in ["secret xyzzy", "attack", "attack"]:
    q.expect(r"\$ ")
    q.sendline(cmd)
q.expect(r"\$ ")
q.sendcontrol("a")
q.send("x")
q.expect(pexpect.EOF)
out.close()
