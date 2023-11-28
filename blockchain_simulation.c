/* This program */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <bcrypt.h>

typedef struct block_node {
    time_t timestamp;
    float data; // Cee is currency, data is amount
    char *previous_hash;
    struct block_node *next;
} block_node;

// void make_hash(block_node *block);

void print_block(const block_node *block) {
    printf("Timestamp: %ld\n", block->timestamp);
    printf("Amount: %.2f\n", block->data);
    printf("Previous Hash: %s\n", block->previous_hash);
}


int main() {

    // set genesis block to NULL values besides .next to point to 1st node


    // sample block for now
    block_node myBlock;
    myBlock.timestamp = 1234567890;
    myBlock.data = 100.0;
    myBlock.previous_hash = NULL; 
    myBlock.next = NULL;
    
    return 0;
}

add_block(block_node *head, time_t new_timestamp, float new_data, char *new_previous_hash, char new_hash[255])
{
    // Create new block
    block_node *new_block = malloc(sizeof(block_node));
    if (new_block)
    {
        /*Assign the block it's values*/
        new_block->timestamp = new_timestamp;
        new_block->data = new_data;
        // PREVIOUS HASH FIELD NEEDED
        // HASH FIELD NEEDED
        new_block->next = NULL;
        // If linked list is empty, the new block is the genesis block.
        if (head == NULL)
        {
            //new_block->previous_hash = NULL;
            return new_block;
        }
        // Otherwise, iterate through the chain and add the block at the end.
        else
        {
            block_node *current = head;
            while (current->next != NULL)
            {
                current = current->next;
            }
            current->next = new_block;
            return head;
        }
    }
    else
    {
        return NULL;
    }
}
