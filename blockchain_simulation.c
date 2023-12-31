/*blockchain_simulation.c
Authors: Jacob Martin and Aliza Camero 
Date: 12/5/2023
Purpose of the blockchain_simulation.c file: The main driver file for our blockchain simulation program.
Prompts a user to choose between several options: Add block, display blockchain, show menu, save blockchain to 
csv file and quit, read csv file, and quit program. Once the user decides what they would like to do, the 
program allows them to simulate creating a blockchain and output it to a csv file. Once a blockchain is started, 
a user is not intended to be able to change it in order to simulated the immutability of a real blockchain.
*/

#include "blockchain_simulation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <bcrypt.h>
#include <stdbool.h>
#include <wchar.h>
#include <math.h>

int main() {

    char timestamp[TIMESTAMP_SIZE];
    double data;
    char temp_hash[HASH_SIZE] = "";
    char previous_hash[HASH_SIZE] = " ";
    time_t t;

    printf("Welcome to the Blockchain Simulator!\n\n");
    show_menu();

    block_node *head_ptr = NULL;
    block_node **block_node_ptrs = NULL; // a pointer to the array of block_nodes
    int rows = 0;
    int cols = 10; 
    int block_count = 0;
    bool csv_loaded = false;

    while (true)
    {
        int choice = get_int("\nWhat would you like to do? (Choose an option): ");
        enum {ADD = 1, DISPLAY, MENU, SAVE_AND_QUIT, READ_FILE, QUIT};
        
        switch (choice)
        { 
            case ADD:

                if (fileExists("cee_blockchain_record.csv") && csv_loaded == false) {
                    printf("Error: 'cee_blockchain_record.csv' found. "
                           "Please load data from the CSV file before adding a block.\n");
                    break;
                }

                time(&t);
                strcpy(timestamp, ctime(&t)); // Get the current time

                char* new_line_ptr = strchr(timestamp, '\n'); // Get rid of \n in the timestamp
                if (new_line_ptr)
                    *new_line_ptr = '\0';

                if (head_ptr == NULL) // If the list is empty, make the genesis block
                {
                    hash_info_needed newHash;
                    strcpy(newHash.timestamp, timestamp);

                    newHash.data = data; // A genesis block has an amount of 0.

                    newHash.previous_hash = previous_hash; 
                    strcpy(temp_hash, make_hash(&newHash)); // Generate the block's Hash
                    
                    block_count++;
                    head_ptr = add_block(head_ptr, block_count, timestamp, 0, temp_hash, previous_hash); // Make the block
                    strcpy(previous_hash, temp_hash);
                }

                data = get_float("Enter amount of Cee: ");

                hash_info_needed newHash; // Gather the data needed to make the Hash
                strcpy(newHash.timestamp, timestamp);
                newHash.data = data;
                newHash.previous_hash = previous_hash;

                strcpy(temp_hash, make_hash(&newHash)); // Generate the block's Hash
                block_count++; 
                head_ptr = add_block(head_ptr, block_count, timestamp, data, temp_hash, previous_hash);
                strcpy(previous_hash, temp_hash);       
                
                printf("Block successfully created and added to the blockchain!\n");    
                break;     
            case DISPLAY:
                if (head_ptr != NULL) {
                    print_block(head_ptr);
                } else {
                    printf("No data to display. Please add blocks or load data from a .csv file.\n");
                }
                break;   
            case MENU:
                show_menu();
                break;
            case SAVE_AND_QUIT:
                allocate_2D_array_memory(&block_node_ptrs, &rows, cols, block_count);
                propagate_to_2D_array(block_node_ptrs, head_ptr, rows, cols);

                write_bc_data_to_csv(block_node_ptrs, rows, cols, "cee_blockchain_record.csv");
                
                free_linked_list_memory(head_ptr);
                free_2D_array_memory(&block_node_ptrs, rows);
                printf("Blockchain saved to 'cee_blockchain_record.csv'. See you next time!\n");
                quit();
                break;
            case READ_FILE:
                if (!fileExists("cee_blockchain_record.csv")) {
                    printf("Error: 'cee_blockchain_record.csv' not found in the project directory. "
                            "Please add a block or save block information.\n");
                    break;
                }

                head_ptr = read_csv(head_ptr);
                block_node *current = head_ptr;
                while(current->next != NULL) // Move to the end of the list to link the next block added by user
                {
                    current = current->next;
                }
                strcpy(previous_hash, current->hash);
                block_count = current->block_number;

                csv_loaded = true;
                printf("Blockchain sucessfully imported from 'cee_blockchain_record.csv'.\n");
                break;
            case QUIT:
                printf("Program exited.\n");
                quit();
                break;
        }
    }    

    return 0;
}