#include <stdio.h>


//the main function of the program
void main(){
	
    //i is the iterator, j is the count and n is the totalnumberofdata variables 
    int i, j, n=0; 
    /*a and b are the variables for gradient and constant respectively, sumx, sumy, sumxsq and sumxy are the variables which  
      stores the sum of x, y x^2 and x*y respectively*/
    float a, b, sumx=0, sumy=0, sumxsq=0, sumxy=0;
    float x, y;
    FILE *fptr;
    char filename[15];
 
    //looping through the 4 data files 
    for(i=1; i<=4; i++){
        //formatting the names of the files 
        sprintf(filename, "datasetLR%d.txt", i);
        fptr = fopen(filename, "r");
        
        //taking file input till the end of file and using the input data to calculate sumx, sumy, sumxy and xymxsq
        while(!feof(fptr)){
            fscanf(fptr, "%f,%f\n", &x, &y);
            sumx += x;
            sumy += y;
            sumxy += x*y;
            sumxsq += (x*x);
            n++;
        }
        fclose(fptr);
    }
	
    //calculating the constant
    b = (n*sumxy - sumx*sumy)/(n*sumxsq - sumx*sumx);
    //calculating the gradient
    a = (sumy*sumxsq-sumx*sumxy) / (n*sumxsq - sumx*sumx);

    //printing the equation
    printf("The equation of regression line generated from the given dataset is: y = %.2fx + %.2f\n\nWhere,\ngradient(a) = %.2f and\nconstant(b) = %.2f", b, a, a, b); 
	
    //taking 'x' value from user as input to calculate and display the new output 'y'
    printf("\n\nPlease enter the new value that you want to predict using this model: ");
	scanf("%f", &x);
	y = b*x + a;
	printf("The corresponding y coordinate of the given input would be: %.2f\n\n", y);
    
}
