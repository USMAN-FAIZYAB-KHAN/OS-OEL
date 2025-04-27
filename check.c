#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "display.h"
#include "process.h"
#include "memory.h"

int main() {
    int no_of_process, size_of_mm;

    system("clear");

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
    memory_table[0].ending_address = size_of_mm -1;
    memory_table[0].size = size_of_mm;

    for (int i = 0; i < no_of_process; i++) {
        printf("\nProcess %d\n", i+1);
        processes[i].process_id = i+1;
        printf("Arrival time (ms): ");
        scanf("%d", &processes[i].arrival_time);
        printf("Execution time (ms): ");
        scanf("%d", &processes[i].execution_time);
        get_size(&processes[i].size, "Size (e.g., 10 MB, 13.5 kB, 100 B): ");
    }

    printf("\n");
    print_process_table(processes, no_of_process);

    // Find maximum arrival time
    int max_arrival_time = processes[0].arrival_time;
    for (int i = 1; i < no_of_process; i++) {
        if (processes[i].arrival_time > max_arrival_time) {
            max_arrival_time = processes[i].arrival_time;
        }
    }

    printf("\n");

    for (int time = 0; time <= max_arrival_time; time++) {
        printf("\nTime (%d ms)\n", time);

        for (int i = 0; i < no_of_process; i++) {
            if (processes[i].arrival_time == time) {
                printf("Allocating PID %d...\n", processes[i].process_id);
                allocate_process(&memory_table, &memory_entries, processes[i]);
            }
        }

        print_memory_table(memory_table, memory_entries);
        print_memory(memory_table, memory_entries, size_of_mm);
    }

    free(processes);
    free(memory_table);
    return 0;
}