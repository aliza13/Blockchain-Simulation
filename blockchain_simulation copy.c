/* This program */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <bcrypt.h>
#include <stdbool.h>

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0) // returns true if >= 0
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L) // error code

typedef struct hash_info_needed {
    char timestamp[50];
    double data;
    char *previous_hash;
} hash_info_needed;

typedef struct block_node {
    char timestamp[50];
    double data; // Cee is currency, data is amount
    char hash[255];
    char previous_hash[255];
    struct block_node *next;
} block_node;

char* make_hash(hash_info_needed *block);
block_node* add_block(block_node *head, char new_timestamp[50], double new_data, char new_hash[255], char new_previous_hash[255]);
void show_menu(void);
int clear_buffer(void);
int get_int(const char *prompt);
double get_float(const char *prompt);
void print_block(block_node *block);
void quit(void);

void store_in_blockchain(block_node Cee_Blockchain[][], int block_count, char new_timestamp[50], double new_data, char new_hash[255], char new_previous_hash[255]) {
    strcpy(Cee_Blockchain[block_count][0].timestamp, new_timestamp);
    Cee_Blockchain[block_count][0].data = new_data;
    // new hash &n prev hash
}

int main() {
    printf("Welcome to the Blockchain Simulator!\n\n");
    show_menu();

    char previous_hash[255] = "";
    double data;
    char timestamp;
    time_t t;
    char current_time[50];
    char temp_hash[255] = "";

    block_node *head_ptr = NULL;
    // ea row is ONE block
    block_node Cee_Blockchain[1000][1]; // ea element in 2D is type b_n
    int block_count = 0; 
    // Make the genesis block:
    time(&t);
    strcpy(current_time, ctime(&t));
    hash_info_needed newHash;
    strcpy(newHash.timestamp, current_time);
    
    newHash.data = data;
    newHash.previous_hash = previous_hash;

    strcpy(temp_hash, make_hash(&newHash));
    head_ptr = add_block(head_ptr, current_time, 0, temp_hash, previous_hash);

    strcpy(previous_hash, temp_hash); 

    while (true)
    {
        int choice = get_int("\nWhat would you like to do? (Choose an option): ");
        enum {ADD = 1, DISPLAY, MENU, QUIT};
        
        switch (choice)
        { 
            case ADD:
                time(&t);
                strcpy(current_time, ctime(&t));

                // printf("%s", current_time);
                data = get_float("Enter amount of Cee: ");
                // printf("Enter data: ");
                // scanf(" %f", &data);
                // clear_buffer();
                // printf("Data: %.2f", data);

                hash_info_needed newHash;
                strcpy(newHash.timestamp, current_time);
                
                newHash.data = data;
                newHash.previous_hash = previous_hash;

                strcpy(temp_hash, make_hash(&newHash));

                //printf("Previous Hash: %s", previous_hash);

                head_ptr = add_block(head_ptr, current_time, data, temp_hash, previous_hash);
                // store_in_blockchain(Cee_Blockchain, block_count, current_time, data, temp_hash, previous_hash);
                block_count++;
                strcpy(previous_hash, temp_hash);              
                break;     
            case DISPLAY:
                print_block(head_ptr);
                break;   
            case MENU:
                show_menu();
                break;
            case QUIT:
                printf("Thanks, see you next time!");
                quit();
                break;
        }
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

block_node* add_block(block_node *head, char new_timestamp[50], double new_data, char new_hash[255], char new_previous_hash[255])
{
    // Create new block
    block_node *new_block = malloc(sizeof(block_node));
    if (new_block)
    {
        /*Assign the block it's values*/
        
        //printf("\nNew Timestamp: %s", new_timestamp);
        strcpy(new_block->timestamp, new_timestamp);
        //printf("\nBlock Time: %s", new_block->timestamp);

        //printf("\nNew Data: %f", new_data);
        new_block->data = new_data;
        //printf("\nBlock Data: %f", new_block->data);

        strcpy(new_block->hash, new_hash);

        //printf("\nNew Prev Hash: %s", new_previous_hash);
        strcpy(new_block->previous_hash, new_previous_hash);
        //printf("\nBlock Prev Hash: %s", new_block->previous_hash);

        new_block->next = NULL;
        // If linked list is empty, the new block is the genesis block.
        if (head == NULL)
        {
            //new_block->previous_hash = NULL;
            return new_block;
        }
        //Otherwise, iterate through the chain and add the block at the end.
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

void show_menu(void)
{
    printf("************ Blockchain  Simulation ************\n");
    printf("*               1: Add Block                   *\n");
    printf("*               2: Display Block Info          *\n");
    printf("*               3: Show Menu                   *\n");
    printf("*               4: Quit                        *\n");
    printf("************************************************\n");
}

int clear_buffer(void)
{
    int ch;
    int count = 0;
    while((ch = getchar()) != '\n' && ch != EOF)
        ++count;
    
    return count;
}

int get_int(const char *prompt)
{
    int num;
    int ret;
    int count;

    do
    {
        printf("%s", prompt);
        ret = scanf("%d", &num);

        /* Clear buffer */
        count = clear_buffer();

        /**
         * If scanf() did not successfully read one integer
         * or, if clear_buffer() reads at least one non-white character.
         */
        if (ret != 1 || count > 0)
            printf("Invalid input! Please enter just one integer from 1 to 6.\n");

    } while(ret != 1 || count != 0);

    return num;
}

double get_float(const char *prompt)
{
    double num;
    int ret;
    int count;

    do
    {
        printf("%s", prompt);
        ret = scanf("%lf", &num);

        /* Clear buffer */
        count = clear_buffer();

        /**
         * If scanf() did not successfully read one integer
         * or, if clear_buffer() reads at least one non-white character.
         */
        if (ret != 1 || count > 0)
            printf("Invalid input! Please enter just one integer from 1 to 6.\n");

    } while(ret != 1 || count != 0);

    return num;
}

void print_block(block_node *block) {
    
    block_node *current = block;

    if (!current)
        puts("There are no blocks in the chain.");
    
    while (current != NULL)
    {
        printf("\nTimestamp:\t%s\nAmount:\t%f\nHash:\t%s\nPrevious Hash:\t%s\n", current->timestamp, current->data, current->hash, current->previous_hash);
        current = current->next;
    }
    return;
}

void quit(void)
{
    exit(0);
}