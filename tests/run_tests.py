# Copyright (c) 2026 Shubhan Punde · MIT licence
"""Checks every lab program against known answers.

* Textbook examples (Silberschatz, Operating System Concepts) for page replacement,
  disk scheduling and the banker's algorithm.
* CPU schedulers: three hand-made cases (including an idle CPU and out-of-order input)
  plus 200 random workloads, each compared with an independent Python reference.

Run with `make test` (builds everything first).
"""
import random
import re
import subprocess
import sys
from collections import deque
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
BIN = ROOT / "bin"
failures = []


def run(prog, stdin):
    return subprocess.run([str(BIN / prog)], input=stdin, capture_output=True, text=True, timeout=5).stdout


def check(name, ok, detail=""):
    print(f"{'PASS' if ok else 'FAIL'}  {name}{'  ' + detail if detail and not ok else ''}")
    if not ok:
        failures.append(name)


# ---------------------------------------------------------------- page replacement
REF = "3\n20\n7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1\n"
for prog, faults in [("fifo", 15), ("lru", 12), ("optimal", 9)]:
    out = run(f"page-replacement/{prog}", REF)
    check(f"page replacement {prog}: {faults} faults", f"Page Faults: {faults}," in out, out[-80:])

# ---------------------------------------------------------------- disk scheduling
REQ = "8\n98 183 37 122 14 124 65 67\n53\n"
check("disk FCFS: 640 cylinders", "Total seek time: 640" in run("disk-scheduling/fcfs", REQ))
check("disk SSTF: 236 cylinders", "seek operations: 236" in run("disk-scheduling/sstf", REQ))
check("disk C-SCAN: 382 cylinders", "Total head movement: 382" in run("disk-scheduling/c_scan", REQ + "200\n"))
check("disk C-SCAN: requests all below head are served",
      "10 -> 20 -> 30" in run("disk-scheduling/c_scan", "3\n10 20 30\n100\n200\n"))

# ---------------------------------------------------------------- banker's algorithm
SAFE = "5\n3\n7 5 3\n3 2 2\n9 0 2\n2 2 2\n4 3 3\n0 1 0\n2 0 0\n3 0 2\n2 1 1\n0 0 2\n3 3 2\n"
check("banker's: textbook safe sequence", "P1 -> P3 -> P4 -> P0 -> P2" in run("deadlock/bankers", SAFE))
check("banker's: unsafe state detected", "NOT in a safe state" in run("deadlock/bankers", "2\n1\n5\n5\n2\n2\n1\n"))

# ---------------------------------------------------------------- shell scripts
prime = lambda n: subprocess.run(["bash", str(ROOT / "shell-scripting/prime.sh")], input=f"{n}\n",
                                 capture_output=True, text=True).stdout
check("prime.sh: 1 is not prime", "1 is not a prime" in prime(1))
check("prime.sh: 13 is prime", "13 is a prime" in prime(13))
check("prime.sh: 9 is not prime", "9 is not a prime" in prime(9))


# ---------------------------------------------------------------- CPU scheduling references
def fcfs(P):
    t, ct = 0, {}
    for i in sorted(range(len(P)), key=lambda i: (P[i][0], i)):
        t = max(t, P[i][0]) + P[i][1]
        ct[i] = t
    return ct


def nonpreemptive(P, key):
    t, done, ct = 0, set(), {}
    while len(done) < len(P):
        ready = [i for i in range(len(P)) if i not in done and P[i][0] <= t]
        if not ready:
            t = min(P[i][0] for i in range(len(P)) if i not in done)
            continue
        i = min(ready, key=lambda i: (key(P[i]), P[i][0], i))
        t += P[i][1]
        ct[i] = t
        done.add(i)
    return ct


def preemptive(P, key):
    rem, t, ct = [b for _, b, _ in P], 0, {}
    while len(ct) < len(P):
        ready = [i for i in range(len(P)) if rem[i] > 0 and P[i][0] <= t]
        if not ready:
            t += 1
            continue
        i = min(ready, key=lambda i: (key(P[i], rem[i]), P[i][0], i))
        rem[i] -= 1
        t += 1
        if rem[i] == 0:
            ct[i] = t
    return ct


def round_robin(P, q):
    order = sorted(range(len(P)), key=lambda i: (P[i][0], i))
    rem, t, Q, k, ct = [b for _, b, _ in P], 0, deque(), 0, {}

    def admit(upto):
        nonlocal k
        while k < len(order) and P[order[k]][0] <= upto:
            Q.append(order[k])
            k += 1

    admit(t)
    while len(ct) < len(P):
        if not Q:
            t = P[order[k]][0]
            admit(t)
            continue
        i = Q.popleft()
        run_for = min(q, rem[i])
        t += run_for
        rem[i] -= run_for
        admit(t)
        if rem[i]:
            Q.append(i)
        else:
            ct[i] = t
    return ct


def averages(P, ct):
    tat = [ct[i] - P[i][0] for i in range(len(P))]
    wt = [tat[i] - P[i][1] for i in range(len(P))]
    return round(sum(tat) / len(P), 2), round(sum(wt) / len(P), 2)


def program_input(prog, P, q):
    n = len(P)
    if prog in ("fcfs", "sjf"):
        return f"{n}\n" + "".join(f"{a}\n{b}\n" for a, b, _ in P)
    if prog.startswith("priority"):
        return f"{n}\n" + "".join(f"{a}\n{b}\n{p}\n" for a, b, p in P)
    if prog == "srtf":
        return f"{n}\n" + "".join(f"{a}\n" for a, _, _ in P) + "".join(f"{b}\n" for _, b, _ in P)
    return f"{n}\n{q}\n" + "".join(f"{a}\n{b}\n" for a, b, _ in P)


def program_averages(out):
    tat = re.search(r"(?i)turn\s*around\s*time\s*[=:]\s*(-?[\d.]+)", out)
    wt = re.search(r"(?i)waiting\s*time\s*[=:]\s*(-?[\d.]+)", out)
    return (round(float(tat.group(1)), 2) if tat else None, round(float(wt.group(1)), 2) if wt else None)


SCHEDULERS = {
    "fcfs": fcfs,
    "sjf": lambda P: nonpreemptive(P, lambda x: x[1]),
    "srtf": lambda P: preemptive(P, lambda x, r: r),
    "priority_nonpreemptive": lambda P: nonpreemptive(P, lambda x: -x[2]),   # higher number first
    "priority_preemptive": lambda P: preemptive(P, lambda x, r: x[2]),      # lower number first
    "round_robin": lambda P: round_robin(P, 2),
}
CASES = {
    "textbook": [(0, 8, 3), (1, 4, 1), (2, 9, 4), (3, 5, 2)],
    "idle CPU": [(0, 3, 2), (6, 2, 1), (7, 4, 3)],
    "unsorted input": [(2, 6, 2), (5, 2, 1), (1, 8, 3), (0, 3, 4), (4, 4, 5)],
    "10 processes": [(i, (i * 7) % 9 + 1, (i * 3) % 5 + 1) for i in range(10)],
}
for prog, ref in SCHEDULERS.items():
    for name, P in CASES.items():
        exp = averages(P, ref(P))
        got = program_averages(run(f"cpu-scheduling/{prog}", program_input(prog, P, 2)))
        check(f"{prog}: {name}", got == exp, f"expected (TAT, WT) {exp}, got {got}")
    rng = random.Random(42)
    mismatches = []
    for _ in range(200):
        P = [(rng.randint(0, 15), rng.randint(1, 9), rng.randint(1, 5)) for _ in range(rng.randint(1, 10))]
        exp = averages(P, ref(P))
        got = program_averages(run(f"cpu-scheduling/{prog}", program_input(prog, P, 2)))
        if got != exp:
            mismatches.append((P, exp, got))
    check(f"{prog}: 200 random workloads", not mismatches, str(mismatches[:1]))

print(f"\n{len(failures)} failed" if failures else "\nall checks passed")
sys.exit(1 if failures else 0)
