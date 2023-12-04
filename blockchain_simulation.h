#pragma once

#include <stdbool.h>

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0) // returns true if >= 0
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L) // error code
#define HASH_SIZE 255
#define CSV_LINE_SIZE 1024
#define TIMESTAMP_SIZE 64
#define COLS 10

typedef struct hash_info_needed {
    char timestamp[TIMESTAMP_SIZE];
    double data;
    char *previous_hash;
} hash_info_needed;

typedef struct block_node {
    char timestamp[TIMESTAMP_SIZE];
    int block_number;
    double data; // Cee is currency, data is amount
    char hash[HASH_SIZE];
    char previous_hash[HASH_SIZE];
    struct block_node *next;
} block_node;

char* make_hash(hash_info_needed *block);
block_node* add_block(block_node *head, int block_number, char new_timestamp[TIMESTAMP_SIZE], 
                      double new_data, char new_hash[HASH_SIZE], char new_previous_hash[HASH_SIZE]);
void propagate_to_2D_array(block_node **block_node_ptrs, block_node *head_ptr, int rows, int cols);
// TRIPLE pointer...... to modify the value **bnptr is pting to, not the actual **ptr
void allocate_2D_array_memory(block_node ***block_node_ptrs, int *rows, int cols, int block_count);
void free_linked_list_memory(block_node *head);
void free_2D_array_memory(block_node ***block_node_ptrs, int rows);
void write_bc_data_to_csv(block_node **block_node_ptrs, int rows, int cols, char *filename);
block_node* read_csv(block_node* head_ptr);
bool fileExists(char *fileName);
void show_menu(void);
int clear_buffer(void);
int get_int(const char *prompt);
double get_float(const char *prompt);
void print_block(block_node *block);
void quit(void);