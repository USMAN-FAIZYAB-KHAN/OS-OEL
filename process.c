#include <stdio.h>
#include <string.h>
#include "process.h"
#include "display.h"

// Get the size of a process in bytes based on user input
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

// Check if all processes are completed
int all_processes_completed(Process *processes, int count) {
    for (int i = 0; i < count; i++) {
        if (processes[i].completed == 0) {  // Process not completed
            return 0;
        }
    }
    return 1;  // All processes are completed
}

// Initialize the process queue
void init_queue(ProcessQueue *queue) {
    queue->front = -1;
    queue->rear = -1;
}

// Check if the process queue is empty
int is_empty(ProcessQueue *queue) {
    return queue->front == -1;
}

// Check if the process queue is full
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
        queue->front = 0;  // Set front to 0 if queue was empty
    }
    queue->rear++;
    queue->items[queue->rear] = p;
}

// Remove and return the process from the front of the queue
Process dequeue(ProcessQueue *queue) {
    if (is_empty(queue)) {
        printf("Queue is empty. Cannot dequeue.\n");
        Process empty_process = {0};  // Return default empty process
        return empty_process;
    }

    Process p = queue->items[queue->front];
    
    if (queue->front == queue->rear) {  // Only one process in the queue
        queue->front = queue->rear = -1;  // Reset queue
    } else {
        queue->front++;
    }

    return p;
}

// View the process at the front of the queue without removing it
Process peek_front(ProcessQueue *queue) {
    if (is_empty(queue)) {
        printf("Queue is empty. No front element.\n");
        Process empty_process = {0};  // Return default empty process
        return empty_process;
    }

    return queue->items[queue->front];
}

// Display the contents of the queue
void display_queue(ProcessQueue *queue) {
    dualprintf("------------------------------------------------------------\n");
    dualprintf("|");

    if (!is_empty(queue)) {
        for (int i = queue->front; i <= queue->rear; i++) {
            dualprintf("  P%-2d  |", queue->items[i].process_id);
        }
    }

    dualprintf("\n");
    dualprintf("------------------------------------------------------------\n");
}

