/* This program */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <bcrypt.h>

typedef struct block_node {
    int block_num;
    time_t timestamp;
    int data; // Cee is currency, data is amount
    char *previous_hash;
    char hash[65]; 
    struct block_node *next;
} block_node;

// void make_hash(block_node *block);

void print_block(const block_node *block) {
    printf("Block Number: %d\n", block->block_num);
    printf("Timestamp: %ld\n", block->timestamp);
    printf("Amount: %.2f\n", block->data);
    printf("Previous Hash: %s\n", block->previous_hash);
    printf("Hash: %s\n", block->hash);
}


int main() {
    // sample block for now
    block_node myBlock;
    myBlock.block_num = 1;
    myBlock.timestamp = 1234567890;
    myBlock.data = 100.0;
    myBlock.previous_hash = NULL; 
    myBlock.next = NULL;
    
    return 0;
}


