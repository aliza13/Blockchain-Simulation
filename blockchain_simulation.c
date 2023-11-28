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


