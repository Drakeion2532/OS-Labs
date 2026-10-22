#include <stdio.h>
#include <stdlib.h>

// Function to sort the request array in ascending order
void sortRequests(int requests[], int n) {
    int temp;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (requests[j] > requests[j + 1]) {
                temp = requests[j];
                requests[j] = requests[j + 1];
                requests[j + 1] = temp;
            }
        }
    }
}

int main() {
    int n, head, diskSize, seekTime = 0;
    
    printf("Enter the number of disk I/O requests: ");
    scanf("%d", &n);
    
    int requests[n];
    
    printf("Enter the requests: ");
    for (int i = 0; i < n; i++) {
        scanf("%d", &requests[i]);
    }
    
    printf("Enter the initial head position: ");
    scanf("%d", &head);

    printf("Enter the disk size (number of cylinders, e.g. 200): ");
    scanf("%d", &diskSize);
    
    sortRequests(requests, n);  // Sort the request array in ascending order
    
    // Find the index where the head is located
    int currentIndex = 0;
    while (currentIndex < n && requests[currentIndex] <= head) {
        currentIndex++;
    }
    
    printf("Order of execution: ");
    
    // C-SCAN goes to the end of the disk in one direction, then returns in the other direction
    // First, it goes in the increasing order
    for (int i = currentIndex; i < n; i++) {
        printf("%d -> ", requests[i]);
        seekTime += abs(requests[i] - head);
        head = requests[i];
    }
    
    // C-SCAN: if requests remain below the start position, run to the last cylinder,
    // jump back to cylinder 0 and keep moving up. (The old version turned around at the
    // last request instead of the disk end, and skipped this step entirely when every
    // request was below the head, so those requests were never served.)
    if (currentIndex > 0) {
        seekTime += (diskSize - 1) - head;  // sweep to the end of the disk
        seekTime += diskSize - 1;           // return jump to cylinder 0 (counted as head movement)
        head = 0;
        for (int i = 0; i < currentIndex; i++) {
            printf("%d -> ", requests[i]);
            seekTime += abs(requests[i] - head);
            head = requests[i];
        }
    }
    
    printf("\n");
    
    printf("Total head movement: %d cylinders (includes the return jump to 0)\n", seekTime);
    
    return 0;
}
