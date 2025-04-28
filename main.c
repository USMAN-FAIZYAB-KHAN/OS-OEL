#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "display.h"
#include "process.h"
#include "memory.h"

// Main function
int main()
{
    int no_of_process, size_of_mm;
    ProcessQueue ready_queue, waiting_queue;

    // Open output file for logging results
    output_file = fopen("output.txt", "w");
    if (output_file == NULL)
    {
        printf("Could not open output file.\n");
        return 1;
    }

    // Initialize the ready and waiting queues
    init_queue(&ready_queue);
    init_queue(&waiting_queue);

    system("clear"); // Clear the console screen

    // Get the size of main memory from the user
    get_size(&size_of_mm, "Size of main memory (e.g., 10 MB, 13.5 kB, 100 B): ");

    // Handle invalid memory size
    if (size_of_mm == -1)
    {
        printf("Invalid memory size input. Exiting...\n");
        fclose(output_file);
        return 1;
    }

    // Get number of processes from user
    printf("Enter number of processes: ");
    scanf("%d", &no_of_process);

    // Allocate memory for process information
    Process *processes = (Process *)malloc(sizeof(Process) * no_of_process);
    if (processes == NULL)
    {
        printf("Memory allocation failed.\n");
        fclose(output_file);
        exit(0);
    }

    // Allocate memory for memory table (initially only one free block)
    MemoryTableEntry *memory_table = (MemoryTableEntry *)malloc(sizeof(MemoryTableEntry));
    if (memory_table == NULL)
    {
        printf("Memory allocation failed.\n");
        fclose(output_file);
        exit(0);
    }

    int memory_entries = 1; // Initially only one memory block

    // Initialize memory table with a single free block
    memory_table[0].status = 0;      // 0 means free
    memory_table[0].process_id = -1; // No process yet
    memory_table[0].starting_address = 0;
    memory_table[0].ending_address = size_of_mm - 1;
    memory_table[0].size = size_of_mm;

    // Get details of all processes from the user
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
    // Print the process table
    print_process_table(processes, no_of_process);

    // Initialize simulation variables
    int time = 0;
    int current_pid = -1;
    int current_completion_time = 0;

    dualprintf("\n=== Simulation Start ===\n");

    // Main simulation loop: continue until all processes are completed
    while (!all_processes_completed(processes, no_of_process))
    {
        dualprintf("\n[Time: %d ms]\n", time);

        // Check if current running process is completed
        if (current_pid != -1 && time >= current_completion_time)
        {
            dualprintf("Process %d completed execution.\n", current_pid);
            // Free memory of completed process
            deallocate_process(&memory_table, &memory_entries, current_pid);

            // Mark process as completed
            for (int i = 0; i < no_of_process; i++)
            {
                if (processes[i].process_id == current_pid)
                {
                    processes[i].completed = 1;
                    break;
                }
            }

            current_pid = -1; // No process running now

            // Try to allocate memory for processes in waiting queue
            while (!is_empty(&waiting_queue))
            {
                Process oldest_waiting = peek_front(&waiting_queue);

                if (allocate_process(&memory_table, &memory_entries, oldest_waiting))
                {
                    // Allocation successful
                    oldest_waiting = dequeue(&waiting_queue);
                    enqueue(&ready_queue, oldest_waiting);
                    dualprintf("Process %d from waiting queue allocated memory.\n", oldest_waiting.process_id);
                }
                else
                {
                    // Try memory compaction if allocation failed
                    if (is_memory_compacted(memory_table, memory_entries))
                    {
                        dualprintf("Process %d from waiting queue could not be allocated memory.\n", oldest_waiting.process_id);
                        break;
                    }
                    else
                    {
                        dualprintf("Process %d from waiting queue could not be allocated memory. Trying to compact memory...\n", oldest_waiting.process_id);

                        // Show memory state before compaction
                        dualprintf("\nMemory Before Compaction:\n");
                        print_memory_table(memory_table, memory_entries);
                        print_memory(memory_table, memory_entries, size_of_mm);
                        dualprintf("\n");

                        // Compact memory
                        compact_memory(&memory_table, &memory_entries, size_of_mm);

                        // Show memory state after compaction
                        dualprintf("\nMemory After Compaction:\n");
                        print_memory_table(memory_table, memory_entries);
                        print_memory(memory_table, memory_entries, size_of_mm);
                        dualprintf("\n");

                        if (allocate_process(&memory_table, &memory_entries, oldest_waiting))
                        {
                            oldest_waiting = dequeue(&waiting_queue);
                            enqueue(&ready_queue, oldest_waiting);
                            dualprintf("Process %d from waiting queue allocated memory after compaction.\n", oldest_waiting.process_id);
                        }
                        else
                        {
                            dualprintf("Process %d from waiting queue could not be allocated memory even after compaction.\n", oldest_waiting.process_id);
                            break;
                        }
                    }
                }
            }
        }

        // Check if any new processes have arrived at current time
        for (int i = 0; i < no_of_process; i++)
        {
            if (processes[i].arrival_time == time)
            {
                dualprintf("Process %d arrived. Checking waiting queue...\n", processes[i].process_id);

                // Only allocate memory if there are no processes in the waiting queue
                if (is_empty(&waiting_queue))
                {
                    dualprintf("Waiting queue is empty. Trying to allocate memory for Process %d...\n", processes[i].process_id);

                    int is_allocated = allocate_process(&memory_table, &memory_entries, processes[i]);
                    if (is_allocated)
                    {
                        dualprintf("Process %d allocated memory successfully.\n", processes[i].process_id);
                        enqueue(&ready_queue, processes[i]);
                    }
                    else
                    {
                        // Try compaction if memory is fragmented
                        if (is_memory_compacted(memory_table, memory_entries))
                        {
                            dualprintf("Process %d could not be allocated memory. Added to waiting queue.\n", processes[i].process_id);
                            enqueue(&waiting_queue, processes[i]);
                        }
                        else
                        {
                            dualprintf("Process %d could not be allocated memory. Trying to compact memory...\n", processes[i].process_id);

                            // Show memory before compaction
                            dualprintf("\nMemory Before Compaction:\n");
                            print_memory_table(memory_table, memory_entries);
                            print_memory(memory_table, memory_entries, size_of_mm);
                            dualprintf("\n");

                            compact_memory(&memory_table, &memory_entries, size_of_mm);

                            // Show memory after compaction
                            dualprintf("\nMemory After Compaction:\n");
                            print_memory_table(memory_table, memory_entries);
                            print_memory(memory_table, memory_entries, size_of_mm);
                            dualprintf("\n");

                            if (allocate_process(&memory_table, &memory_entries, processes[i]))
                            {
                                dualprintf("Process %d allocated memory successfully after compaction.\n", processes[i].process_id);
                                enqueue(&ready_queue, processes[i]);
                            }
                            else
                            {
                                dualprintf("Process %d could not be allocated memory even after compaction. Added to waiting queue.\n", processes[i].process_id);
                                enqueue(&waiting_queue, processes[i]);
                            }
                        }
                    }
                }
                else
                {
                    dualprintf("There are processes in the waiting queue. Process %d is added to the waiting queue.\n", processes[i].process_id);
                    enqueue(&waiting_queue, processes[i]);
                }
            }
        }

        // If no process running and ready queue is not empty, start the next process
        if (current_pid == -1 && !is_empty(&ready_queue))
        {
            Process next = dequeue(&ready_queue);
            current_pid = next.process_id;
            current_completion_time = time + next.execution_time;
            dualprintf("Process %d started execution (Expected completion at %d ms).\n", current_pid, current_completion_time);
        }

        // Display current waiting and ready queues
        dualprintf("\nWaiting Queue\n");
        display_queue(&waiting_queue);

        dualprintf("\nReady Queue\n");
        display_queue(&ready_queue);

        // Display memory table
        dualprintf("\nCurrent Memory Table:\n");
        print_memory_table(memory_table, memory_entries);
        dualprintf("\n");
        print_memory(memory_table, memory_entries, size_of_mm);

        // Move to next millisecond
        time++;
    }

    dualprintf("\n=== Simulation Complete ===\n");

    // Free allocated memory
    free(processes);
    free(memory_table);

    // Close the output file
    fclose(output_file);

    return 0;
}
