#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
    int process_id;
    int arrival_time;
    int size;
} Process;

typedef struct 
{
    int status;
    int size;
    int process_id;
    int starting_address;
} MemoryTableEntry;

void get_size(int *size_in_bytes, const char *prompt) {
    float size;
    char unit[3]; 

    // Use the passed prompt message
    printf("%s", prompt);

    // Read input directly with scanf for size and unit
    if (scanf("%f %2s", &size, unit) != 2) {
        printf("Invalid input.\n");
        *size_in_bytes = -1;
        return;
    }

    // Handle the units and convert to bytes
    if (strcmp(unit, "MB") == 0) {
        *size_in_bytes = (int)(size * 1024 * 1024);  // Convert MB to bytes
    } else if (strcmp(unit, "kB") == 0) {
        *size_in_bytes = (int)(size * 1024);  // Convert kB to bytes
    } else if (strcmp(unit, "B") == 0) {
        *size_in_bytes = (int)size;  // No conversion needed for bytes
    } else {
        printf("Invalid unit. Please enter MB, kB, or B.\n");
        *size_in_bytes = -1;
    }
}

void print_table(Process *processes, int count) {
    int total_size = 0;  // Variable to accumulate the total size of all processes

    // Print the table header
    printf("+------------+-------------------+-----------------+\n");
    printf("| Process ID | Arrival Time (ms) | Size (bytes)    |\n");
    printf("+------------+-------------------+-----------------+\n");

    int max_size = processes[0].size;  // Initialize max size with the first process size
    int min_size = processes[0].size;  // Initialize min size with the first process size

    // Print the details for each process
    for (int i = 0; i < count; i++) {
        printf("| %-10d | %-17d | %-15d |\n", processes[i].process_id, processes[i].arrival_time, processes[i].size);
        total_size += processes[i].size;  // Add the size of the current process to the total size
        
        if (processes[i].size > max_size) {
            max_size = processes[i].size;
        }

        if (processes[i].size < min_size) {
            min_size = processes[i].size;
        }

    }

    // Calculate the average size as an integer
    int average_size = (count > 0) ? total_size / count : 0;

    // Print the average row
    printf("+------------+-------------------+-----------------+\n");
    printf("| Average    |                   | %-15d |\n", average_size);
    printf("| Minimum    |                   | %-15d |\n", min_size);
    printf("| Maximum    |                   | %-15d |\n", max_size);

    // Print the table footer
    printf("+------------+-------------------+-----------------+\n");
}

int get_() {

}


int main() {
    int no_of_process, size_of_mm;

    system("clear");

    // Pass custom message for size input
    get_size(&size_of_mm, "Size of main memory (e.g., 10 MB, 13.5 kB, 100 B): ");

    if (size_of_mm == -1) {
        printf("Invalid memory size input. Exiting...\n");
        return 1;
    }


    printf("Enter number of processes: ");
    scanf("%d", &no_of_process);

    Process *processes = (Process *)malloc(sizeof(Process) * no_of_process);
    if (processes == NULL) {
        printf("Memory allocation failed.\n");
        exit(0);
    }

    MemoryTableEntry *memory_table = (MemoryTableEntry *)malloc(sizeof(MemoryTableEntry));
    if (memory_table == NULL) {
        printf("Memory allocation failed.\n");
        exit(0);
    }
    
    int memory_entries = 1;

    // Initially entire memory is free
    memory_table[0].status = 0;
    memory_table[0].process_id = -1;
    memory_table[0].starting_address = 0;
    memory_table[0].size = size_of_mm;

    for (int i = 0; i < no_of_process; i++) {
        printf("\nProcess %d\n", i+1);
        processes[i].process_id = i+1;
        printf("Arrival time (ms): ");
        scanf("%d", &processes[i].arrival_time);
        get_size(&processes[i].size, "Size (e.g., 10 MB, 13.5 kB, 100 B): ");
    }

    printf("\n");
    print_table(processes, no_of_process);

    // Find the maximum arrival time
    int max_arrival_time = processes[0].arrival_time;
    for (int i = 1; i < no_of_process; i++) {
        
        if (processes[i].arrival_time > max_arrival_time) {
            max_arrival_time = processes[i].arrival_time;
        }
    }

    printf("\n");
    for (int time=0; time<=max_arrival_time; time++) {
        printf("Time (%d ms)\n", time);
        for (int i=0; i<no_of_process; i++) {
            if (processes[i].arrival_time == time) {
                printf("PID = %d\n", processes[i].process_id);

            }
        }
    }

    free(processes);
    return 0;
}
