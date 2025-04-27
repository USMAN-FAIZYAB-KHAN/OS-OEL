#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESSES 100

// Structure for process details
typedef struct 
{
    int process_id;
    int arrival_time;
    int execution_time;
    int size;
} Process;

// Structure for a queue to hold processes
typedef struct {
    Process items[MAX_PROCESSES];
    int front;
    int rear;
} ProcessQueue;

// Function prototype for getting the size of a process
void get_size(int *size_in_bytes, const char *prompt);

// Queue methods prototypes
void init_queue(ProcessQueue *queue);  // Initialize the queue
int is_empty(ProcessQueue *queue);     // Check if the queue is empty
int is_full(ProcessQueue *queue);      // Check if the queue is full
void enqueue(ProcessQueue *queue, Process p);  // Add a process to the queue
Process dequeue(ProcessQueue *queue);   // Remove and return a process from the queue
Process peek_front(ProcessQueue *queue);  // View the process at the front without removing

#endif // PROCESS_H
