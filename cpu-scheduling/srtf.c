// Copyright (c) 2026 Shubhan Punde · MIT licence
#include<stdio.h>
int main()
{
int a[10], b[10], x[10];
int waiting[10], turnaround[10], completion[10];
int i, smallest, count = 0, time, n;
double avg = 0, tt = 0, end;
printf("\nEnter the number of Processes: "); // Input
scanf("%d", &n);
if (n < 1 || n > 10)
{
printf("This program supports 1 to 10 processes.\n");
return 0;
}
for (i = 0; i < n; i++)
{
printf("\nEnter arrival time of process %d : ", i + 1); // Input
scanf("%d", &a[i]);
}
for (i = 0; i < n; i++)
{
printf("\nEnter burst time of process %d : ", i + 1); // Input
scanf("%d", &b[i]);
}
for (i = 0; i < n; i++)
x[i] = b[i];
for (time = 0; count != n; time++)
{
/* pick the arrived process with the shortest remaining time (no sentinel slot, so all
   10 slots are usable; the old b[9] sentinel corrupted the 10th process) */
smallest = -1;
for (i = 0; i < n; i++)
{
if (a[i] <= time && b[i] > 0 && (smallest == -1 || b[i] < b[smallest]))
smallest = i;
}
if (smallest == -1)
continue; /* CPU idle this time unit */
b[smallest]--;

if (b[smallest] == 0)
{
count++;
end = time + 1;
completion[smallest] = end;
waiting[smallest] = end - a[smallest] - x[smallest];
turnaround[smallest] = end - a[smallest];
}
}
printf("Process\tburst-time\tarrival-time\twaiting-time\tturnaround-time\tcompletion-time\n");
for (i = 0; i < n; i++)
{
printf("p%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n",i+1,x[i],a[i],waiting[i],turnaround[i],completion[i]);
avg = avg + waiting[i];
tt = tt + turnaround[i];
}
printf("\nAverage waiting time = %.2lf\n", avg / n);
printf("Average Turnaround time = %.2lf\n", tt / n);
printf("\n");
}
