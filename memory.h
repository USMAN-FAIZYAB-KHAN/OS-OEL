#ifndef MEMORY_H
#define MEMORY_H

#include "process.h"  // Include the Process header for Process struct

// MemoryTableEntry structure for memory block management
typedef struct {
    int status;              // 0 = Free, 1 = Occupied
    int size;                // Size of the memory block
    int process_id;          // Process ID using this memory block
    int starting_address;    // Starting address of the block
    int ending_address;      // Ending address of the block
} MemoryTableEntry;

// Function prototypes
void insert_memory_block(MemoryTableEntry **table, int *count, int index, MemoryTableEntry new_entry);
int allocate_process(MemoryTableEntry **memory_table, int *entries, Process p);
void compact_memory(MemoryTableEntry **memory_table, int *entries, int total_memory_size);
void remove_memory_block(MemoryTableEntry **memory_table, int *entries, int index);
void deallocate_process(MemoryTableEntry **memory_table, int *entries, int process_id);

#endif // MEMORY_H
