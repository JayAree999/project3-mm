#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "mm.h"
#include <string.h>
#include <pthread.h>
#include "mm-mt.h"

#define MAX_THREADS 4
#define BLOCK_SIZE 6
//for accessing matrice indexes row/column
#define ARRAY(n,m) ((n*(long)SIZEX)+m)
typedef struct blockInfo{
    int blocksize;
    int row;
    int col;
    int shutdown;
} blockInfo;


//.h
// #define SIZEX 30
//#define SIZEY 30

// Work queue
blockInfo* work_q;
int qSize;
int head = 0;
int tail = 0; 
int workCount = 0;
// Mutex and Conditional variables
pthread_t* workers;
pthread_mutex_t mutexParse;
pthread_mutex_t mutexQ;
pthread_cond_t condQ;


void addJob(blockInfo data) {
    pthread_mutex_lock(&mutexQ);
    if (workCount < qSize) {
        work_q[tail] = data;
        tail = (tail+1) % qSize;
        workCount++;
    }
    pthread_mutex_unlock(&mutexQ);
    pthread_cond_signal(&condQ);
}


int removeJob(blockInfo* data) {
    int success = (workCount) ? 1 : 0;
    if (success) {
        *data = work_q[head];
        head = (head+1) % qSize;
        workCount--;
    }
    return success;
}
void* start_thread(void* args) {
    while(1) {
   	
   	
        pthread_mutex_lock(&mutexQ);
        blockInfo data;
        while (!removeJob(&data)) {
            pthread_cond_wait(&condQ, &mutexQ);
        }
        pthread_mutex_unlock(&mutexQ);
    
        if (data.shutdown < 0) break;
        
        block_multiply(data.blocksize,data.row,data.col);

    }
    return NULL;
}



// Task 1: Flush the cache so that we can do our measurement :)
void flush_all_caches() {

	
	
	const void *p;
	const long * cp = (const long * )p;
    size_t i = 0;
    unsigned int allocation_size = (long)SIZEX*(long)SIZEY;
    
    if (p == NULL || allocation_size <= 0)
            return;
    

     for (i = 0; i <  allocation_size ; i++) { // allocation size
     		
            asm volatile("clflush (%0)\n\t"
                         : 
                         : "r"(huge_matrixA + i) // 
                         : "memory");

            asm volatile("clflush (%0)\n\t"
                         : 
                         : "r"(huge_matrixB + i) //
                         : "memory");
                         
            asm volatile("clflush (%0)\n\t"
                         : 
                         : "r"(huge_matrixC + i) //
                         : "memory");
    	}

    
    	asm volatile("sfence\n\t"
                 :
                 :
                 : "memory");
                 
	memset(huge_matrixA, 0, (long)SIZEX*(long)SIZEY);
	memset(huge_matrixB, 0, (long)SIZEX*(long)SIZEY);
	memset(huge_matrixC, 0, (long)SIZEX*(long)SIZEY);
		
}

void load_matrix_base()
{	
	
	printf("load_matrix_base\n");
	long i;
	huge_matrixA = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY); 
	huge_matrixB = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixC = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	
	// Load the input
	// Note: This is suboptimal because each of these loads can be done in parallel.
	for(i=0;i<((long)SIZEX*(long)SIZEY);i++)
	{
		fscanf(fin1,"%ld", (huge_matrixA+i)); 		
		fscanf(fin2,"%ld", (huge_matrixB+i)); 		
		huge_matrixC[i] = 0;		
	}
}

void free_all()
{
	free(huge_matrixA);
	free(huge_matrixB);
	free(huge_matrixC);
}

void multiply_base()
{	//row - wise
	for(long i = 0; i < (long)SIZEX; i++){  // row
		for(long j = 0; j < (long)SIZEY; j++){ // column
			
			long total = 0;
			
			//multiply
			for(long k = 0; k < (long)SIZEY; k++){
			
				total += (huge_matrixA[ ARRAY(i,k)] * huge_matrixB[ARRAY(k,j)]);
			
			//resulting matrix 
			huge_matrixC[ ARRAY(i,j)] = total;
			}
		}
	}
	
}

void compare_results()
{
	fout = fopen("./out.in","r");
	ftest = fopen("./reference.in","r");
	long i;
	long temp1, temp2;
	for(i=0;i<((long)SIZEX*(long)SIZEY);i++)
	{
		fscanf(fout, "%ld", &temp1);
		fscanf(ftest, "%ld", &temp2);
		if(temp1!=temp2)
		{
			printf("Wrong solution!");
			exit(1);
		}
		else {
			printf("Correct!");
			exit(1);
		}
		
	}
	fclose(fout);
	fclose(ftest);
}

void write_results()
{
	// Your code here
	//
	// Basically, make sure the result is written on fout
	// Each line represent value in the X-dimension of your matrix
	
	char buffer[256];
	fout = fopen("./out.in","w");

	// read from matrixC and write in fout
	for( long i = 0; i < (long)SIZEX; i++){
		for( long j = 0; j <(long)SIZEY; j++){

			memset(buffer, '\0', 256);
			sprintf(buffer, "%ld ", huge_matrixC [ARRAY(i,j)]);
			fwrite(buffer, sizeof(char), strlen(buffer), fout); //write a line to fout
			
		}
		char* end = "\n"; // end of line
		fwrite(end, sizeof(char), strlen(end), fout); //write new line
	}
}

void load_matrix()
{
	printf("load_matrix\n");
	long i;
	huge_matrixA = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY); 
	huge_matrixB = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixC = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	
	// Load the input
	// Note: This is suboptimal because each of these loads can be done in parallel.
	for(i=0;i<((long)SIZEX*(long)SIZEY);i++)
	{
		fscanf(fin1,"%ld", (huge_matrixA+i)); 		
		fscanf(fin2,"%ld", (huge_matrixB+i)); 		
		huge_matrixC[i] = 0;		
	}
}


void multiply(){


  // Initializing mutex and conditional variables
    pthread_mutex_init(&mutexQ, NULL);
    pthread_mutex_init(&mutexParse, NULL);
    pthread_cond_init(&condQ, NULL);

    qSize = MAX_THREADS;
    blockInfo q[qSize*10];
    work_q = q;
    // Initializing threads
    pthread_t temp[MAX_THREADS];
    workers = temp;
    for (int i=0; i<MAX_THREADS; i++) {
        if (pthread_create(&workers[i], NULL, &start_thread, NULL) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);   
        }
    }
    printf("ADD BLOCK\n");
    //add threading block
	
	for(long i = 0; i < (long)SIZEX; i += BLOCK_SIZE){
		for(long j = 0; j < (long)SIZEY; j += BLOCK_SIZE){
				blockInfo block;
			  	block.blocksize= BLOCK_SIZE;
				block.row=i;
				block.col=j;
				block.shutdown = 1;
				addJob(block);
			
		}
	}
	
    	printf("FINISH ADD\n");

	
	//-----------------------------DONE THREADING------------------------------
	 for (int i=0; i<MAX_THREADS; i++) {
		blockInfo data;
		data.shutdown = -1;
		addJob(data);
	    }

	//after finish above destroy
	for (int i = 0; i < MAX_THREADS; i++) {
        if (pthread_join(workers[i], NULL) != 0) {
            perror("Failed to join the thread");
        }
    	}
	
    pthread_mutex_destroy(&mutexQ);
    pthread_mutex_destroy(&mutexParse);
    pthread_cond_destroy(&condQ);
    printf("Exiting...\n");
  

}
void block_multiply(int bsize, int row, int column) {
	printf("block_multiply\n");
	
 	int i, j, k, kk, jj; // iterators

 
 	double sum;
 	int en = bsize * ((long)SIZEX/bsize); /* Amount that fits evenly into blocks */

	
	// block size version from pdf
	

	for (kk = 0; kk < en; kk += bsize) {
		for (jj = 0; jj < en; jj += bsize) {
			for (i = 0; i < (long)SIZEX; i++) {
				for (j = 0; j < jj + bsize; j++) {
				
 					sum = huge_matrixC[ARRAY(i,j)];
 			
					for (k = kk; k < kk + bsize; k++) {
 						sum += (huge_matrixA [ARRAY(i,k)]) * 
 						(huge_matrixB [ARRAY(k,j)]) ;
					}
					huge_matrixC [ARRAY(i,j)] = sum;
				}
 			}
		}
 	}
 }


//for printing 
void print_matrix(){

    for (long i = 0; i < (long)SIZEX*(long)SIZEY; i++)
    {
        printf("%ld ", huge_matrixC[i]);
    }
    printf("\n");
}


int main()
{
	
	clock_t s,t;
	double total_in_base = 0.0;
	double total_in_your = 0.0;
	double total_mul_base = 0.0;
	double total_mul_your = 0.0;
	fin1 = fopen("./input1.in","r");
	fin2 = fopen("./input2.in","r");
	fout = fopen("./out.in","w");
	ftest = fopen("./reference.in","r");
		
	flush_all_caches();
	
	s = clock();
	load_matrix_base();
	t = clock();
	total_in_base += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("[Baseline] Total time taken during the load = %f seconds\n", total_in_base);
	
	
	s = clock();
	
	multiply_base();
	t = clock();
	total_mul_base += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("[Baseline] Total time taken during the multiply = %f seconds\n", total_mul_base);
	
	fclose(fin1);
	fclose(fin2);
	fclose(fout);
		
		
	
	flush_all_caches();
	free_all();
	
	
	// Block Size / Threading Version
	
	fin1 = fopen("./input1.in","r");
	fin2 = fopen("./input2.in","r");
	fout = fopen("./out.in","w");
	ftest = fopen("./reference.in","r");
	
	s = clock();
	load_matrix();
	t = clock();
	total_in_your += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("Total time taken during the load = %f seconds\n", total_in_your);


	s = clock();
	
	multiply();

	t = clock();
	total_mul_your += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("Total time taken during the multiply = %f seconds\n", total_mul_your);
	print_matrix();
	

	
	write_results();
	
	
	fclose(fin1);
	fclose(fin2);
	fclose(fout);
	free_all();
	compare_results();

    
    
    	printf("Exiting...\n");
	return 0;

}


// References 

//multiply_base 
//accessing indexes and printing
//https://stackoverflow.com/questions/57324924/how-to-create-a-big-matrix-like-with-1000-rows-columns-in-c

//Flushing the cache
 
//https://stackoverflow.com/questions/11277984/how-to-flush-the-cpu-cache-in-linux-from-a-c-program
//




