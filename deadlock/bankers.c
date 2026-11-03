// Copyright (c) 2026 Shubhan Punde · https://github.com/Drakeion2532/OS-Labs · MIT licence · sp2532-c062ca2a33dc
#include<stdio.h>
int i,j;
int np,nr,max[10][10],allocation[10][10],need[10][10],avail[10];
void readmatrix(int matrix[10][10]);
void calculateNeed();
void display(int matrix[10][10]);
void banker();
int main() {
    printf("enter the number of process:");
    scanf("%d",&np);
    printf("\n enter the number of resources:" );
    scanf("%d",&nr);
    if (np < 1 || np > 10 || nr < 1 || nr > 10)
    {
        printf("\nThis program supports 1-10 processes and 1-10 resource types.\n");
        return 1;
    }
    printf("enter the max matrix:");
    readmatrix(max);
    printf("enter the allocation matrix:");
    readmatrix(allocation);
    printf("\n Enter Available Resource:");
    for(i=0;i<nr;i++)
    {
        scanf("%d",&avail[i]);
    }
    calculateNeed();
    printf("\n Max Matrix\n");
    display(max);
    printf("\n Allocation Matrix");
    display(allocation);
    printf("\n Need Matrix");
    display(need);
    printf("\n Available Resources");
    for(i=0;i<nr;i++)
    printf("%d  \t",avail[i]);
    banker();
}
void readmatrix(int matrix[10][10])
{
    int i,j;
    for(i=0;i<np;i++)
    for(j=0;j<nr;j++)
    scanf("%d",&matrix[i][j]);
}
void calculateNeed()
{
    int i,j;
    for(i=0;i<np;i++)
    {
        for(j=0;j<nr;j++)
        {
            need[i][j]=max[i][j]-allocation[i][j];
        }
    }
}
void display(int matrix[10][10])
{
    int i,j;
    for(i=0;i<np;i++)
    {
        for(j=0;j<nr;j++)
        {
            printf("%d  \t",matrix[i][j]);
        }
        printf("\n");
    }
}
void banker()
{
    /* Safety algorithm: repeatedly find an unfinished process whose remaining need fits in
       the currently available (work) vector, let it finish and release its allocation.
       Fixes in this version: compare need[i][j] with work[j] (was avail[i]); never pick a
       finished process twice (the old loop did, and overflowed safe_seq, causing a crash);
       print a readable sequence. */
    int finish[10] = {0}, work[10], safe_seq[10], k = 0, progress = 1;
    for (j = 0; j < nr; j++)
        work[j] = avail[j];
    while (k < np && progress)
    {
        progress = 0;
        for (i = 0; i < np; i++)
        {
            if (finish[i])
                continue;
            int fits = 1;
            for (j = 0; j < nr; j++)
                if (need[i][j] > work[j]) { fits = 0; break; }
            if (fits)
            {
                for (j = 0; j < nr; j++)
                    work[j] += allocation[i][j];
                finish[i] = 1;
                safe_seq[k++] = i;
                progress = 1;
            }
        }
    }
    if (k < np)
    {
        printf("\n\nThe system is NOT in a safe state (deadlock possible).\n");
        return;
    }
    printf("\n\nThe system is in a safe state.\nSafe sequence: ");
    for (i = 0; i < np; i++)
        printf("P%d%s", safe_seq[i], i < np - 1 ? " -> " : "\n");
}
