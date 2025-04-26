#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
    int process_id;
    int arrival_time;
    int size;
} Process;

typedef struct 
{
    int status;             // 0 = free, 1 = occupied
    int size;               // size of memory block
    int process_id;         // id of process occupying this block (-1 if free)
    int starting_address;   // starting address of this block
    int ending_address;
} MemoryTableEntry;

void get_size(int *size_in_bytes, const char *prompt) {
    float size;
    char unit[3]; 

    printf("%s", prompt);

    if (scanf("%f %2s", &size, unit) != 2) {
        printf("Invalid input.\n");
        *size_in_bytes = -1;
        return;
    }

    if (strcmp(unit, "MB") == 0) {
        *size_in_bytes = (int)(size * 1024 * 1024);
    } else if (strcmp(unit, "kB") == 0) {
        *size_in_bytes = (int)(size * 1024);
    } else if (strcmp(unit, "B") == 0) {
        *size_in_bytes = (int)size;
    } else {
        printf("Invalid unit. Please enter MB, kB, or B.\n");
        *size_in_bytes = -1;
    }
}

void print_process_table(Process *processes, int count) {
    int total_size = 0;

    printf("+------------+-------------------+-----------------+\n");
    printf("| Process ID | Arrival Time (ms) | Size (bytes)    |\n");
    printf("+------------+-------------------+-----------------+\n");

    int max_size = processes[0].size;
    int min_size = processes[0].size;

    for (int i = 0; i < count; i++) {
        printf("| %-10d | %-17d | %-15d |\n", processes[i].process_id, processes[i].arrival_time, processes[i].size);
        total_size += processes[i].size;
        
        if (processes[i].size > max_size) {
            max_size = processes[i].size;
        }

        if (processes[i].size < min_size) {
            min_size = processes[i].size;
        }
    }

    int average_size = (count > 0) ? total_size / count : 0;

    printf("+------------+-------------------+-----------------+\n");
    printf("| Average    |                   | %-15d |\n", average_size);
    printf("| Minimum    |                   | %-15d |\n", min_size);
    printf("| Maximum    |                   | %-15d |\n", max_size);
    printf("+------------+-------------------+-----------------+\n");
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
void allocate_process(MemoryTableEntry **memory_table, int *entries, Process p) {
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

            printf("Process %d allocated successfully.\n", p.process_id);
            return;
        }
    }
    printf("Process %d could not be allocated (Not enough memory).\n", p.process_id);
}

int main() {
    int no_of_process, size_of_mm;

    system("clear");

    get_size(&size_of_mm, "Size of main memory (e.g., 10 MB, 13.5 kB, 100 B): ");

    if (size_of_mm == -1) {
        printf("Invalid memory size input. Exiting...\n");
        return 1;
    }

    printf("Enter number of processes: ");
    scanf("%d", &no_of_process);

    Process *processes = (Process *)malloc(sizeof(Process) * no_of_process);
    if (processes == NULL) {
        printf("Memory allocation failed.\n");
        exit(0);
    }

    MemoryTableEntry *memory_table = (MemoryTableEntry *)malloc(sizeof(MemoryTableEntry));
    if (memory_table == NULL) {
        printf("Memory allocation failed.\n");
        exit(0);
    }
    
    int memory_entries = 1;

    // Initially entire memory is free
    memory_table[0].status = 0;
    memory_table[0].process_id = -1;
    memory_table[0].starting_address = 0;
    memory_table[0].ending_address = size_of_mm -1;
    memory_table[0].size = size_of_mm;

    for (int i = 0; i < no_of_process; i++) {
        printf("\nProcess %d\n", i+1);
        processes[i].process_id = i+1;
        printf("Arrival time (ms): ");
        scanf("%d", &processes[i].arrival_time);
        get_size(&processes[i].size, "Size (e.g., 10 MB, 13.5 kB, 100 B): ");
    }

    printf("\n");
    print_process_table(processes, no_of_process);

    // Find maximum arrival time
    int max_arrival_time = processes[0].arrival_time;
    for (int i = 1; i < no_of_process; i++) {
        if (processes[i].arrival_time > max_arrival_time) {
            max_arrival_time = processes[i].arrival_time;
        }
    }

    printf("\n");

    for (int time = 0; time <= max_arrival_time; time++) {
        printf("\nTime (%d ms)\n", time);

        for (int i = 0; i < no_of_process; i++) {
            if (processes[i].arrival_time == time) {
                printf("Allocating PID %d...\n", processes[i].process_id);
                allocate_process(&memory_table, &memory_entries, processes[i]);
            }
        }

        print_memory_table(memory_table, memory_entries);
        print_memory(memory_table, memory_entries, size_of_mm);
    }

    free(processes);
    free(memory_table);
    return 0;
}
