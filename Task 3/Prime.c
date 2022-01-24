#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//nos is the array that we are going to store our prime numbers in, prime count variable counts the total prime numbers that we have found 
int *nos, primecount = 0;
FILE *fileptr;
//mutex variable to lock the critical section while we are searching for the prime numbers
pthread_mutex_t mutex;

//this structure is used to pass starting and end index to the thread function
struct calc{
	int start;
	int finish;
};

//a function which returns 1 if the argument is prime. (Uses recursion to check if a number is prime or not.)
int checkPrime(int n, int i) { 
    if(n <= 2){
        return (n == 2) ? 1:0; 
    } 
    if(n%i == 0){
        return 0; 
    } 
    if(i*i > n){
        return 1; 
	}
    return checkPrime(n, i+1); 
}
  
//thread function which checks if the numbers in our array is prime or not. prints prime numbers in the file.
void *isPrime(void *ptr){
	struct calc *arg = ptr;
	int st = arg->start, fi = arg->finish;
	int i, j;
	for(i=st; i<=fi; i++){
		int num = *(nos+i);
		if(checkPrime(num, 2) == 1){	
			pthread_mutex_lock(&mutex);
			primecount++;
			fprintf(fileptr, "%d. %d\n",primecount, num);
			pthread_mutex_unlock(&mutex);
		}
	}
}  
  

void main(){
	
    int i, n = 0, j=0, nooffiles = 3; 
    FILE *fptr;
    char filename[15], chr;
    int numOfThreads;
    
    //we find the total number of numbers that we have in individual files.
    for(i=1; i<=nooffiles; i++){
        sprintf(filename, "PrimeData%d.txt", i);
        fptr = fopen(filename, "r");
        chr = getc(fptr);
        while(chr != EOF){
        	if(chr == '\n'){
        		if(chr != ' '){
        			n++;	
				}
			}	
            chr = getc(fptr);
		}
        fclose(fptr);
    }
    
    printf("Let's find all the prime numbers in the given files.\nHmm... there are alot of (%d) numbers to check. Let's use Multithreading!", n);
	printf("\n\nPlease enter the number of threads that you want to use: ");
    scanf("%d", &numOfThreads);
    struct calc divider[numOfThreads];
    
    //now we use that number to dynamically allocate a 1 dimensional array which we use to store all the numbers in the given files.
    nos = (int*)malloc(n*sizeof(int));
    for(i=1; i<=nooffiles; i++){
        sprintf(filename, "PrimeData%d.txt", i);
        fptr = fopen(filename, "r");
        while(!feof(fptr)){
            fscanf(fptr ,"%d\n", (nos+j));
            j++;
		}
		fclose(fptr);
    }
    
    //slicing the threads.
    int slicelist[numOfThreads];
	for(i=0; i<numOfThreads; i++){
		slicelist[i] = n/numOfThreads;		
	}
	int reminder = n%numOfThreads;
	for(i=0; i<reminder; i++){
		slicelist[i]++;
	}
	int startList[numOfThreads], endList[numOfThreads];
	for(i=0; i<numOfThreads; i++){
        if (i==0) {
            startList[i] = 0;
        } else {
            startList[i] = endList[i-1] + 1;
        }
        endList[i] = startList[i] + slicelist[i] - 1;
    }
    
    //opeaning a new file in write mode. 
    fileptr = fopen("Prime Numbers.txt","w");
    //initializing mutex.
	pthread_mutex_init(&mutex, NULL);
	
	//adding start index and end index in our structure and also creating threads which checks all the numbers in the array from the start index and the end index.
	pthread_t threads[numOfThreads];
    for(i=0; i<numOfThreads; i++){
        divider[i].start = startList[i];
        divider[i].finish = endList[i];
	    pthread_create(&threads[i], NULL, isPrime, &divider[i]);
    }
	//joining the threads
	for(i=0; i<numOfThreads; i++){
		pthread_join(threads[i], NULL);
    }
    //destroying the initialized mutex.
    pthread_mutex_destroy(&mutex);

	//printing the total prime numbers found in the given files.
	printf("\n\nPrime Count: %d\n", primecount);
 	fprintf(fptr, "\n\nThere are total of %d prime numbers.", primecount);
 	free(nos);
    printf("\nComputation complete!\n\n\"Prime Numbers.txt\" file has been created in the same folder.\nPlease check the file to see the all the prime numbers.");
    fclose(fileptr);
}
