#include <stdio.h>
#include <string.h>
#include "display.h"

void print_process_table(Process *processes, int count) {
    int total_size = 0;

    printf("+------------+-------------------+---------------------+-----------------+\n");
    printf("| Process ID | Arrival Time (ms) | Execution Time (ms) | Size (bytes)    |\n");
    printf("+------------+-------------------+---------------------+-----------------+\n");

    int max_size = processes[0].size;
    int min_size = processes[0].size;

    for (int i = 0; i < count; i++) {
        printf("| %-10d | %-17d | %-19d | %-15d |\n", processes[i].process_id, processes[i].arrival_time, processes[i].execution_time, processes[i].size);
        total_size += processes[i].size;
        
        if (processes[i].size > max_size) {
            max_size = processes[i].size;
        }

        if (processes[i].size < min_size) {
            min_size = processes[i].size;
        }
    }

    int average_size = (count > 0) ? total_size / count : 0;

    printf("+------------+-------------------+---------------------+-----------------+\n");
    printf("| Average    |                   |                     | %-15d |\n", average_size);
    printf("| Minimum    |                   |                     | %-15d |\n", min_size);
    printf("| Maximum    |                   |                     | %-15d |\n", max_size);
    printf("+------------+-------------------+---------------------+-----------------+\n");
}

void print_memory_table(MemoryTableEntry *table, int count) {
    printf("\nMemory Table:\n");
    printf("+--------------+--------------+--------------+------------+----------------+\n");
    printf("| Start Addr   | Ending Addr  | Size (bytes) | Status     | Process ID     |\n");
    printf("+--------------+--------------+--------------+------------+----------------+\n");
    for (int i = 0; i < count; i++) {
        printf("| %-12d | %-12d | %-12d | %-10s | %-14d |\n",
               table[i].starting_address,
               table[i].ending_address,
               table[i].size,
               (table[i].status == 0) ? "Free" : "Occupied",
               table[i].process_id);
    }
    printf("+--------------+--------------+--------------+------------+----------------+\n");
}

void print_memory(MemoryTableEntry *table, int count, int total_size) {
    int width = 100; // total width
    int height = 5;  // odd for vertical centering

    // Top Border
    printf("+");
    for (int i = 0; i < width; i++) printf("-");
    printf("+\n");

    // Body
    for (int row = 0; row < height; row++) {
        printf("|");

        int current_width = 0;

        for (int i = 0; i < count; i++) {
            int block_width = (table[i].size * width) / total_size;

            if (block_width <= 0) continue; // Skip too small blocks

            // Reserve 1 character for internal separator if not last block
            int inside_width = block_width;
            if (i != count - 1) inside_width--;

            if (table[i].process_id != -1) { // Occupied block
                char label[10];
                sprintf(label, " P%d ", table[i].process_id);
                int label_len = strlen(label);

                if (row == height/2) { // Center Process ID horizontally
                    if (inside_width <= label_len) {
                        printf("%s", label);
                        for (int j = label_len; j < inside_width; j++) printf("-");
                    } else {
                        int left_padding = (inside_width - label_len) / 2;
                        int right_padding = inside_width - label_len - left_padding;

                        for (int j = 0; j < left_padding; j++) printf("-");
                        printf("%s", label);
                        for (int j = 0; j < right_padding; j++) printf("-");
                    }
                } else { // Other rows just '-'
                    for (int j = 0; j < inside_width; j++) printf("-");
                }
            } else { // Free block
                for (int j = 0; j < inside_width; j++) printf(" ");
            }

            if (i != count - 1) {
                printf("|");
            }

            current_width += block_width;
        }

        // Fill any missing space due to rounding
        while (current_width < width) {
            printf(" ");
            current_width++;
        }

        printf("|\n");
    }

    // Bottom Border
    printf("+");
    for (int i = 0; i < width; i++) printf("-");
    printf("+\n");
}