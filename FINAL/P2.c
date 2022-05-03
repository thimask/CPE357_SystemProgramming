#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<math.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>



pthread_t tid[2];

pthread_mutex_t lock;


// A function to sort the algorithm using Odd Even sort
void oddEvenSort(int arr[], int n)

{

	int isSorted = -1; // Initially array is unsorted


	while (isSorted != 1) {
		isSorted = 1;

int window_size =4;

pthread_mutex_lock(&lock);
		// Perform Bubble sort on odd indexed element
		int i=0;
		for ( i= 1; i <= n - 2; i = i + 2) {
			if (arr[i] > arr[i + 1]) {
			int temp = arr[i];
			arr[i] = arr[i+1];
			arr[i+1] = temp;
			isSorted = -1;
			}
					
			
		}
		pthread_mutex_unlock(&lock);
		pthread_mutex_lock(&lock);

		// Perform Bubble sort on even indexed element
		for (i = 0; i <= n - 2; i = i + 2) {
			if (arr[i] > arr[i + 1]) {
			int temp = arr[i];
			arr[i] = arr[i+1];
			arr[i+1] = temp;
			isSorted = -1;
			}
		}
		pthread_mutex_unlock(&lock);
	}

	return;
}

// A utility function ot print an array of size n
void printArray(int arr[], int n)
{
int i=0;
	for (i = 0; i < n; i++)
		printf("%d ", arr[i]);
	
}

// Driver program to test above functions.
int main(int argc, char * argv[])
{
int n= atoi(argv[1]);
int * arr = (int*)malloc(n * sizeof(int));

int i=0;
for(i=0; i<n; i++)
{
arr[i] = rand()%1000;

}


if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

	oddEvenSort(arr, n);
	printArray(arr, n);
free(arr);

	return (0);
}
