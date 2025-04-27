#include <stdio.h>
#include <string.h>
#include "process.h"

// Function to get the size of a process in bytes based on the user's input
void get_size(int *size_in_bytes, const char *prompt) {
    float size;
    char unit[3]; 

    printf("%s", prompt);

    if (scanf("%f %2s", &size, unit) != 2) {
        printf("Invalid input.\n");
        *size_in_bytes = -1;
        return;
    }

    if (strcmp(unit, "MB") == 0) {
        *size_in_bytes = (int)(size * 1024 * 1024);
    } else if (strcmp(unit, "kB") == 0) {
        *size_in_bytes = (int)(size * 1024);
    } else if (strcmp(unit, "B") == 0) {
        *size_in_bytes = (int)size;
    } else {
        printf("Invalid unit. Please enter MB, kB, or B.\n");
        *size_in_bytes = -1;
    }
}

// Initialize the queue
void init_queue(ProcessQueue *queue) {
    queue->front = -1;
    queue->rear = -1;
}

// Check if the queue is empty
int is_empty(ProcessQueue *queue) {
    return queue->front == -1;
}

// Check if the queue is full
int is_full(ProcessQueue *queue) {
    return queue->rear == MAX_PROCESSES - 1;
}

// Add a process to the queue
void enqueue(ProcessQueue *queue, Process p) {
    if (is_full(queue)) {
        printf("Queue is full. Cannot add more processes.\n");
        return;
    }
    if (is_empty(queue)) {
        queue->front = 0;  // If the queue was empty, set front to 0
    }
    queue->rear++;
    queue->items[queue->rear] = p;
}

// Remove and return a process from the queue
Process dequeue(ProcessQueue *queue) {
    if (is_empty(queue)) {
        printf("Queue is empty. Cannot dequeue.\n");
        Process empty_process = {0};  // Return a default empty process
        return empty_process;
    }

    Process p = queue->items[queue->front];
    
    if (queue->front == queue->rear) {  // If only one element was in the queue
        queue->front = queue->rear = -1;  // Reset the queue to empty
    } else {
        queue->front++;
    }

    return p;
}

// View the process at the front without removing it
Process peek_front(ProcessQueue *queue) {
    if (is_empty(queue)) {
        printf("Queue is empty. No front element.\n");
        Process empty_process = {0};  // Return a default empty process
        return empty_process;
    }

    return queue->items[queue->front];
}