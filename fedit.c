#include "mu.h"
#include "unistd.h"
#include "stdio.h"
#include "getopt.h" 
#include <string.h>
#include <stdbool.h>
void rotate_left(FILE *file, int rotation_bytes) {
    // Determine the size of the file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size == 0) return; // If the file is empty, do nothing

    // Normalize the rotation bytes
    //rotation_bytes = rotation_bytes % file_size;
    rotation_bytes = rotation_bytes % file_size;

    // Create a buffer to hold the file's contents
    unsigned char *buffer = malloc(file_size);
    if (!buffer) {
        perror("Failed to allocate memory");
        exit(1);
    }

    // Read the entire file into the buffer
    fread(buffer, 1, file_size, file);

    // Create a new buffer for the rotated contents
    unsigned char *rotated = malloc(file_size);
    if (!rotated) {
        perror("Failed to allocate memory");
        free(buffer);
        exit(1);
    }

    // Perform the rotation
    for (long i = 0; i < file_size; i++) {
        long new_pos = (i + file_size + rotation_bytes) % file_size; // Calculate new position
        rotated[i] = buffer[new_pos]; // Assign the byte to the new position
    }

    // Write the rotated contents back to the file
    fseek(file, 0, SEEK_SET); // Go back to the start of the file
    fwrite(rotated, 1, file_size, file); // Write the rotated bytes

    // Clean up
    free(buffer);
    free(rotated);
}

void expand_file(FILE *file, int expand_count, char value) {
    // Read the original content of the file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    printf("YESEJR\n");
    
    char *content = malloc(file_size + 1); // Allocate memory for file content
    fread(content, 1, file_size, file);
    content[file_size] = '\0'; // Null-terminate the string

    // Prepare new content
    int new_length = file_size + expand_count;
    char *new_content = malloc(new_length + 1);
    strcpy(new_content, content); // Copy original content
    for (int i = 0; i < expand_count; i++) {
        new_content[file_size + i] = value; // Append the expansion character
    }
    new_content[new_length] = '\0'; // Null-terminate the new content

    // Write the new content back to the file
    fseek(file, 0, SEEK_SET); // Go back to the beginning of the file
    fwrite(new_content, 1, new_length, file);
    fflush(file); // Ensure all data is written
    free(content);
    free(new_content);
}

void contract_file(FILE *file, int contract_size) {
    // Move to the end of the file
    fseek(file, 0, SEEK_END);
    long current_size = ftell(file); // Get the current size of the file

    // If the contract size is greater than or equal to the current size, truncate to 0
    if (contract_size >= current_size) {
        freopen(NULL, "w", file); // Truncate file to 0 bytes
    } else {
        // Calculate the new size after contraction
        long new_size = current_size - contract_size;

        // Use ftruncate to change the file size
        // Note: You must open the file in a mode that allows truncation
        // Move to the new size position
        ftruncate(fileno(file), new_size);
    }
}

void skip_and_keep_file(FILE *file, int skip, int keep) {
    // Move to the end of the file to find the current size
    fseek(file, 0, SEEK_END);
    long current_size = ftell(file); // Get the current size of the file

    // If the skip exceeds or matches the file size, we will truncate it
    if (skip >= current_size) {
        // Truncate the file
        freopen(NULL, "w", file); // This truncates the file
        return; // The file will be empty
    }

    // Calculate the starting position from where we need to keep bytes
    long start_position = skip;

    // If the keep exceeds the remaining bytes after skipping, adjust it
    if (keep + skip > current_size) {
        keep = current_size - skip; // Adjust keep to remaining bytes
    }

    // Create a buffer to hold the data to be kept
    char *buffer = malloc(keep);
    if (!buffer) {
        perror("Memory allocation failed");
        exit(1);
    }

    // Move to the start position to read the bytes to keep
    fseek(file, start_position, SEEK_SET);
    //fread(buffer, 1, keep, file);

    unsigned char byte;
    int byte_count = 0;
    int buffer_index = 0;
    while (fread(&byte, sizeof(unsigned char), 1, file) == 1 && skip + byte_count< current_size) {
        // Write to buffer only if the byte count is a multiple of skip
        if (skip + keep < current_size) {
            if (buffer_index < keep) {
                printf("%d: %d\n",buffer_index,byte_count);
                buffer[buffer_index++] = byte; // Store the byte in the buffer
            }
        }
        byte_count++;
    }

    // Truncate the file and write back the kept data
    freopen(NULL, "w", file); // Truncate the file
    fwrite(buffer, 1, keep, file); // Write the kept data back

    // Clean up
    free(buffer);
}

int 
main(int argc, char *argv[])
{
    /* 
     * TODO: delete the two MU_UNUSED lines below (they are just there so that
     * this file compiles without warnings) and implement the project.  You'll
     * want to parse the command-line arguments in main and create other
     * functions as needed.
     */
    int opt;
    int contract_flag = 0;
    int expand_flag = 0;
    char *filename = NULL;
    char value = 'A';
    int expand_count = 0;
    int contract_size = 0;
    int skip = 0;
    int keep = 0;
    //int rotate_amount;
    FILE *file;

    const char *short_opts = "hl:x:c:v:k:s:";
    struct option long_opts[] = {
        {"help", no_argument, NULL, 'h'},
        {"rotate-left", required_argument, NULL, 'l'},
        {"expand", required_argument, NULL, 'x'},
        {"contract", required_argument, NULL, 'c'},
        {"value", required_argument, NULL, 'v'},
        {"keep", required_argument, NULL, 'k'},
        {"skip", required_argument, NULL, 's'},
        {NULL, 0, NULL, 0}
    };

    filename = argv[argc-1];    
    optind = 1;
    while((opt = getopt_long(argc, argv, short_opts,long_opts,NULL))!=-1){
        if (opt=='h'){
            printf("usage : \nfedit [-h] [-l NROTL] [-r NROTR] [-x NEXPAND]"
            "[-c NCONTRACT] [-v CHAR] [-s NSKIP] [-k NKEEP] [-r] PATH\n");
            return 0;
        }
        file = fopen(filename, "r+");
        if (!file) {
            perror("Error opening file");
            exit(2);
        }
        switch (opt){
            case 'c':{
                if(expand_flag==1){
                    fprintf(stderr,"not allowed to contract and expand at same time\n");
                    exit(1);
                }
                contract_size = atoi(optarg);
                contract_file(file,contract_size);
                printf("Hello World\n");
                contract_flag=1;
                break;
            }
            case 'v':
                value = optarg[0];
                printf("%c\n",value);
                break;
            case 'x':{
                if(contract_flag==1){
                    printf("We are getting HERE GUYS\n");
                    fprintf(stderr,"not allowed to contract and expand at the same time\n");
                    exit(1);
                }
                expand_count = atoi(optarg);
                printf("%d\n",expand_count);
                printf("Hello SIr\n");
                expand_flag=1;
                break;
            }
            case 'k':{
                if (optarg != NULL) {
                    // Make sure the next argument is not another option
                    if (optind < argc && optarg[0] == '-') {
                        fprintf(stderr, "Missing argument for -k\n");
                        exit(1);
                    }
                    keep = atoi(optarg);
                } else {
                    fprintf(stderr, "Missing argument for -k\n");
                    exit(1);
                }
                break;
            }
            case 'l':{
                int rotation_size = atoi(optarg);
                printf("%d\n",rotation_size);
                if(rotation_size<0){
                    fprintf(stderr,"RAAA\n");
                    exit(1);
                }
                rotate_left(file,rotation_size);
                break;
            }
            case 's':{
                if (optarg != NULL) {
                    // Make sure the next argument is not another option
                    if (optind < argc && optarg[0] == '-') {
                        fprintf(stderr, "Missing argument for -s\n");
                        exit(1); // Return non-zero status
                    }
                    skip = atoi(optarg);
                } else {
                    fprintf(stderr, "Missing argument for -s\n");
                    exit(1); // Return non-zero status
                }
                break;
            }
            case '?':{
                //fprintf(stderr,"Unknown Operation");
                exit(1);
                break;
            }
            default:{
                exit(1);
            }

        }
    }
    
    if(expand_flag==1){
        expand_file(file,expand_count,value);
    }
    
    if(keep>0){
        skip_and_keep_file(file,skip,keep);
    }

    return 0;
}
