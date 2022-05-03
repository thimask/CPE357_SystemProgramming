#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h> 
#include <dirent.h>
#include <libgen.h>
#include <errno.h> 
#define MATRIX_DIMENSION_XY 10

//************************************************************************************************************************
//calls program1 
int main(int argc, char *argv[])
{
    //init argument array for program1
    char*args[4];
    args[0] = malloc(100);
    args[1] = malloc(100);
    args[2] = malloc(100);
    args[3] = malloc(100);
    strcpy(args[0], argv[1]);
    strcpy(args[2], argv[2]);
    args[3] = NULL;
    
    //for loop to fork and call program1 based on the number of processes. 
    for (int i = 0; i < atoi(argv[2]); i++)
    {
        if (fork() != 0)
        {
            sprintf(args[1], "%d", i);
            execv(argv[1], args);
            int j = errno;
            return 0;
        }
        else
        {
           wait(0);
        }
    }
    return 0;    
}
