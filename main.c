#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "display.h"
#include "process.h"
#include "memory.h"

int main()
{
    int no_of_process, size_of_mm;
    ProcessQueue ready_queue, waiting_queue;
    init_queue(&ready_queue);
    init_queue(&waiting_queue);

    system("clear");

    get_size(&size_of_mm, "Size of main memory (e.g., 10 MB, 13.5 kB, 100 B): ");

    if (size_of_mm == -1)
    {
        printf("Invalid memory size input. Exiting...\n");
        return 1;
    }

    printf("Enter number of processes: ");
    scanf("%d", &no_of_process);

    Process *processes = (Process *)malloc(sizeof(Process) * no_of_process);
    if (processes == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(0);
    }

    MemoryTableEntry *memory_table = (MemoryTableEntry *)malloc(sizeof(MemoryTableEntry));
    if (memory_table == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(0);
    }

    int memory_entries = 1;

    memory_table[0].status = 0;
    memory_table[0].process_id = -1;
    memory_table[0].starting_address = 0;
    memory_table[0].ending_address = size_of_mm - 1;
    memory_table[0].size = size_of_mm;

    for (int i = 0; i < no_of_process; i++)
    {
        printf("\nProcess %d\n", i + 1);
        processes[i].process_id = i + 1;
        processes[i].completed = 0;
        printf("Arrival time (ms): ");
        scanf("%d", &processes[i].arrival_time);
        printf("Execution time (ms): ");
        scanf("%d", &processes[i].execution_time);
        get_size(&processes[i].size, "Size (e.g., 10 MB, 13.5 kB, 100 B): ");
    }

    printf("\n");
    print_process_table(processes, no_of_process);

    int time = 0;
    int current_pid = -1;
    int current_completion_time = 0;

    printf("\n=== Simulation Start ===\n");

    while (!all_processes_completed(processes, no_of_process))
    {
        printf("\n[Time: %d ms]\n", time);

        // Process Completion
        if (current_pid != -1 && time >= current_completion_time)
        {
            printf("Process %d completed execution.\n", current_pid);
            deallocate_process(&memory_table, &memory_entries, current_pid);

            for (int i = 0; i < no_of_process; i++)
            {
                if (processes[i].process_id == current_pid)
                {
                    processes[i].completed = 1;
                    break;
                }
            }

            current_pid = -1;

            // Try to allocate waiting processes
            while (!is_empty(&waiting_queue))
            {
                Process oldest_waiting = peek_front(&waiting_queue);

                if (allocate_process(&memory_table, &memory_entries, oldest_waiting))
                {
                    oldest_waiting = dequeue(&waiting_queue);
                    enqueue(&ready_queue, oldest_waiting);
                    printf("Process %d from waiting queue allocated memory.\n", oldest_waiting.process_id);
                }
                else
                {
                    if (is_memory_compacted(memory_table, memory_entries))
                    {
                        printf("Process %d from waiting queue could not be allocated memory.\n", oldest_waiting.process_id);
                        break;
                    }
                    else
                    {
                        printf("Process %d from waiting queue could not be allocated memory. Trying to compact memory...\n", oldest_waiting.process_id);
                        printf("\nMemory After Compaction:\n");
                        print_memory_table(memory_table, memory_entries);
                        print_memory(memory_table, memory_entries, size_of_mm);
                        printf("\n");

                        compact_memory(&memory_table, &memory_entries, size_of_mm);

                        printf("\nMemory After Compaction:\n");
                        print_memory_table(memory_table, memory_entries);
                        print_memory(memory_table, memory_entries, size_of_mm);
                        printf("\n");

                        // ⚡ Try allocation again
                        if (allocate_process(&memory_table, &memory_entries, oldest_waiting))
                        {
                            oldest_waiting = dequeue(&waiting_queue);
                            enqueue(&ready_queue, oldest_waiting);
                            printf("Process %d from waiting queue allocated memory after compaction.\n", oldest_waiting.process_id);
                        }
                        else
                        {
                            printf("Process %d from waiting queue could not be allocated memory even after compaction.\n", oldest_waiting.process_id);
                            // 🚪 Break because if compaction didn't help, next waiting processes also won't fit
                            break;
                        }
                    }
                }
            }
        }

        // New process arrivals
        for (int i = 0; i < no_of_process; i++)
        {
            if (processes[i].arrival_time == time)
            {
                printf("Process %d arrived. Trying to allocate memory...\n", processes[i].process_id);
                int is_allocated = allocate_process(&memory_table, &memory_entries, processes[i]);
                if (is_allocated)
                {
                    printf("Process %d allocated memory successfully.\n", processes[i].process_id);
                    enqueue(&ready_queue, processes[i]);
                }
                else
                {
                    if (is_memory_compacted(memory_table, memory_entries))
                    {
                        printf("Process %d could not be allocated memory. Added to waiting queue\n", processes[i].process_id);
                        enqueue(&waiting_queue, processes[i]);
                    }
                    else
                    {
                        printf("Process %d could not be allocated memory. Trying to compact memory...\n", processes[i].process_id);

                        printf("\nMemory Table Before Compaction\n");
                        print_memory_table(memory_table, memory_entries);
                        print_memory(memory_table, memory_entries, size_of_mm);
                        printf("\n");

                        compact_memory(&memory_table, &memory_entries, size_of_mm);

                        printf("\nMemory Table After Compaction\n");
                        print_memory_table(memory_table, memory_entries);
                        print_memory(memory_table, memory_entries, size_of_mm);
                        printf("\n");

                        if (allocate_process(&memory_table, &memory_entries, processes[i]))
                        {
                            printf("Process %d allocated memory successfully after compaction.\n", processes[i].process_id);
                            enqueue(&ready_queue, processes[i]);
                        }
                        else
                        {
                            printf("Process %d could not be allocated memory even after compaction. Added to waiting queue.\n", processes[i].process_id);
                            enqueue(&waiting_queue, processes[i]);
                        }
                    }
                }
            }
        }

        // New process execution
        if (current_pid == -1 && !is_empty(&ready_queue))
        {
            Process next = dequeue(&ready_queue);
            current_pid = next.process_id;
            current_completion_time = time + next.execution_time;
            printf("Process %d started execution (Expected completion at %d ms).\n", current_pid, current_completion_time);
        }

        // Print memory status
        printf("\nCurrent Memory Table:\n");
        print_memory_table(memory_table, memory_entries);
        print_memory(memory_table, memory_entries, size_of_mm);

        time++;
    }

    printf("\n=== Simulation Complete ===\n");

    free(processes);
    free(memory_table);
    return 0;
}
