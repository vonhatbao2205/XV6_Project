#!/usr/bin/env python3
"""Extra checks, independent of the unchanged course grading script."""
import os
import random
from gradelib import Runner, save, shell_script, test, run_tests

os.environ["SANDBOX_AUDIT"] = "1"
r = Runner(save("xv6.out.audit"))


@test(1, "sandbox additional regression")
def test_regression():
    r.run_qemu(shell_script(["sbaudit"]),
               timeout=60)
    r.match("^sbaudit: ALL PASSED", no=["sbaudit: FAIL"])

@test(1, "lab memory-reuse repeated verification")
def test_memory_reuse():
    rng = random.Random(24120182)
    alphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    for i, length in enumerate([1, 8, 16, 24, 8, 8, 8, 8, 8, 8]):
        secret = "0Az"[i % 3] + "".join(rng.choice(alphabet)
                                       for _ in range(length - 1))
        r.run_qemu(shell_script(["secret " + secret, "attack", "attack"]))
        r.match("^" + secret + "$")
        print("verified trial %d, length %d" % (i + 1, length))


run_tests()
