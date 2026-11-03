# Operating Systems Lab Programs (C and Bash)

[![tests](https://github.com/Drakeion2532/OS-Labs/actions/workflows/tests.yml/badge.svg)](https://github.com/Drakeion2532/OS-Labs/actions/workflows/tests.yml)

The classic operating-systems algorithms from my B.Tech OS lab (VIT Pune), written in C, plus a few Bash exercises. Every program is checked automatically against textbook answers (Silberschatz, *Operating System Concepts*), and the CPU schedulers are also compared with an independent Python reference on 200 random workloads each.

## What's inside

| Folder | Program | Algorithm |
|---|---|---|
| `cpu-scheduling/` | `fcfs.c` | First-Come, First-Served |
| | `sjf.c` | Shortest Job First (non-preemptive) |
| | `srtf.c` | Shortest Remaining Time First (preemptive SJF) |
| | `priority_nonpreemptive.c` | Priority, non-preemptive (**higher number = higher priority**) |
| | `priority_preemptive.c` | Priority, preemptive (**lower number = higher priority**) |
| | `round_robin.c` | Round Robin with arrival times and a time quantum |
| `disk-scheduling/` | `fcfs.c`, `sstf.c`, `c_scan.c` | FCFS, Shortest Seek Time First, Circular SCAN |
| `page-replacement/` | `fifo.c`, `lru.c`, `optimal.c` | FIFO, Least Recently Used, Optimal (Belady) |
| `deadlock/` | `bankers.c` | Banker's algorithm (safety check and safe sequence) |
| `shell-scripting/` | `prime.sh`, `reverse.sh`, `bubble_sort.sh`, `linux_commands.txt` | Bash basics and a Linux command worksheet |

Each scheduler prints the completion, turnaround and waiting time for every process, then the averages.

## Where these ideas run in real systems

These are textbook versions, but each one is the starting point for something a real operating system or service does every second:

| Lab | In practice |
|---|---|
| CPU scheduling | Linux replaced its CFS scheduler with **EEVDF** in kernel 6.6. Both descend from fair-share ideas: round robin's time slices, plus a weighting that favours tasks that have had less CPU, much like priority aging. SRTF-style "shortest first" thinking appears in job schedulers and in request queues of web servers. |
| Disk scheduling | Linux's block layer offers **mq-deadline**, **BFQ** and **Kyber** schedulers, or **none** for fast NVMe SSDs where seek time no longer matters. mq-deadline keeps requests sorted by position (the SCAN family) but adds deadlines so no request starves, the same starvation problem SSTF has. |
| Page replacement | Linux approximates **LRU** with active/inactive page lists, and since kernel 6.1 can use **Multi-Gen LRU**. Databases and CDNs use LRU-like caches (often with frequency counts) for the same reason the LRU lab beats FIFO. **Optimal** can't be implemented (it needs the future) but is the yardstick the others are measured against. |
| Banker's algorithm | General-purpose OSes don't use it, because processes rarely declare their maximum needs in advance. The idea of granting a request only if the system stays in a safe state survives in admission control for clusters, databases and real-time systems. |
| Shell scripting | The same loops and tests automate log rotation, backups and deployment scripts on every Linux server. |

## Build, run and test

```bash
make                                   # builds everything into bin/
./bin/cpu-scheduling/round_robin       # each program prompts for its input
make test                              # 42 checks, see below
```

Needs `gcc` (or any C11 compiler), `make`, `python3` for the tests, and `bash` for the scripts.

Example: the textbook disk queue 98, 183, 37, 122, 14, 124, 65, 67 with the head at 53 on a 200-cylinder disk:

| Algorithm | Head movement |
|---|---|
| FCFS | 640 cylinders |
| SSTF | 236 cylinders |
| C-SCAN | 382 cylinders (including the jump back to 0) |

## How the programs are checked

`tests/run_tests.py` runs every compiled program and compares its output with known answers:

* **Page replacement:** the 20-reference textbook string with 3 frames gives 15 faults (FIFO), 12 (LRU) and 9 (Optimal).
* **Disk scheduling:** the queue above, plus a C-SCAN case where every request is below the head.
* **Banker's algorithm:** the textbook 5-process, 3-resource example gives the safe sequence P1 → P3 → P4 → P0 → P2, and an unsafe state is reported as unsafe.
* **CPU scheduling:** four fixed cases (the textbook example, an idle CPU, processes typed out of arrival order, and 10 processes) and 200 random workloads per algorithm, each compared with a Python reference implementation.
* **Shell:** `prime.sh` on 1, 9 and 13.

## Bugs found and fixed when these checks were added

Writing the tests turned up real bugs in the original lab code. They are fixed here, and each fix is commented in the source:

| Program | Bug | Effect |
|---|---|---|
| `disk-scheduling/sstf.c` | Served requests were marked `-1`, but the search for the closest request didn't skip them | Only 4 of 8 requests served; reported 82 instead of 236 |
| `disk-scheduling/c_scan.c` | Turned around at the last request instead of the disk end, and skipped the wrap-around when all requests were below the head | Reported 350 instead of 382; some requests never served |
| `deadlock/bankers.c` | Compared `need[i][j]` with `avail[i]`, and could pick a finished process again | Crashed (array overflow) on the textbook example |
| `cpu-scheduling/fcfs.c` | Ignored idle time before a late arrival | Negative waiting times |
| `cpu-scheduling/sjf.c` | After an idle gap, ran the next arrival without comparing it with others arriving at the same time | Wrong order, higher averages |
| `cpu-scheduling/srtf.c`, `priority_nonpreemptive.c` | Used slot `[9]` of a 10-element array as a sentinel | Wrong results with 10 processes, and with an idle CPU |
| `cpu-scheduling/priority_preemptive.c` | Fell back to process 0 when nothing was ready, even if it had finished | Wrong times whenever the CPU was idle |
| `cpu-scheduling/round_robin.c` | Assumed process 1 arrived first and input was sorted; linear queue could overflow | Wrong results for out-of-order input; possible crash on long runs |
| `shell-scripting/prime.sh` | No check for numbers below 2 | Reported 1 as prime |

Two earlier drafts that duplicated `sjf.c` and `round_robin.c` with incorrect logic were removed.

## Limits

These are teaching programs: they read interactively from standard input, support up to 10 processes (50 for Round Robin and 10×10 matrices for the banker's algorithm), and use simple loops rather than optimised data structures.

## Licence and credit

MIT © 2026 [Shubhan Punde](https://github.com/Drakeion2532). You're welcome to learn from and reuse this code; the licence requires keeping the copyright notice, and presenting the project as your own work is not permitted. To cite it, use [CITATION.cff](CITATION.cff).

<sub>Project ID: sp2532-c062ca2a33dc</sub>
