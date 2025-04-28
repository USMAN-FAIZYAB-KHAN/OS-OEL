#include <stdio.h>
#include <string.h>
#include <stdarg.h> 
#include "display.h"


// Global file pointer
FILE *output_file = NULL;

// Dualprintf function: prints to console and to file (passed as parameter)
void dualprintf(const char *format, ...)
{
    va_list args1, args2;
    va_start(args1, format);
    va_copy(args2, args1);

    vprintf(format, args1);      // Print to console
    if (output_file != NULL)
    {
        vfprintf(output_file, format, args2); // Print to the provided file
    }

    va_end(args1);
    va_end(args2);
}


// Print the process table with details and statistics
void print_process_table(Process *processes, int count) {
    int total_size = 0;

   dualprintf("+------------+-------------------+---------------------+-----------------+\n");
   dualprintf("| Process ID | Arrival Time (ms) | Execution Time (ms) | Size (bytes)    |\n");
   dualprintf("+------------+-------------------+---------------------+-----------------+\n");

    int max_size = processes[0].size;
    int min_size = processes[0].size;

    for (int i = 0; i < count; i++) {
       dualprintf("| %-10d | %-17d | %-19d | %-15d |\n", 
               processes[i].process_id, 
               processes[i].arrival_time, 
               processes[i].execution_time, 
               processes[i].size);
        total_size += processes[i].size;
        
        if (processes[i].size > max_size) {
            max_size = processes[i].size;
        }

        if (processes[i].size < min_size) {
            min_size = processes[i].size;
        }
    }

    int average_size = (count > 0) ? total_size / count : 0;

   dualprintf("+------------+-------------------+---------------------+-----------------+\n");
   dualprintf("| Average    |                   |                     | %-15d |\n", average_size);
   dualprintf("| Minimum    |                   |                     | %-15d |\n", min_size);
   dualprintf("| Maximum    |                   |                     | %-15d |\n", max_size);
   dualprintf("+------------+-------------------+---------------------+-----------------+\n");
}

// Print the memory table with block details
void print_memory_table(MemoryTableEntry *table, int count) {
   dualprintf("+--------------+--------------+--------------+------------+----------------+\n");
   dualprintf("| Start Addr   | Ending Addr  | Size (bytes) | Status     | Process ID     |\n");
   dualprintf("+--------------+--------------+--------------+------------+----------------+\n");
    
    for (int i = 0; i < count; i++) {
       dualprintf("| %-12d | %-12d | %-12d | %-10s | %-14d |\n",
               table[i].starting_address,
               table[i].ending_address,
               table[i].size,
               (table[i].status == 0) ? "Free" : "Occupied",
               table[i].process_id);
    }
    
   dualprintf("+--------------+--------------+--------------+------------+----------------+\n");
}

// Print a visual representation of memory blocks
void print_memory(MemoryTableEntry *table, int count, int total_size) {
    int width = 100; // total width
    int height = 5;  // total height

    // Top border
   dualprintf("+");
    for (int i = 0; i < width; i++)dualprintf("-");
   dualprintf("+\n");

    // Print each row of memory
    for (int row = 0; row < height; row++) {
       dualprintf("|");

        int current_width = 0;

        for (int i = 0; i < count; i++) {
            int block_width = (table[i].size * width) / total_size;

            if (block_width <= 0) continue; // Skip very small blocks

            int inside_width = block_width;
            if (i != count - 1) inside_width--;

            if (table[i].process_id != -1) { // Occupied block
                char label[10];
                sprintf(label, " P%d ", table[i].process_id);
                int label_len = strlen(label);

                if (row == height / 2) { // Center process ID horizontally
                    if (inside_width <= label_len) {
                       dualprintf("%s", label);
                        for (int j = label_len; j < inside_width; j++)dualprintf("-");
                    } else {
                        int left_padding = (inside_width - label_len) / 2;
                        int right_padding = inside_width - label_len - left_padding;

                        for (int j = 0; j < left_padding; j++)dualprintf("-");
                       dualprintf("%s", label);
                        for (int j = 0; j < right_padding; j++)dualprintf("-");
                    }
                } else { // Other rows are filled with '-'
                    for (int j = 0; j < inside_width; j++)dualprintf("-");
                }
            } else { // Free block
                for (int j = 0; j < inside_width; j++)dualprintf(" ");
            }

            if (i != count - 1) {
               dualprintf("|");
            }

            current_width += block_width;
        }

        // Fill any missing space due to rounding
        while (current_width < width) {
           dualprintf(" ");
            current_width++;
        }

       dualprintf("|\n");
    }

    // Bottom border
   dualprintf("+");
    for (int i = 0; i < width; i++)dualprintf("-");
   dualprintf("+\n");
}
