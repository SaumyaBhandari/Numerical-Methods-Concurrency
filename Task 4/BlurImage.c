#include <stdio.h>
#include "lodepng.h"
#include <stdlib.h>
#include <pthread.h>

/* we now declare a 3 dimensional array. The first dimension stores the pixels in the height of the image
the second dimension stores the pixels in the width of the image and the third dimension stores the 
red, green and blue values of the individual pixel */
int ***arr;

// declaring a character image array. 
unsigned char *Image;


//declaring a structure to pass into the thread function
struct pixels{
    int starth;
    int endh;
    int h;
    int w;
    int tid;
    int kernel_size;
};


//this is the thread function that we use to blur our image.
void *blur(void *ptr){

    int i, j, k, l, m; 
    struct pixels *arg = (struct pixels*)ptr;
	int sth = arg->starth, fih = arg->endh, kernelsize = arg->kernel_size;
    int thread_id = arg->tid, height = arg->h, width = arg->w;
    
    //first we travese through the height of the image. height is sliced almost equally for the threads
    for(i=sth; i<=fih; i++ ){
    	//then we traverse through the whole width of the image
        for(j=0; j<width; j++){
        	//we then set the average value of each RGB pixel to 0, and the number of pixels to 0
            int avgR=0, avgG=0, avgB=0, numOfPixels = 0;
            //now we traverse the kernel, which is a 2D matrix; through the image. 
            for (l= -kernelsize; l<=kernelsize; l+=kernelsize){
                for (m = -kernelsize; m<=kernelsize; m+=kernelsize){
                    int xPixel = i+l;
                    int yPixel = j+m;
                    //check if there exists a pixel in our image at the given index
                    if(xPixel >=0 && yPixel >= 0 && xPixel<height && yPixel<width){
                        avgR += arr[xPixel][yPixel][0];
                        avgG += arr[xPixel][yPixel][1];
                        avgB += arr[xPixel][yPixel][2];
                        numOfPixels++;
                    }          
                }
            }
            //now we set the R G and B value of our image with the new red green and blue values. 
            Image[4*width*i+4*j+0] = avgR/numOfPixels;
            Image[4*width*i+4*j+1] = avgG/numOfPixels;
            Image[4*width*i+4*j+2] = avgB/numOfPixels;
        }
    }
    pthread_exit(0);

}

void main(){

	unsigned int error, width, height;
    int i, j, k;
    int numOfThreads, ksize;
    char filename[20];
    printf("Enter the file that you want to blur: ");
    scanf("%s", &filename);
    error = lodepng_decode32_file(&Image, &width, &height, filename);
    printf("\nPlease enter the number of threads that you want to use to blur the image: ");
    scanf("%d", &numOfThreads);
    if(numOfThreads > height){
    	numOfThreads = numOfThreads%height + 1;
    	printf("\nThe threads that you want to use are greater than the height of the image!\nThe program will automatically choose the appropriate threads for blurring this image: %d threads", numOfThreads);
	}
    printf("\n\nPlease enter the size of the Kernel(blur matrix) that you want to use\nEnter \"1\" for 3x3 matrix, \"2\" for 5x5, \"3\" for 7x7 and so on: ");
    scanf("%d", &ksize);
    
    //initializing the 3d array dynamically reading the height and width of the image.
    arr = (int***)malloc(height * sizeof(int**));
    for (i = 0; i < height; i++) {
        arr[i] = (int**)malloc(width * sizeof(int*));
        for (j = 0; j < width; j++) {
                arr[i][j] = (int*)malloc(3 * sizeof(int));
        }
    }

	//storing the values of pixels in our array.
    if(error){
        printf("Error in decoding image %d: %s\n", error, lodepng_error_text(error));
    }else{
        for(i=0; i<height; i++){
            for(j=0; j<width; j++){
                for(k=0; k<3; k++){
                    arr[i][j][k] = Image[4*width*i+ 4*j + k];
                }
            }
        }
    }

	//now we slice our array based on the height of the image.
    int sliceHeight[numOfThreads];
	for(i=0; i<numOfThreads; i++){
		sliceHeight[i] = height/numOfThreads;			
	}
	int reminderHeight = height%numOfThreads;
	for(i=0; i<reminderHeight; i++){
		sliceHeight[i]++;
	}
	int startHeight[numOfThreads], endHeight[numOfThreads];
	for(i=0; i<numOfThreads; i++){
        if (i==0) {
            startHeight[i] = 0;
        } else {
            startHeight[i] = endHeight[i-1] + 1;
        }
        endHeight[i] = startHeight[i] + sliceHeight[i] - 1;
    }

    
    //now we add the sliced heights, height width and everything that we require to the structure
    //we also run the blur function which now blurs our image using multiple threads.
    struct pixels divider[numOfThreads];
    pthread_t threads[numOfThreads];
    int thid = 1;
    for(i=0; i<numOfThreads; i++){
        divider[i].starth = startHeight[i];
        divider[i].endh = endHeight[i];
        divider[i].h = height;
        divider[i].w = width; 
        divider[i].tid = thid;
        divider[i].kernel_size = ksize;
        thid++;
        pthread_create(&threads[i], NULL, blur, &divider[i]);   
    }
    //now we join the threads that we have created.
    for(i=0; i<numOfThreads; i++){
        pthread_join(threads[i], NULL);   
    }
	
	//now lets encode a new image using lodepng_encode32(); function.
    unsigned char *png;
    char imagename[20];
    size_t pngsize;
    printf("\n\nImage has been blurred. Please enter the name of output image: ");
    scanf("%s", &imagename);
    lodepng_encode32(&png, &pngsize, Image, width, height);
    lodepng_save_file(png, pngsize, imagename);
    
}
