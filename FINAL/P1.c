#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>




int main(int argc, char * argv[])
{
if(argc< 3)
{

printf("Invalid no. of arguments\n");
}
else
{

int fd =0;
int n = atoi(argv[2]);

char *param[] = { "gcc", "-c", argv[1], 0 };
int i=0; 
int * map = (int*) mmap(NULL, getpagesize(), PROT_READ| PROT_WRITE, MAP_SHARED, fd, 0);


for(i; i< n; i++)
{
execvp(param[0], param);
}

}


}
