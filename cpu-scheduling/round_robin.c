// Copyright (c) 2026 Shubhan Punde · MIT licence
#include <stdio.h>

/* Round Robin CPU scheduling with arrival times.
   Processes may be entered in any order. Newly arrived processes join the ready queue
   before a preempted process is put back, which is the standard textbook convention.
   (The previous version assumed process 1 arrived first and the input was sorted, and
   its ever-growing linear queue q[100] could overflow on long runs.) */

#define MAXP 50

struct process {
    int arrivalTime;
    int burstTime;
    int completionTime;
    int turnTime;
    int waitingTime;
};

int main(void) {
    int n, tq;
    struct process p[MAXP];
    int remaining[MAXP], order[MAXP];

    printf("Enter the number of processes: ");
    if (scanf("%d", &n) != 1 || n < 1 || n > MAXP) {
        printf("This program supports 1 to %d processes.\n", MAXP);
        return 1;
    }
    printf("Enter time quantum of CPU: ");
    if (scanf("%d", &tq) != 1 || tq < 1) {
        printf("The time quantum must be at least 1.\n");
        return 1;
    }
    for (int i = 0; i < n; i++) {
        printf("Enter arrival time of process %d: ", i + 1);
        scanf("%d", &p[i].arrivalTime);
        printf("Enter burst time of process %d: ", i + 1);
        scanf("%d", &p[i].burstTime);
        remaining[i] = p[i].burstTime;
        order[i] = i;
        printf("\n");
    }

    /* order[] = process indices sorted by arrival time (stable, so ties keep input order) */
    for (int i = 1; i < n; i++) {
        int key = order[i], j = i - 1;
        while (j >= 0 && p[order[j]].arrivalTime > p[key].arrivalTime) {
            order[j + 1] = order[j];
            j--;
        }
        order[j + 1] = key;
    }

    /* circular ready queue: each process is in the queue at most once, so n slots suffice */
    int queue[MAXP], head = 0, count = 0, next = 0, completed = 0, time = 0;
    int totalTurn = 0, totalWaiting = 0;

    while (completed < n) {
        while (next < n && p[order[next]].arrivalTime <= time) {      /* admit arrivals */
            queue[(head + count++) % n] = order[next++];
        }
        if (count == 0) {                                               /* CPU idle */
            time = p[order[next]].arrivalTime;
            continue;
        }
        int i = queue[head];
        head = (head + 1) % n;
        count--;

        int run = remaining[i] < tq ? remaining[i] : tq;
        time += run;
        remaining[i] -= run;

        while (next < n && p[order[next]].arrivalTime <= time) {      /* arrivals during the slice */
            queue[(head + count++) % n] = order[next++];
        }
        if (remaining[i] > 0) {
            queue[(head + count++) % n] = i;                            /* back of the queue */
        } else {
            p[i].completionTime = time;
            p[i].turnTime = time - p[i].arrivalTime;
            p[i].waitingTime = p[i].turnTime - p[i].burstTime;
            totalTurn += p[i].turnTime;
            totalWaiting += p[i].waitingTime;
            completed++;
        }
    }

    printf("\n");
    printf("Average Waiting Time = %.2f\n", (float) totalWaiting / n);
    printf("Average Turnaround Time = %.2f\n", (float) totalTurn / n);
    printf("\n");
    printf("Process\tAT\tBT\tCT\tWT\tTAT\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\n", i + 1, p[i].arrivalTime, p[i].burstTime,
               p[i].completionTime, p[i].waitingTime, p[i].turnTime);
    }
    return 0;
}
