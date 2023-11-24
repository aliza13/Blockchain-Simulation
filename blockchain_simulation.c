/* This program */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <bcrypt.h>

typedef struct {
    float amount;
    char sender_addy[64];
    char receiver_addy[64];
} transaction_data;

typedef struct block_node {
    int block_num;
    time_t timestamp;
    transaction_data *data;
    char *previous_hash;
    char hash[65]; 
    struct block_node *next;
} block_node;

// void make_hash(block_node *block);

void print_block(const block_node *block) {
    printf("Block Number: %d\n", block->block_num);
    printf("Timestamp: %ld\n", block->timestamp);
    printf("Amount: %.2f\n", block->data->amount);
    printf("Sender Address: %s\n", block->data->sender_addy);
    printf("Receiver Address: %s\n", block->data->receiver_addy);
    printf("Previous Hash: %s\n", block->previous_hash);
    printf("Hash: %s\n", block->hash);
}


int main() {
    // sample block for now
    block_node myBlock;
    myBlock.block_num = 1;
    myBlock.timestamp = 1234567890;
    myBlock.data = malloc(sizeof(transaction_data));
    myBlock.data->amount = 100.0;
    strcpy(myBlock.data->sender_addy, "S1");
    strcpy(myBlock.data->receiver_addy, "R1");
    myBlock.previous_hash = NULL; 
    myBlock.next = NULL;
    
    return 0;
}


