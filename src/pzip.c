#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pzip.h"

//my inclusion
#include "pthread.h"

/**
 * pzip() - zip an array of characters in parallel
 *
 * Inputs:
 * @n_threads:		   The number of threads to use in pzip
 * @input_chars:		   The input characters (a-z) to be zipped
 * @input_chars_size:	   The number of characaters in the input file
 *
 * Outputs:
 * @zipped_chars:       The array of zipped_char structs
 * @zipped_chars_count:   The total count of inserted elements into the zippedChars array.
 * @char_frequency[26]: Total number of occurences
 *
 * NOTE: All outputs are already allocated. DO NOT MALLOC or REASSIGN THEM !!!
 *
 */

/*
struct str {
	char *  chars; 
};
*/


static void * 
func(void *arg){
	//fprintf(stdout,"thread printed\n"); 
	char *str = arg; 
	fprintf(stdout,"%s\n",str); 
	free(str); 
	pthread_exit(NULL);
}


void pzip(int n_threads, char *input_chars, int input_chars_size,
	  struct zipped_char *zipped_chars, int *zipped_chars_count,
	  int *char_frequency)
{
	//barrier init
	//pthread_barrier_t barrier; 
	//pthread_barrier_init (&barrier, NULL, 3);


	int str_n = input_chars_size/n_threads; 
	pthread_t threads[n_threads]; 
	for(int i = 0; i <  n_threads; ++i){
		//string intialization 
		char * str = malloc(str_n*sizeof(char)); 
		for(int j = 0; j < str_n; ++j){
			str[j] = input_chars[i*str_n+j]; 
		}
		if(pthread_create(&threads[i],NULL,*func,(void *)str)){
			fprintf(stderr,"Error: thread creation"); 
			exit(-1); 
		} 
	}
	pthread_exit(NULL); 
	
	exit(0);
}
