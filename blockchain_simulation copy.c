/* This program */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <bcrypt.h>

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0) // returns true if >= 0
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L) // error code

typedef struct hash_info_needed {
    time_t timestamp;
    float data;
    char *previous_hash;
} hash_info_needed;

typedef struct block_node {
    time_t timestamp;
    float data; // Cee is currency, data is amount
    char *previous_hash;
    struct block_node *next;
} block_node;

// void make_hash(block_node *block);
char* make_hash(hash_info_needed *block);

void print_block(const block_node *block) {
    printf("Timestamp: %ld\n", block->timestamp);
    printf("Amount: %.2f\n", block->data);
    printf("Previous Hash: %s\n", block->previous_hash);
}

int main() {

    // set genesis block to NULL values besides .next to point to 1st node

    // sample block for now
    hash_info_needed myBlock;
    myBlock.timestamp = 12345678910;
    myBlock.data = 5.2;
    myBlock.previous_hash = NULL;
    
    // create the hash and get it as a hexadecimal stringS
    char* hexHash = make_hash(&myBlock);

    // print hash to term
    if (hexHash) {
        printf("Hash: %s\n", hexHash);
        HeapFree(GetProcessHeap(), 0, hexHash); // Free the memory allocated for the hexadecimal string
    }
    
    return 0;
}

char* make_hash(hash_info_needed *block) {
    // takes in block_node ptr and returns hash as a hex string
    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_HASH_HANDLE hHash = NULL;
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    DWORD cbData = 0, cbHashObject = 0, cbHash = 0;
    PBYTE pbHashObject = NULL;
    PBYTE pbHash = NULL;
    char* hexHash = NULL;

    // open an algorithm handle
    if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
            &hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0))) {
        wprintf(L"**** Error 0x%x returned by BCryptOpenAlgorithmProvider\n", status);
        goto Cleanup;
    }

    // Calculate the size of the buffer to hold the hash object
    if (!NT_SUCCESS(status = BCryptGetProperty(
            hAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&cbHashObject, sizeof(DWORD), &cbData, 0))) {
        wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
        goto Cleanup;
    }

    //wprintf(L"Size of Hash Object Buffer: %lu bytes\n", cbHashObject);

    // Allocate the hash object on the heap
    pbHashObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHashObject);
    if (NULL == pbHashObject) {
        wprintf(L"**** memory allocation failed\n");
        goto Cleanup;
    }

    // Calculate the length of the hash
    if (!NT_SUCCESS(status = BCryptGetProperty(
            hAlg, BCRYPT_HASH_LENGTH, (PBYTE)&cbHash, sizeof(DWORD), &cbData, 0))) {
        wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
        goto Cleanup;
    }

    // Allocate the hash buffer on the heap
    pbHash = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHash);
    if (NULL == pbHash) {
        wprintf(L"**** memory allocation failed\n");
        goto Cleanup;
    }

    // Create a hash
    if (!NT_SUCCESS(status = BCryptCreateHash(
            hAlg, &hHash, pbHashObject, cbHashObject, NULL, 0, 0))) {
        wprintf(L"**** Error 0x%x returned by BCryptCreateHash\n", status);
        goto Cleanup;
    }

    // Hash some data (using block_num and data from the block)
    if (!NT_SUCCESS(status = BCryptHashData(
            hHash, (PBYTE)block, sizeof(hash_info_needed), 0))) {
        wprintf(L"**** Error 0x%x returned by BCryptHashData\n", status);
        goto Cleanup;
    }

    // Get the hash value
    if (!NT_SUCCESS(status = BCryptFinishHash(
            hHash, pbHash, cbHash, 0))) {
        wprintf(L"**** Error 0x%x returned by BCryptFinishHash\n", status);
        goto Cleanup;
    }

    // Convert the binary hash to a hexadecimal string
    hexHash = (char*)HeapAlloc(GetProcessHeap(), 0, cbHash * 2 + 1);
    if (hexHash == NULL) {
        wprintf(L"**** memory allocation failed\n");
        goto Cleanup;
    }

    for (DWORD i = 0; i < cbHash; i++) {
        sprintf(&hexHash[i * 2], "%02x", pbHash[i]);
    }

Cleanup:

    if (hAlg) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
    }

    if (hHash) {
        BCryptDestroyHash(hHash);
    }

    if (pbHashObject) {
        HeapFree(GetProcessHeap(), 0, pbHashObject);
    }

    if (pbHash) {
        HeapFree(GetProcessHeap(), 0, pbHash);
    }

    return hexHash;
}

// add_block(block_node *head, time_t new_timestamp, float new_data, char *new_previous_hash, char new_hash[255])
// {
//     // Create new block
//     block_node *new_block = malloc(sizeof(block_node));
//     if (new_block)
//     {
//         /*Assign the block it's values*/
//         new_block->timestamp = new_timestamp;
//         new_block->data = new_data;
//         // PREVIOUS HASH FIELD NEEDED
//         // HASH FIELD NEEDED
//         new_block->next = NULL;
//         // If linked list is empty, the new block is the genesis block.
//         if (head == NULL)
//         {
//             //new_block->previous_hash = NULL;
//             return new_block;
//         }
//         // Otherwise, iterate through the chain and add the block at the end.
//         else
//         {
//             block_node *current = head;
//             while (current->next != NULL)
//             {
//                 current = current->next;
//             }
//             current->next = new_block;
//             return head;
//         }
//     }
//     else
//     {
//         return NULL;
//     }
// }
