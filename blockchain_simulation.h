/*blockchain_simulation.h
Authors (Name & Student ID): Jacob Martin X00526617, Aliza Camero
Date: 12/5/2023
Purpose of the blockchain_simulation.h file: The header file for our blockchain simulation program.
Houses all the function prototypes and #define macros.
*/

#pragma once

#include <stdbool.h>

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0) // returns true if >= 0
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L) // error code
#define HASH_SIZE 255
#define CSV_LINE_SIZE 1024
#define TIMESTAMP_SIZE 64
#define COLS 10

/**
 * Our own structure that holds all the necessary
 * information required to generate a unique hash:
 * timestamp, amount of data (Cee), and and the unique 
 * hash of the previous block in the chain.
*/
typedef struct hash_info_needed {
    char timestamp[TIMESTAMP_SIZE];
    double data;
    char *previous_hash;
} hash_info_needed;

/**
 * Our own structure that represents a block in the
 * blockchain. Each block has a timestamp, block number,
 * amount of data (Cee), a unique hash, and the unique hash of 
 * the previous block in the chain.
*/
typedef struct block_node {
    char timestamp[TIMESTAMP_SIZE];
    int block_number;
    double data; // Cee is currency, data is amount
    char hash[HASH_SIZE];
    char previous_hash[HASH_SIZE];
    struct block_node *next;
} block_node;

char* make_hash(hash_info_needed *block);

/**
 * Creates a new block to add to the blockchain and puts it at the end of the chain.
 * 
 * @param head A pointer to the head of the linked list.
 * @param block_number The number of the block in the chain.
 * @param new_timestamp The current time when the block was created.
 * @param new_data The amount of Cee the block will hold.
 * @param new_hash The block's uniquie hash identifier.
 * @param new_previous_hash The unique hash identifier of the previous block in the chain.
 * 
 * @return      A pointer to the head of the linked list; NULL if memory allocation fails.
*/
block_node* add_block(block_node *head, int block_number, char new_timestamp[TIMESTAMP_SIZE], 
                      double new_data, char new_hash[HASH_SIZE], char new_previous_hash[HASH_SIZE]);

void propagate_to_2D_array(block_node **block_node_ptrs, block_node *head_ptr, int rows, int cols);

void allocate_2D_array_memory(block_node ***block_node_ptrs, int *rows, int cols, int block_count);

void free_linked_list_memory(block_node *head);

void free_2D_array_memory(block_node ***block_node_ptrs, int rows);

void write_bc_data_to_csv(block_node **block_node_ptrs, int rows, int cols, char *filename);

/**
 * Opens a file named 'cee_blockchain_record.csv'. If the file is successfully
 * read, the file is parsed through line by line and each line's info is used to re-create the
 * blockchain in the linked list. If the file cannot be read, an error message is shown.
 * 
 * @param head_ptr A pointer to the head of the linked list.
 * 
 * @return         A pointer to the head of the linked list. 
*/
block_node* read_csv(block_node* head_ptr);

bool fileExists(char *fileName);

/**
 * Displays the menu for the user by printing to terminal.
*/
void show_menu(void);

/**
 * Clears the input buffer. Function taken from the one we made in class.
 * 
 * @return      An int representing the number of characters counted.
*/
int clear_buffer(void);

/**
 * Prompts the user for an integer. Function taken from the one we made in class.
 * 
 * @param prompt A pointer to the string prompting the user for input.
 * 
 * @return      A number (int) that the user input.
 * 
*/
int get_int(const char *prompt);

/**
 * Prompts the user for a double. Function is a modified version of get_int() from class.
 * 
 * @param prompt A pointer to the string prompting the user for input.
 * 
 * @return      A number (double) that the user input.
*/
double get_float(const char *prompt);

/**
 * Iterates through a linked list, printing each element's information until there are no more 
 * blocks in the chain. If there are no blocks in the chain, print an error message.
 * 
 * @param block A pointer to the head of the linked list.
*/
void print_block(block_node *block);

void quit(void);