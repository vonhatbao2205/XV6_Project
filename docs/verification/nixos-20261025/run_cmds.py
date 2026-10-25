#!/usr/bin/env python3
"""Boot xv6 once and run the given shell commands in order.

Usage (inside nix-shell, from an xv6 tree): python3 run_cmds.py <log> <cmd>...
"""
import sys

import pexpect

out = open(sys.argv[1], "w")
q = pexpect.spawn("make qemu", encoding="utf-8", timeout=120)
q.logfile_read = out
q.expect("init: starting sh")
for cmd in sys.argv[2:]:
    q.expect(r"\$ ")
    q.sendline(cmd)
q.expect(r"\$ ")
q.sendcontrol("a")
q.send("x")
q.expect(pexpect.EOF)
out.close()
