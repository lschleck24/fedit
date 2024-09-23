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
        long new_pos = (i + file_size - rotation_bytes) % file_size; // Calculate new position
        rotated[i] = buffer[new_pos]; // Assign the byte to the new position
    }

    // Write the rotated contents back to the file
    fseek(file, 0, SEEK_SET); // Go back to the start of the file
    fwrite(rotated, 1, file_size, file); // Write the rotated bytes

    // Clean up
    free(buffer);
    free(rotated);
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
    //int rotate_amount;

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
        FILE *file = fopen(filename, "r+");
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
                printf("Hello World\n");
                contract_flag=1;
                break;
            }
            case 'x':{
                if(contract_flag==1){
                    printf("We are getting HERE GUYS\n");
                    fprintf(stderr,"not allowed to contract and expand at the same time\n");
                    exit(1);
                }
                printf("Hello SIr\n");
                expand_flag=1;
                break;
            }
            case 'k':{
                if(optind < argc && argv[optind][0] != '-'){
                    fprintf(stderr,"Missing argument\n");
                    exit(1);
                }
                break;
            }
            case 'l':{
                printf("SIRRRRR\n");
                int rotation_size = atoi(optarg);
                printf("%d\n",rotation_size);
                if(rotation_size<0){
                    fprintf(stderr,"RAAA\n");
                    exit(1);
                }
                rotate_left(file,rotation_size);
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
    return 0;
}
