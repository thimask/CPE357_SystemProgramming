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
#include <strings.h>

#define MATRIX_DIMENSION_XY 10
//************************************************************************************************************************
// sets one element of the matrix
void set_matrix_elem(float *M,int x,int y,float f)
{
    M[x + y*MATRIX_DIMENSION_XY] = f;
}
//************************************************************************************************************************
// checks to see if both matrices are the same
int quadratic_matrix_compare(float *A,float *B)
{
    for(int a = 0;a<MATRIX_DIMENSION_XY;a++)
        for(int b = 0;b<MATRIX_DIMENSION_XY;b++)
        if(A[a +b * MATRIX_DIMENSION_XY]!=B[a +b * MATRIX_DIMENSION_XY]) 
            return 0;
    
    return 1;
}
//************************************************************************************************************************
//print a matrix
void quadratic_matrix_print(float *C)
{
    printf("\n");
for(int a = 0;a<MATRIX_DIMENSION_XY;a++)
    {
    printf("\n");
    for(int b = 0;b<MATRIX_DIMENSION_XY;b++)
        printf("%.2f,",C[a + b* MATRIX_DIMENSION_XY]);
        fflush(0);
    }
printf("\n");
}
//************************************************************************************************************************
// multiply two matrices
void quadratic_matrix_multiplication(float *A,float *B,float *C)
{
	//nullify the result matrix first
for(int a = 0;a<MATRIX_DIMENSION_XY;a++)
    for(int b = 0;b<MATRIX_DIMENSION_XY;b++)
        C[a + b*MATRIX_DIMENSION_XY] = 0.0;
//multiply
for(int a = 0;a<MATRIX_DIMENSION_XY;a++) // over all cols a
    for(int b = 0;b<MATRIX_DIMENSION_XY;b++) // over all rows b
        for(int c = 0;c<MATRIX_DIMENSION_XY;c++) // over all rows/cols left
            {
                C[a + b*MATRIX_DIMENSION_XY] += A[c + b*MATRIX_DIMENSION_XY] * B[a + c*MATRIX_DIMENSION_XY]; 
            }
}
//************************************************************************************************************************
// multiply two matrices
void quadratic_matrix_multiplication_split(float *A,float *B,float *C, int par_id, int par_count)
{
    int work = MATRIX_DIMENSION_XY/par_count;
    int start = par_id*work;
    int end = start + work;
    if (par_id == par_count - 1)
    {
        end = MATRIX_DIMENSION_XY;
    }

    //nullify the result matrix first
    for(int a = 0;a<MATRIX_DIMENSION_XY;a++)
        for(int b = start;b<end;b++)
            C[a + b*MATRIX_DIMENSION_XY] = 0.0;
    //multiply
    for(int a = 0;a<MATRIX_DIMENSION_XY;a++) // over all cols a
        for(int b = start;b<end;b++) // over all rows b
            for(int c = 0;c<MATRIX_DIMENSION_XY;c++) // over all rows/cols left
                {
                    C[a + b*MATRIX_DIMENSION_XY] += A[c + b*MATRIX_DIMENSION_XY] * B[a + c*MATRIX_DIMENSION_XY]; 
                }
}
//************************************************************************************************************************

//synch algorithm. make sure, ALL processes get stuck here until all ARE here

void synch(int par_id,int par_count,int *ready)  //idea: every synch step waits for another value (1,2,3,4,5,..)
{
    int synchid = ready[par_count]+1; //synchid is progressing with each synch
    ready[par_id]=synchid;
    int breakout = 0;
    while(1)
        {
        breakout=1;    
        for(int i=0;i<par_count;i++) 
            {
            if(ready[i]<synchid) //"less than" because one process could run ahead and increment its ready[par_is] var already.
                {breakout = 0;break;}
            }
        if(breakout==1)
            {
            ready[par_count] = synchid; //and here we increment the additional variable
            break;
            }
        }
}
/* void synch(int par_id,int par_count,int *ready)
{
    ready[par_id] = 1;
    int count = 0;
    while(count < par_count)
    {
        while(ready[count] == 0);
        count++;
    }
    ready[par_id] = 2;
    if (par_id == 0)
    {
        int count = 0;
        while(count < par_count)
        {
            while(ready[count] == 1);
            count++;
        }
        for (int i = 1; i<par_count; i++)
        {
            ready[0] = 2;
            ready[i] = 0;
        }
        ready[0] = 0;
    }
    else
    {
        while (ready[0] == 2);
    }
} */
/* void synch(int par_id,int par_count,int *ready)
{
    ready[par_id] = 1;
    int count = 0;
    while(count < par_count)
    {
        while(ready[count] == 0);
        count++;
    }
    ready[par_id] = 2;
    if (par_id == 0)
    {
        int count = 0;
        while(count < par_count)
        {
            ready[0] = 2;
            while(ready[count] != 2);
            count++;
        }
        for (int i = 1; i<par_count; i++)
        {
            ready[0] = 2;
            ready[i] = 0;
        }
        ready[0] = 0;
    }
    else
    {
        while (ready[0] == 2);
    }
} */
//************************************************************************************************************************
int main(int argc, char *argv[])
    {
    int par_id = 0; // the parallel ID of this process
    int par_count = 1; // the amount of processes
    float *A,*B,*C; //matrices A,B and C
    int *ready; //needed for synch
    if(argc!=3){printf("no shared\n");}
    else
        {
        par_id= atoi(argv[1]);
        par_count= atoi(argv[2]);
    // strcpy(shared_mem_matrix,argv[3]);
        }
    if(par_count==1){printf("only one process\n");}

    int fd[4];
    if(par_id==0)
    {
        //init the shared memory for A,B,C, ready. shm_open with C_CREAT here! then ftruncate! then mmap
        fd[0] = shm_open("matrixA", O_RDWR | O_CREAT, 0777);
        fd[1] = shm_open("matrixB", O_RDWR | O_CREAT, 0777);
        fd[2] = shm_open("matrixC", O_RDWR | O_CREAT, 0777);
        fd[3] = shm_open("synchobject", O_RDWR | O_CREAT, 0777);
        ftruncate(fd[0], sizeof(float)*100);
        ftruncate(fd[1], sizeof(float)*100);
        ftruncate(fd[2], sizeof(float)*100);
        ftruncate(fd[3], sizeof(float)*100);
        A = (float*)mmap(0, sizeof(float)*100, PROT_READ|PROT_WRITE, MAP_SHARED, fd[0], 0);
        B = (float*)mmap(0, sizeof(float)*100, PROT_READ|PROT_WRITE, MAP_SHARED, fd[1], 0);
        C = (float*)mmap(0, sizeof(float)*100, PROT_READ|PROT_WRITE, MAP_SHARED, fd[2], 0);
        ready = (int*)mmap(0, sizeof(int)*(par_count+1), PROT_READ|PROT_WRITE, MAP_SHARED, fd[3], 0);
        for (int i = 0; i <= par_count; i++)
        {
            ready[i] = 0;
        }   
    }
    else
    {
        //init the shared memory for A,B,C, ready. shm_open withOUT C_CREAT here! NO ftruncate! but yes to mmap
        sleep(2); //needed for initalizing synch
        fd[0] = shm_open("matrixA", O_RDWR, 0777);
        fd[1] = shm_open("matrixB", O_RDWR, 0777);
        fd[2] = shm_open("matrixC", O_RDWR, 0777);
        fd[3] = shm_open("synchobject", O_RDWR, 0777);
        A = (float*)mmap(0, sizeof(int)*100, PROT_READ|PROT_WRITE, MAP_SHARED, fd[0], 0);
        B = (float*)mmap(0, sizeof(int)*100, PROT_READ|PROT_WRITE, MAP_SHARED, fd[1], 0);
        C = (float*)mmap(0, sizeof(int)*100, PROT_READ|PROT_WRITE, MAP_SHARED, fd[2], 0);
        ready = (int*)mmap(0, sizeof(int)*(par_count+1), PROT_READ|PROT_WRITE, MAP_SHARED, fd[3], 0);
        ready[par_id] = 0;
    }
    synch(par_id,par_count,ready);

    clock_t stop, start;
    if(par_id==0)
    {
        //initialize the matrices A and B
        for(int x = 0; x < 100; x++)
        {
            for(int y = 0; y < 100; y++)
            {
                //set_matrix_elem(A, x, y, 1 + (rand() % 9));
                //set_matrix_elem(B, x, y, 1 + (rand() % 9));
                set_matrix_elem(A, x, y, 1);
                set_matrix_elem(B, x, y, 1);
            }
        }
        start = clock();
    }

    synch(par_id,par_count,ready);

    // quadratic_matrix_multiplication_parallel(par_id, par_count,A,B,C, ...);
    quadratic_matrix_multiplication_split(A, B, C, par_id, par_count);
        
    synch(par_id,par_count,ready);

    if(par_id==0)
    {
        stop = clock();
        double timetaken = ((double)(stop - start))/CLOCKS_PER_SEC;
        printf("\nTime taken to multiply: %f seconds", timetaken);
        quadratic_matrix_print(C);
        fflush(0);
    }
        
    synch(par_id, par_count, ready);

    close (fd[0]);
    close (fd[1]);
    close (fd[2]);
    close (fd[3]);
    shm_unlink("matrixA");
    shm_unlink("matrixB");
    shm_unlink("matrixC");
    shm_unlink("synchobject");

    //lets test the result:
    float M[MATRIX_DIMENSION_XY * MATRIX_DIMENSION_XY];
    quadratic_matrix_multiplication(A, B, M);
    if (quadratic_matrix_compare(C, M))
    {
        printf("full points!\n");
        fflush(0);
    }
    else
    {
         printf("buuug!\n");
         fflush(0);
    }
    return 0;    
}
