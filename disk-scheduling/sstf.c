#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int absDiff(int a, int b) {
    return abs(a - b);
}

int findClosestRequest(int request[], int served[], int n, int head) {
    int minDiff = INT_MAX;
    int index = -1;

    for (int i = 0; i < n; i++) {
        if (served[i]) continue;  /* skip requests already handled */
        int diff = absDiff(head, request[i]);
        if (diff < minDiff) {
            minDiff = diff;
            index = i;
        }
    }

    return index;
}

void sstfDiskScheduling(int request[], int n, int head) {
    int totalSeekOperations = 0;
    int served[n];
    for (int i = 0; i < n; i++) served[i] = 0;
    printf("Sequence of disk access:");

    for (int i = 0; i < n; i++) {
        int index = findClosestRequest(request, served, n, head);
        printf(" %d", request[index]);

        // Calculate the seek operations for each request
        totalSeekOperations += absDiff(head, request[index]);

        // Update the head position
        head = request[index];

        // Mark the request as served. (The old code overwrote it with -1, but the
        // search did not skip -1, so it was picked again once the head was near 0.)
        served[index] = 1;
    }

    printf("\nTotal number of seek operations: %d\n", totalSeekOperations);

    // Calculate the average seek length
    double averageSeekLength = (double)totalSeekOperations / n;
    printf("Average seek length: %lf\n", averageSeekLength);
}

int main() {
    int n;

    printf("Enter the number of disk requests: ");
    scanf("%d", &n);

    int request[n];

    printf("Enter the disk request sequence:\n");
    for (int i = 0; i < n; i++) {
        printf("Request %d: ", i + 1);
        scanf("%d", &request[i]);
    }

    int head;

    printf("Enter the initial position of the disk head: ");
    scanf("%d", &head);

    sstfDiskScheduling(request, n, head);

    return 0;
}
