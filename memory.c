#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

// Insert a memory block into the memory table at a specific index
void insert_memory_block(MemoryTableEntry **table, int *count, int index, MemoryTableEntry new_entry) {
    // Reallocate memory to make space for the new block
    *table = (MemoryTableEntry *)realloc(*table, sizeof(MemoryTableEntry) * (*count + 1));

    if (*table == NULL) {
        printf("Memory reallocation failed.\n");
        exit(0);
    }

    // Shift blocks to the right to make space at the index
    for (int i = *count; i > index; i--) {
        (*table)[i] = (*table)[i - 1];
    }

    // Insert the new memory block
    (*table)[index] = new_entry;
    (*count)++;
}

// Allocate memory to a process using First Fit strategy
int allocate_process(MemoryTableEntry **memory_table, int *entries, Process p) {
    for (int i = 0; i < *entries; i++) {
        // Find first free block large enough
        if ((*memory_table)[i].status == 0 && (*memory_table)[i].size >= p.size) {
            int remaining = (*memory_table)[i].size - p.size;

            // Occupy the current block
            (*memory_table)[i].status = 1;
            (*memory_table)[i].process_id = p.process_id;
            (*memory_table)[i].size = p.size;
            (*memory_table)[i].ending_address = (*memory_table)[i].starting_address + p.size - 1;
    
            // If there is leftover space, create a new free block
            if (remaining > 0) {
                MemoryTableEntry new_block;
                new_block.status = 0;
                new_block.size = remaining;
                new_block.process_id = -1;
                new_block.starting_address = (*memory_table)[i].starting_address + p.size;
                new_block.ending_address = (*memory_table)[i].starting_address + p.size + remaining - 1;

                insert_memory_block(memory_table, entries, i + 1, new_block);
            }

            return 1; // Allocation successful
        }
    }
    return 0; // Allocation failed
}

// Remove a memory block from the memory table
void remove_memory_block(MemoryTableEntry **memory_table, int *entries, int index) {
    // Shift all blocks after the index one position to the left
    for (int j = index; j < *entries - 1; j++) {
        (*memory_table)[j] = (*memory_table)[j + 1];
    }

    // Decrease the total number of entries
    (*entries)--;

    // Reallocate memory for the reduced array
    *memory_table = realloc(*memory_table, sizeof(MemoryTableEntry) * (*entries));
}

// Deallocate memory occupied by a specific process
void deallocate_process(MemoryTableEntry **memory_table, int *entries, int process_id) {
    int found = 0;
    
    // Mark all blocks belonging to the process as free
    for (int i = 0; i < *entries; i++) {
        if ((*memory_table)[i].process_id == process_id) {
            (*memory_table)[i].status = 0;
            (*memory_table)[i].process_id = -1;
            found = 1;
        }
    }

    if (!found) return; // Process not found

    // Merge adjacent free blocks
    for (int i = 0; i < *entries - 1; ) {
        if ((*memory_table)[i].status == 0 && (*memory_table)[i+1].status == 0) {
            // Merge with next block
            (*memory_table)[i].size += (*memory_table)[i+1].size;
            (*memory_table)[i].ending_address = (*memory_table)[i+1].ending_address;

            // Remove the next block
            remove_memory_block(memory_table, entries, i + 1);
        } else {
            i++;
        }
    }
}

// Check if the memory is already compacted (no gaps between occupied blocks)
int is_memory_compacted(MemoryTableEntry *memory_table, int entries) {
    int last_end_address = -1;

    for (int i = 0; i < entries; i++) {
        if (memory_table[i].status == 1) { // Check only occupied blocks
            if (memory_table[i].starting_address > last_end_address + 1) {
                return 0; // Gap found, not compacted
            }
            last_end_address = memory_table[i].ending_address;
        }
    }

    return 1; // No gaps found, memory is compacted
}

// Compact memory by moving occupied blocks together
void compact_memory(MemoryTableEntry **memory_table, int *entries, int total_memory_size) {
    int current_address = 0;
    int i = 0;

    // Move occupied blocks to the start
    while (i < *entries) {
        if ((*memory_table)[i].status == 1) {
            int block_size = (*memory_table)[i].size;
            (*memory_table)[i].starting_address = current_address;
            (*memory_table)[i].ending_address = current_address + block_size - 1;
            current_address += block_size;
            i++;
        } else {
            // Remove free block
            remove_memory_block(memory_table, entries, i);
        }
    }

    // Add a single free block at the end if there is remaining space
    if (current_address < total_memory_size) {
        MemoryTableEntry free_block;
        free_block.status = 0;
        free_block.process_id = -1;
        free_block.starting_address = current_address;
        free_block.ending_address = total_memory_size - 1;
        free_block.size = free_block.ending_address - free_block.starting_address + 1;

        insert_memory_block(memory_table, entries, *entries, free_block);
    }
}
