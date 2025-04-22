#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
    int process_id;
    int arrival_time;
    int size;
} Process;

void get_size(int *size_in_bytes, const char *prompt) {
    float size;
    char unit[3];  // to hold unit (MB, kB, B)

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

    // Print the details for each process
    for (int i = 0; i < count; i++) {
        printf("| %-10d | %-17d | %-15d |\n", processes[i].process_id, processes[i].arrival_time, processes[i].size);
        total_size += processes[i].size;  // Add the size of the current process to the total size
    }

    // Calculate the average size as an integer
    int average_size = (count > 0) ? total_size / count : 0;

    // Print the average row
    printf("+------------+-------------------+-----------------+\n");
    printf("| Average    |                   | %-15d |\n", average_size);

    // Print the table footer
    printf("+------------+-------------------+-----------------+\n");
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

    for (int i = 0; i < no_of_process; i++) {
        printf("\nProcess %d\n", i+1);
        processes[i].process_id = i+1;
        printf("Arrival time (ms): ");
        scanf("%d", &processes[i].arrival_time);
        get_size(&processes[i].size, "Size (e.g., 10 MB, 13.5 kB, 100 B): ");
    }

    printf("\n");
    print_table(processes, no_of_process);

    free(processes);
    return 0;
}
