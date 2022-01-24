#include <stdio.h>
#include <math.h>
#include <pthread.h>

//creating a structure for dividing the threads later
struct th_st_fin{
	int start;
	int finish;
	int tid;
};

//this is the variable that we are going to sum through different threads in order to calculate pi
double sum = 0;
//this is the thread function that we use to calculate the value of pi using the formula
void *calculatepi(void* ptr){
	struct th_st_fin *arg = ptr;
	int st = arg->start, fi = arg->finish, tno = arg->tid;
	int i; double s = 0;
	for(i=st; i<=fi; i++) {
		s += (pow(-1, i))/((2*i)+1);
	}
	sum += s;
}



void main(){
	
	//declaring the variables 
	int i, num, iterations;
	
	//asking the users for the number of iterations and the number of threads to split those iterations into
	printf("Please enter the number of iterations that you want: ");
    scanf("%ld", &iterations);
    printf("Enter the number of threads that you want to use: ");
    scanf("%d", &num);
    
    
	//we are now going to slice the number if iterations into equal or near to equal parts
    int slicelist[num];
	for(i=0; i<num; i++){
		slicelist[i] = iterations/num;		
	}
	int reminder = iterations%num;
	for(i=0; i<reminder; i++){
		slicelist[i]++;
	}


    //creating the thread
	struct th_st_fin divider[num];
	pthread_t threads[num];
    for(i=0; i<num; i++){
        divider[i].start = (i*slicelist[i]);
        divider[i].finish = (i*slicelist[i]) + slicelist[i] - 1;
        divider[i].tid = i;
		pthread_create(&threads[i], NULL, calculatepi, &divider[i]);
    }

	//joining the threads
	for(i=0; i<num; i++){
	 	pthread_join(threads[i], NULL);
    }
    
    
    float pi = sum*4;
    printf("\n\nValue of PI is approximated to = %.8f", pi);
	
}
