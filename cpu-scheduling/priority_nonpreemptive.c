// Copyright (c) 2026 Shubhan Punde · MIT licence
 #include<stdio.h>

struct proc {
    int no, at, bt, ct, wt, tat, pri, status;
};

struct proc read(int i) {
    struct proc p;
    printf("\nProcess No: %d\n", i);
    p.no = i;
    printf("Enter Arrival Time: ");
    scanf("%d", &p.at);
    printf("Enter Burst Time: ");
    scanf("%d", &p.bt);
    printf("Enter Priority: ");
    scanf("%d", &p.pri);
    p.status = 0;
    return p;
}

int main() {
    int n, l, ct = 0, remaining;
    struct proc p[10], temp;
    float avgtat = 0, avgwt = 0;

    printf("<--Highest Priority First Scheduling Algorithm(Non-Preemptive)-->\n");
    printf("Enter Number of Processes: ");
    scanf("%d", &n);
    if(n<1||n>10)
    {
        printf("This program supports 1 to 10 processes.\n");
        return 0;
    }

    for (int i = 0; i < n; i++)
        p[i] = read(i + 1);

    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (p[j].at > p[j + 1].at) {
                temp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = temp;
            }

    remaining = n;

    printf("\nProcessNo\tAT\tBT\tPri\tCT\tTAT\tWT\tRT\n");

    for (ct = p[0].at; remaining != 0;) {
        /* Pick the arrived, unfinished process with the highest priority. The old code
           used p[9] as a sentinel, which broke for 10 processes and "ran" the sentinel
           whenever the CPU was idle. */
        l = -1;
        for (int i = 0; i < n; i++)
            if (p[i].at <= ct && p[i].status != 1 && (l == -1 || p[i].pri > p[l].pri))
                l = i;
        if (l == -1) {          /* nothing has arrived yet: jump to the next arrival */
            for (int i = 0; i < n; i++)
                if (p[i].status != 1) { ct = p[i].at; break; }
            continue;
        }

        p[l].ct = ct = ct + p[l].bt;
        p[l].tat = p[l].ct - p[l].at;
        avgtat += p[l].tat;
        p[l].wt = p[l].tat - p[l].bt;
        avgwt += p[l].wt;
        p[l].status = 1;
        remaining--;

        printf("P%d\t\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", p[l].no, p[l].at, p[l].bt,
               p[l].pri, p[l].ct, p[l].tat, p[l].wt, p[l].wt);
    }

    avgtat /= n, avgwt /= n;
    printf("\nAverage TurnAroundTime=%f\nAverage WaitingTime=%f", avgtat, avgwt);
}
