// Copyright (c) 2026 Shubhan Punde · MIT licence
#include<stdio.h>
#define MAX 9999
struct proc
{
    int no,at,bt,rt,ct,wt,tat,pri,temp;
};
struct proc read(int i)
{
    struct proc p;
    printf("\nProcess No: %d\n",i);
    p.no=i;
    printf("Enter Arrival Time: ");
    scanf("%d",&p.at);
    printf("Enter Burst Time: ");
    scanf("%d",&p.bt);
    p.rt=p.bt;
    printf("Enter Priority: ");
    scanf("%d",&p.pri);
    p.temp=p.pri;
    return p;
}
int main()
{
    int i,n,c,remaining,min_val,min_index;
    struct proc p[10],temp;
    float avgtat=0,avgwt=0;
    printf("Smallest Priority First Scheduling Algorithm (Preemptive)\n");
    printf("Enter Number of Processes: ");
    scanf("%d",&n);
    if(n<1||n>10)
    {
        printf("This program supports 1 to 10 processes.\n");
        return 0;
    }
    for(int i=0;i<n;i++)
        p[i]=read(i+1);
    remaining=n;
	for(int i=0;i<n-1;i++)
        for(int j=0;j<n-i-1;j++)    
            if(p[j].at>p[j+1].at)
            {
            temp=p[j];
            p[j]=p[j+1];
            p[j+1]=temp;
            }
    /* Advance one time unit at a time, always running the arrived, unfinished process
       with the smallest priority number. If nothing has arrived, the CPU idles. (The old
       loop fell back to process 0 even after it had finished, so idle gaps gave wrong times.) */
    c=p[0].at;
    while(remaining>0)
    {
        min_val=MAX,min_index=-1;
        for(int j=0;j<n;j++)
            if(p[j].at<=c&&p[j].temp<min_val)
                min_val=p[j].temp,min_index=j;
        if(min_index==-1)
        {
            c++;
            continue;
        }
        i=min_index;
        p[i].ct=c=c+1;
        p[i].rt--;
        if(p[i].rt==0)
        {
            p[i].temp=MAX;
            remaining--;
        }
    }
		printf("\nProcessNo\tAT\tBT\tPri\tCT\tTAT\tWT\n");
    for(int i=0;i<n;i++)
    {
        p[i].tat=p[i].ct-p[i].at;
        avgtat+=p[i].tat;
        p[i].wt=p[i].tat-p[i].bt;
        avgwt+=p[i].wt;
        printf("P%d\t\t%d\t%d\t%d\t%d\t%d\t%d\n",p[i].no,p[i].at,p[i].bt,p[i].pri,p[i].ct,p[i].tat,p[i].wt);
    }
    avgtat/=n,avgwt/=n;
    printf("\nAverage TurnAroundTime=%f\nAverage WaitingTime=%f",avgtat,avgwt);
}
