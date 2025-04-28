#ifndef DISPLAY_H
#define DISPLAY_H

#include "process.h"
#include "memory.h"

extern FILE *output_file;

// Dual printf function: prints to console and to file
void dualprintf(const char *format, ...);

// Print the table of processes
void print_process_table(Process *processes, int count);

// Print the memory table
void print_memory_table(MemoryTableEntry *table, int count);

// Print memory blocks and free space
void print_memory(MemoryTableEntry *table, int count, int total_size);

#endif // DISPLAY_H
