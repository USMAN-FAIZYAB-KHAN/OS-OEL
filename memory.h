#ifndef MEMORY_H
#define MEMORY_H

#include "process.h"  // Include the Process header for Process struct

// Structure representing a block in memory
typedef struct {
    int status;              // 0 = Free, 1 = Occupied
    int size;                // Size of the memory block
    int process_id;          // ID of the process using this memory block (-1 if free)
    int starting_address;    // Starting address of the memory block
    int ending_address;      // Ending address of the memory block
} MemoryTableEntry;

// Insert a new memory block into the memory table at a specified index
void insert_memory_block(MemoryTableEntry **table, int *count, int index, MemoryTableEntry new_entry);

// Allocate memory for a given process, returns index of allocated block or -1 if failed
int allocate_process(MemoryTableEntry **memory_table, int *entries, Process p);

// Compact memory to remove fragmentation and combine free spaces
void compact_memory(MemoryTableEntry **memory_table, int *entries, int total_memory_size);

// Check if memory is compacted (no free spaces between occupied blocks)
int is_memory_compacted(MemoryTableEntry *memory_table, int entries);

// Remove a memory block from the table at a specified index
void remove_memory_block(MemoryTableEntry **memory_table, int *entries, int index);

// Deallocate memory used by a process with the given process_id
void deallocate_process(MemoryTableEntry **memory_table, int *entries, int process_id);

#endif // MEMORY_H
