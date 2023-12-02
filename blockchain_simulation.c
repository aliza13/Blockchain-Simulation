#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <bcrypt.h>
#include <stdbool.h>
#include <wchar.h>
#include <math.h>

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0) // returns true if >= 0
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L) // error code
#define HASH_SIZE 255
#define TIMESTAMP_SIZE 64
#define COLS 10

typedef struct hash_info_needed {
    char timestamp[TIMESTAMP_SIZE];
    double data;
    char *previous_hash;
} hash_info_needed;

typedef struct block_node {
    char timestamp[TIMESTAMP_SIZE];
    double data; // Cee is currency, data is amount
    char hash[HASH_SIZE];
    char previous_hash[HASH_SIZE];
    struct block_node *next;
} block_node;

char* make_hash(hash_info_needed *block);
block_node* add_block(block_node *head, char new_timestamp[TIMESTAMP_SIZE], double new_data, char new_hash[HASH_SIZE], char new_previous_hash[HASH_SIZE]);
void propagate_to_2D_array(block_node **block_node_ptrs, block_node *head_ptr, int rows, int cols);
// TRIPLE pointer...... to modify the value **bnptr is pting to, not the actual **ptr
void allocate_2D_array_memory(block_node ***block_node_ptrs, int *rows, int cols, int block_count);
void free_linked_list_memory(block_node *head);
void free_2D_array_memory(block_node ***block_node_ptrs, int rows);
void write_bc_data_to_csv(block_node **block_node_ptrs, int rows, int cols, char *filename);
void show_menu(void);
int clear_buffer(void);
int get_int(const char *prompt);
double get_float(const char *prompt);
void print_block(block_node *block);
void quit(void);

int main() {

    char timestamp[TIMESTAMP_SIZE];
    double data;
    char temp_hash[HASH_SIZE] = "";
    char previous_hash[HASH_SIZE] = "";
    time_t t;

    printf("Welcome to the Blockchain Simulator!\n\n");
    show_menu();

    block_node *head_ptr = NULL;
    block_node **block_node_ptrs = NULL; // a pointer to the array of block_nodes, I think
    int rows = 0;
    int cols = 10; // so 10 b_n's can fit
    int block_count = 1; // gen block automatically gets made 

    // Make the genesis block:
    time(&t);
    strcpy(timestamp, ctime(&t)); // get time and store it in ts variable

    hash_info_needed newHash;
    strcpy(newHash.timestamp, timestamp);
    newHash.data = data; 
    newHash.previous_hash = previous_hash; // should be NULL ? or empty string
    strcpy(temp_hash, make_hash(&newHash));
    // hash for gen is made, now let the head point to gen block (1)
    head_ptr = add_block(head_ptr, timestamp, 0, temp_hash, previous_hash);
    strcpy(previous_hash, temp_hash); 

    while (true)
    {
        int choice = get_int("\nWhat would you like to do? (Choose an option): ");
        enum {ADD = 1, DISPLAY, MENU, FINALIZE_THE_BLOCKS, QUIT};
        
        switch (choice)
        { 
            case ADD:
                time(&t);
                strcpy(timestamp, ctime(&t));
                // printf("%s", timestamp);
                data = get_float("Enter amount of Cee: ");
                // printf("Data: %.2f", data);
                hash_info_needed newHash;
                strcpy(newHash.timestamp, timestamp);
                newHash.data = data;
                newHash.previous_hash = previous_hash;

                strcpy(temp_hash, make_hash(&newHash));
                //printf("Previous Hash: %s", previous_hash);

                head_ptr = add_block(head_ptr, timestamp, data, temp_hash, previous_hash);
                strcpy(previous_hash, temp_hash);       
                block_count++; 
                print_block(head_ptr);      
                break;     
            case DISPLAY:
                print_block(head_ptr);
                break;   
            case MENU:
                show_menu();
                break;
            case FINALIZE_THE_BLOCKS:
                allocate_2D_array_memory(&block_node_ptrs, &rows, cols, block_count);
                propagate_to_2D_array(block_node_ptrs, head_ptr, rows, cols);

                write_bc_data_to_csv(block_node_ptrs, rows, cols, "cee_blockchain_record.csv");
                
                free_linked_list_memory(head_ptr);
                free_2D_array_memory(&block_node_ptrs, rows);
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

block_node* add_block(block_node *head, char new_timestamp[TIMESTAMP_SIZE], double new_data, char new_hash[HASH_SIZE], char new_previous_hash[HASH_SIZE])
{
    // Create new block
    block_node *new_block = malloc(sizeof(block_node));
    if (new_block)
    {
        strcpy(new_block->timestamp, new_timestamp);
        // printf("\nBlock Time: %s", new_block->timestamp);

        // printf("\nNew Data: %f", new_data);
        new_block->data = new_data;
        // printf("\nBlock Data: %f", new_block->data);

        strcpy(new_block->hash, new_hash);

        // printf("\nNew Prev Hash: %s", new_previous_hash);
        strcpy(new_block->previous_hash, new_previous_hash);
        // printf("\nBlock Prev Hash: %s", new_block->previous_hash);

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

void propagate_to_2D_array(block_node **block_node_ptrs, block_node *head_ptr, int rows, int cols) {
    int block_count = 0;
    block_node *current_ptr = head_ptr;

    while (current_ptr != NULL && block_count < rows * cols) {
        int row = block_count / cols; // to get position in 2D array
        int col = block_count % cols;

        if (current_ptr != NULL) { // copy values from current block node to 2D array
            strcpy(block_node_ptrs[row][col].timestamp, current_ptr->timestamp);
            block_node_ptrs[row][col].data = current_ptr->data;
            strcpy(block_node_ptrs[row][col].previous_hash, current_ptr->previous_hash);
            strcpy(block_node_ptrs[row][col].hash, current_ptr->hash);
        } else {
            printf("Error propagating values in table.\n");
        }

        current_ptr = current_ptr->next;
        block_count++;
    }
}

void allocate_2D_array_memory(block_node ***block_node_ptrs, int *rows, int cols, int block_count) {
    int new_amount_of_rows = ceil((double)block_count / cols); // calc how many rows are needed and then round up

    *block_node_ptrs = realloc(*block_node_ptrs, sizeof(block_node *) * new_amount_of_rows); // for ptrs array, use realloc or malloc ?

    for (int i = *rows; i < new_amount_of_rows; i++) { // to loop through each index of ptrs to b_n's and allocate memory
        (*block_node_ptrs)[i] = malloc(sizeof(block_node) * cols);
    }

    *rows = new_amount_of_rows;
}

void free_linked_list_memory(block_node *head) {
    block_node *current_ptr = head;

    while (current_ptr != NULL) {
        block_node *temp_ptr = current_ptr;
        current_ptr = current_ptr->next; // move ptr to next
        free(temp_ptr);
    }
}

void free_2D_array_memory(block_node ***block_node_ptrs, int rows) {
    // triple pointer points to the 2D array of b_n (double pointer already)
    for (int i = 0; i < rows; i++) {
        free((*block_node_ptrs)[i]); // free each row
    }

    free(*block_node_ptrs); // free array of row ptrs
}

void show_menu(void)
{
    printf("************ Blockchain  Simulation ************\n");
    printf("*               1: Add Block                   *\n");
    printf("*               2: Display Block Info          *\n");
    printf("*               3: Show Menu                   *\n");
    printf("*               4: Finalize Added Blocks       *\n");
    printf("*               5: Quit                        *\n");
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

void write_bc_data_to_csv(block_node **block_node_ptrs, int rows, int cols, char *filename) {
    FILE *blockchain_file = fopen(filename, "a");

    if (!blockchain_file) {
        printf("Error saving to file.");
        return;
    }

    int block_number = 0;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            block_number++;

            if ((strcmp(block_node_ptrs[i][j].hash, "") == 0) && j != 0) {
                break;  
            }

            else if (block_node_ptrs[i][j].data != 0.00 || i == 0) {
                fprintf(blockchain_file, "%d,", block_number);
                fprintf(blockchain_file, "%s,%.2f,%s,%s\n", block_node_ptrs[i][j].timestamp, block_node_ptrs[i][j].data, block_node_ptrs[i][j].previous_hash, block_node_ptrs[i][j].hash);
            }
        }
    }

    fclose(blockchain_file);
}

block_node* read_file(block_node* head_ptr/*HEAD POINTER?*/)
{
    FILE *input_file;

    // We might only want to get last node in the csv to have a starting point to continue the chain

    // Open the file
    input_file = fopen("cee_blockchain_record.csv", "r");

    // If file doesn't exist
    if (!input_file)
    {
        printf("Error reading file.");
        return head_ptr;
    }

    // Get the needed data from the csv

    // Option 1: Get only the last block to continue the chain 

    /*
        **NOT REALLY SURE HOW TO DO THIS ONE...? Maybe you have a better idea of this than i do    
    */

    // Option 2: Load the entirety of the chain back into the program to continue making the chain
    /*
        // Make Placeholder vars to hold the info
        char timestamp[TIMESTAMP_SIZE];
        double data;
        char hash[HASH_SIZE];
        char previous_hash[HASH_SIZE];

        //Store file contents in a variable
        char text[SOME BUFFER SIZE BIG ENOUGH TO HOLD ALL THE DATA];

        while(fgets(text, BUFFER_SIZE, input_file))
        {
            // Iterate through file contents

            // Scan 4 elements per line and stop at commas (csv should help with this)
            if (sscanf(text, "%s, %lf, %s, %s", timestamp, &data, hash, previous_hash) == 4)
            {
                // Create a new block with the read info
                head_ptr = add_block(head, timestamp, data, hash, previous_hash);
            }
            else
            {
                printf("Error: Incorrect format in line '%s'\n", text)
                fclose(input_file);
                return head_ptr;
            }

            fclose(input_file);

            printf("Blockchain simulation record successfully loaded.");
            return head_ptr;
        }
    
    */
}

void quit(void)
{
    exit(0);
}