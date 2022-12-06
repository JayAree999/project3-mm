#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "mm.h"
#include <string.h>

//for accessing matrices index
#define ARRAY(n,m) ((n*(long)SIZEX)+m)

//defined in mm.h
// #define SIZEX 1000
//#define SIZEY 1000


// Task 1: Flush the cache so that we can do our measurement :)
void flush_all_caches() {

	
	const void *p;
    const long *cp = (const long *)p;

    size_t i = 0;
    unsigned int allocation_size = (long)SIZEX*(long)SIZEY;
    
    if (p == NULL || allocation_size <= 0)
            return;
    
  

     for (i = 0; i <  allocation_size ; i++) { // allocation size
     		
            asm volatile("clflush (%0)\n\t"
                         : 
                         : "r"(huge_matrixA + i) // pointer to add
                         : "memory");

            asm volatile("clflush (%0)\n\t"
                         : 
                         : "r"(huge_matrixB + i) // pointer to add
                         : "memory");
                         
            asm volatile("clflush (%0)\n\t"
                         : 
                         : "r"(huge_matrixC + i) // pointer to add
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
			
			long res = 0;
			
			//multiply
			for(long k = 0; k < (long)SIZEY; k++){
					
				res += (huge_matrixA[ARRAY(i,k)] * huge_matrixB[ARRAY(k,j)]);
			}

			huge_matrixC[ARRAY(i,j)] = res;
		}
	}
	
}

void compare_results()
{
	fout = fopen("./out.in","r");
	long i;
	long temp1, temp2;
	for(i=0;i<((long)SIZEX*(long)SIZEY);i++)
	{
		fscanf(fout, "%ld", &temp1);
		fscanf(fout, "%ld", &temp2);
		if(temp1!=temp2)
		{
			printf("Wrong solution!");
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
}

void load_matrix()
{
	// Your code here
}



void multiply()
{
	// Your code here
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
	
	print_matrix();
	fclose(fin1);
	fclose(fin2);
	fclose(fout);
	free_all();
	
	flush_all_caches();
	
	

	/*
	//flush_all_caches();

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
	write_results();
	
	compare_results();
	
	fclose(fin1);
	fclose(fin2);
	fclose(fout);
	*/
	

	return 0;
}


