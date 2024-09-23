#include "mu.h"
#include "unistd.h"
#include "stdio.h"
#include "getopt.h" 
#include <string.h>

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


    while((opt = getopt_long(argc, argv, short_opts,long_opts,NULL))!=-1){
        switch (opt){
            case 'h':
                printf("usage : \nfedit [-h] [-l NROTL] [-r NROTR] [-x NEXPAND]"
                "[-c NCONTRACT] [-v CHAR] [-s NSKIP] [-k NKEEP] [-r] PATH\n");
                break;
            case 'c':{
                if(expand_flag==1){
                    printf("not allowed to contract and expand at same time");
                    return 1;
                }
                printf("Hello World\n");
                contract_flag=1;
                break;
            }
            case 'x':{
                if(contract_flag==1){
                    printf("We are getting HERE GUYS");
                    fprintf(stderr,"not allowed to contract and expand at the same time");
                    return 1;
                }
                printf("Hello SIr\n");
                expand_flag=1;
                break;
            }
        }
    }
    
    return 0;
}
