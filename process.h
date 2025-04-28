#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESSES 100  // Maximum number of processes allowed

// Structure to store process details
typedef struct 
{
    int process_id;       // Unique ID for the process
    int arrival_time;     // Time when the process arrives
    int execution_time;   // Time required to execute the process
    int completed;        // 0 = Not completed, 1 = Completed
    int size;             // Memory size required by the process
} Process;

// Structure for a queue to hold processes
typedef struct {
    Process items[MAX_PROCESSES]; // Array to store processes
    int front;                    // Index of the front item
    int rear;                     // Index of the rear item
} ProcessQueue;

// Function prototypes

// Get size input from user
void get_size(int *size_in_bytes, const char *prompt);

// Check if all processes are completed
int all_processes_completed(Process *processes, int count);

// Initialize the process queue
void init_queue(ProcessQueue *queue);

// Check if the queue is empty
int is_empty(ProcessQueue *queue);

// Check if the queue is full
int is_full(ProcessQueue *queue);

// Add a process to the queue
void enqueue(ProcessQueue *queue, Process p);

// Remove and return the process at the front of the queue
Process dequeue(ProcessQueue *queue);

// View the process at the front without removing it
Process peek_front(ProcessQueue *queue);

// Display the contents of the queue
void display_queue(ProcessQueue *queue);

#endif // PROCESS_H
