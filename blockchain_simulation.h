#pragma once

#include <time.h>

typedef struct {
    int block_num;
    time_t timestamp;
    transaction_data *data;
    char *previous_hash;
    char hash[65]; 
    struct block *next;
} block_node;

typedef struct {
    float amount;
    char sender_addy[64];
    char receiver_addy[64];
} transaction_data;

