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
    block_node **block_node_ptrs = NULL; // a pointer to the array of block_nodes, I think
    int rows = 0;
    int cols = 10; // so 10 b_n's can fit
    int block_count = 0;

    while (true)
    {
        int choice = get_int("\nWhat would you like to do? (Choose an option): ");
        enum {ADD = 1, DISPLAY, MENU, SAVE_AND_QUIT, READ_FILE, QUIT};
        
        switch (choice)
        { 
            case ADD:
                time(&t);
                strcpy(timestamp, ctime(&t)); // Get the current time

                char* new_line_ptr = strchr(timestamp, '\n'); // Get rid of \n in the timestamp
                if (new_line_ptr)
                    *new_line_ptr = '\0';

                if (head_ptr == NULL) // If the list is empty, make the genesis block:
                {
                    hash_info_needed newHash;
                    strcpy(newHash.timestamp, timestamp);

                    newHash.data = data; // A genesis block has an amount of 0.

                    newHash.previous_hash = previous_hash; // Previous hash pointed to by a genesis block is NULL
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
                head_ptr = read_csv(head_ptr);
                block_node *current = head_ptr;
                while(current->next != NULL) // Move to the end of the list to link the next block added by user
                {
                    current = current->next;
                }
                strcpy(previous_hash, current->hash);
                block_count = current->block_number;

                printf("Blockchain sucessfully imported from 'cee_blockchain_record.csv'.\n");
                break;
            case QUIT:
                printf("Thanks, see you next time!\n");
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

block_node* add_block(block_node *head, int block_number, char new_timestamp[TIMESTAMP_SIZE], 
                      double new_data, char new_hash[HASH_SIZE], char new_previous_hash[HASH_SIZE])
{
    // Create new block
    block_node *new_block = malloc(sizeof(block_node));
    if (new_block)
    {
        strcpy(new_block->timestamp, new_timestamp);

        new_block->block_number = block_number;

        new_block->data = new_data;

        strcpy(new_block->hash, new_hash);
        strcpy(new_block->previous_hash, new_previous_hash);

        new_block->next = NULL;
        // If linked list is empty, the new block is the genesis block.
        if (head == NULL)
        {
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
    printf("*               4: Save and Quit               *\n");
    printf("*               5: Load a '.csv' file          *\n");
    printf("*               6: Quit                        *\n");
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
    
    // Move through the chain and print each block's info
    while (current) {
        printf("Block %d, Timestamp: %s, Amount (in Cee): %.2lf, Hash: %s, Previous Block's Hash: %s\n",
               current->block_number, current->timestamp, current->data, current->hash, current->previous_hash);
        current = current->next;
    }
    return;
}

void write_bc_data_to_csv(block_node **block_node_ptrs, int rows, int cols, char *filename) {
    FILE *blockchain_file = fopen(filename, "w");

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
                // fprintf(blockchain_file, "%d,", block_number);
                fprintf(blockchain_file, "%d,%s,%.2f,%s,%s,\n", 
                block_number, block_node_ptrs[i][j].timestamp, block_node_ptrs[i][j].data, 
                block_node_ptrs[i][j].previous_hash, block_node_ptrs[i][j].hash);
            }
        }
    }

    fclose(blockchain_file);
}

block_node* read_csv(block_node* head_ptr) {
    FILE* blockchain_file = fopen("cee_blockchain_record.csv", "r");

    if (!blockchain_file) {
        printf("Error reading file.\n");
        return head_ptr;
    }

    char csv_contents[1024]; // Where the .csv contents are stored

    // Info needed to add new blocks from the file
    int block_number;
    char timestamp[TIMESTAMP_SIZE];
    double data;
    char hash[HASH_SIZE];
    char previous_hash[HASH_SIZE];

    while(fgets(csv_contents, 1024, blockchain_file))
    {
        int read = 0; // Number of elements read from the file

        // Read through each line of the file, and pull out necessary block info
        read = sscanf(csv_contents,
                      "%d,%64[^,],%lf,%255[^,],%255[^,]\n",
                        &block_number, timestamp, &data, previous_hash, hash);
        
        // TESTING CODE:
        // printf("\nRead: %d\n", read);
        // printf("Block num: %d\nTimestamp: %s\nData: %lf\nPrevious Hash: %s\nHash: %s",
        //         block_number, timestamp, data, previous_hash, hash); 

        if (read == 5) // If 5 elements were successfully read, create the block using that info
        {   
            head_ptr = add_block(head_ptr, block_number, timestamp, data, hash, previous_hash);
        }

        if (read != 5 && !feof(blockchain_file)) // If there were not 5 elements read, format is wrong
        {
            printf("File format incorrect.\n");
            return head_ptr;
        }

        if (ferror(blockchain_file)) // Error handling
        {
            printf("Error reading file.\n");
            return head_ptr;
        }
    }

    return head_ptr;
}

void quit(void)
{
    exit(0);
}