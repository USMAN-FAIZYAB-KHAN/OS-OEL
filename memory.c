#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

// Insert memory block into memory table
void insert_memory_block(MemoryTableEntry **table, int *count, int index, MemoryTableEntry new_entry) {
    *table = (MemoryTableEntry *)realloc(*table, sizeof(MemoryTableEntry) * (*count + 1));

    if (*table == NULL) {
        printf("Memory reallocation failed.\n");
        exit(0);
    }

    // Shift right
    for (int i = *count; i > index; i--) {
        (*table)[i] = (*table)[i - 1];
    }

    (*table)[index] = new_entry;
    (*count)++;
}

// First Fit allocation
int allocate_process(MemoryTableEntry **memory_table, int *entries, Process p) {
    for (int i = 0; i < *entries; i++) {
        if ((*memory_table)[i].status == 0 && (*memory_table)[i].size >= p.size) {
            int remaining = (*memory_table)[i].size - p.size;

            // Occupy current block
            (*memory_table)[i].status = 1;
            (*memory_table)[i].process_id = p.process_id;
            (*memory_table)[i].size = p.size;
            (*memory_table)[i].ending_address = (*memory_table)[i].starting_address + p.size - 1;
    
            if (remaining > 0) {
                // Create a new free block for leftover space
                MemoryTableEntry new_block;
                new_block.status = 0;
                new_block.size = remaining;
                new_block.process_id = -1;
                new_block.starting_address = (*memory_table)[i].starting_address + p.size;
                new_block.ending_address = (*memory_table)[i].starting_address + p.size + remaining -1;

                insert_memory_block(memory_table, entries, i + 1, new_block);
            }

            return 1;
        }
    }
    return 0;
}


// Function to remove an entry from the memory table array
void remove_memory_block(MemoryTableEntry **memory_table, int *entries, int index) {
    // Shift all elements after the index one position left
    for (int j = index; j < *entries - 1; j++) {
        (*memory_table)[j] = (*memory_table)[j + 1];
    }
    
    // Reduce the count of entries
    (*entries)--;
    
    // Reallocate memory for the smaller array
    *memory_table = realloc(*memory_table, sizeof(MemoryTableEntry) * (*entries));
}

// Updated deallocate_process function using the new helper function
void deallocate_process(MemoryTableEntry **memory_table, int *entries, int process_id) {
    int found = 0;
    
    // Mark all blocks with this process as free
    for (int i = 0; i < *entries; i++) {
        if ((*memory_table)[i].process_id == process_id) {
            (*memory_table)[i].status = 0;
            (*memory_table)[i].process_id = -1;
            found = 1;
        }
    }
    
    if (!found) return;
    
    // Merge adjacent free blocks
    for (int i = 0; i < *entries - 1; ) {
        if ((*memory_table)[i].status == 0 && (*memory_table)[i+1].status == 0) {
            // Merge blocks
            (*memory_table)[i].size += (*memory_table)[i+1].size;
            (*memory_table)[i].ending_address = (*memory_table)[i+1].ending_address;
            
            // Use our new function to remove the next block
            remove_memory_block(memory_table, entries, i + 1);
        } else {
            i++;
        }
    }
}

// Function to check if the memory is already compacted
int is_memory_compacted(MemoryTableEntry *memory_table, int entries) {
    int last_end_address = -1;

    // Check if any allocated block is not continuous
    for (int i = 0; i < entries; i++) {
        if (memory_table[i].status == 1) { // Only check occupied blocks
            if (memory_table[i].starting_address > last_end_address + 1) {
                // There is a gap between allocated blocks
                return 0; // Memory is not compacted
            }
            last_end_address = memory_table[i].ending_address;
        }
    }

    // If no gaps were found, the memory is compacted
    return 1;
}

void compact_memory(MemoryTableEntry **memory_table, int *entries, int total_memory_size) {
    int current_address = 0;
    int i = 0;

    while (i < *entries) {
        if ((*memory_table)[i].status == 1) { // Occupied block
            int block_size = (*memory_table)[i].size;
            (*memory_table)[i].starting_address = current_address;
            (*memory_table)[i].ending_address = current_address + block_size - 1;
            current_address += block_size;
            i++;
        } else {
            // Remove the free block
            remove_memory_block(memory_table, entries, i);
        }
    }

    // After shifting, if there is leftover memory, create one free block
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
