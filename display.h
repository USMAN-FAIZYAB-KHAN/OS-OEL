#ifndef DISPLAY_H
#define DISPLAY_H

#include "process.h"
#include "memory.h"

void print_process_table(Process *processes, int count);
void print_memory_table(MemoryTableEntry *table, int count);
void print_memory(MemoryTableEntry *table, int count, int total_size);

#endif // DISPLAY_H
