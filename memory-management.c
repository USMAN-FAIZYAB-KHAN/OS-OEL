#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
    int process_id;
    int arrival_time;
} Process;

void print_table(Process *processes, int count) {
    printf("+------------+-------------------+\n");
    printf("| Process ID | Arrival Time (ms) |\n");
    printf("+------------+-------------------+\n");

    for (int i = 0; i < count; i++) {
        printf("| %-10d | %-17d |\n", processes[i].process_id, processes[i].arrival_time);
    }

    printf("+------------+-------------------+\n");
}

void main() {
    int no_of_process;

    system("clear");
    printf("Enter number of processes: ");
    scanf("%d", &no_of_process);
    
    Process *processes = (Process *)malloc(sizeof(Process) * 5);
    if (processes == NULL) {
        printf("Allocation Failed");
        exit(0);
    }

    for (int i = 0; i < no_of_process; i++) {
        processes[i].process_id = i+1;
        printf("Enter arrival time of process %d (ms): ", i+1);
        scanf("%d", &processes[i].arrival_time);
    }

    system("clear");
    print_table(processes, no_of_process);

    free(processes);
}