#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pzip.h"

//my inclusion
#include "pthread.h"

//global variables 
pthread_barrier_t barrier; 

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


typedef struct zipped_str {
	char *  str; 
	struct zipped_char * zipped_chars; 
	int index; 
}zipper;



static void * 
func(void *arg){
	zipper *zipp = (zipper *) arg; 
	int count = 1; 
	char c;
	for(int i = 0; i < strlen(zipp->str) - 1; ++i){
		c = zipp->str[i];
		if(zipp->str[i] == zipp->str[i+1]){
			count = count + 1; 
		}else{
			fprintf(stdout, "%c: %d\n",c,count);
			/*
			struct zipped_char temp_zip;
			temp_zip.character = c;
			temp_zip.occurence = count;
			zipp->zipped_chars[zipp->index+i] = temp_zip; 
			*/
			count = 1;
		}
	}
	/*
	struct zipped_char temp_zip;
	temp_zip.character = c;
	temp_zip.occurence = count;
	zipp->zipped_chars[zipp->index+strlen(zipp->str)] = temp_zip; 
	*/

	pthread_barrier_wait(&barrier); 
	fprintf(stdout, "%c: %d\n\n",c,count); 
	free(zipp->str); 
	free(zipp); 
	pthread_exit(NULL);
}


void pzip(int n_threads, char *input_chars, int input_chars_size,
	  struct zipped_char *zipped_chars, int *zipped_chars_count,
	  int *char_frequency)
{
	//barrier init
	pthread_barrier_init (&barrier, NULL, n_threads);

	//struct zipped_char * zipped_chars_2 = (struct zipped_char *) malloc(input_chars_size*sizeof(struct zipped_char)); 
	void *tret; 
	int str_n = input_chars_size/n_threads; 
	pthread_t threads[n_threads]; 
	for(int i = 0; i <  n_threads; ++i){
		//string intialization 
		char * str = (char *) malloc(str_n*sizeof(char)); 
		for(int j = 0; j < str_n; ++j){
			str[j] = input_chars[i*str_n+j]; 
		}
		zipper* zipp = (zipper *)malloc(sizeof(zipper)); 
		zipp->str = str; 
		zipp->zipped_chars = zipped_chars;
		zipp->index = i*str_n; 
		if(pthread_create(&threads[i],NULL,*func,(void *)zipp)){
			fprintf(stderr,"Error: thread creation"); 
			exit(-1); 
		} 
	}
	for(int i = 0; i < n_threads; ++i){
		if(pthread_join(threads[i],&tret)){
			fprintf(stderr,"Error: thread joining"); 
		}
	}
	pthread_barrier_destroy (&barrier); 
	pthread_exit(NULL); 

	/*
	for(int i = 0; i < 2; ++i){
		fprintf(stdout,"%c: %u",zipped_chars_2->character,zipped_chars_2->occurence); 
	}
	*/
	//free(zipped_chars_2); 
	
	exit(0);
}
