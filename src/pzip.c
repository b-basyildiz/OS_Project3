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
int glob_cfreq[26] = {0}; 
zipper * thread_arr; 
//make global zipped_char
pthread_mutex_t lock; 


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
	struct zipped_char *z_chars;
	int str_n; 
	int z_char_n; 
}zipper;

typedef struct index_zipp {
	zipper * zipped; 
	int t_i; 
}i_zipp;

static void * 
func(void *arg){
	int temp_cfreq[26] = {0};
	int z = (int)arg;
	i_zipp *z = (i_zipp *) arg; 
	z->zipped->z_char_n = 0;
	char * str = z->zipped->str;
	struct zipped_char *zipped_chars = z->zipped->z_chars; 
	int n = z->zipped->str_n; 
	int tn = z->t_i; 
	char c;
	int count = 1;
	int str_n = z->zipped->str_n;
	zipped_chars = (struct zipped_char *) malloc(str_n*sizeof(struct zipped_char));
	int j = 0;
	for(int i = n*tn; i < n*(tn+1)-1; ++i){
		c = str[i]; 
		if(str[i] == str[i+1]){
			count = count + 1; 
		}else{
			fprintf(stdout, "%c: %d\n",c,count);
			temp_cfreq[c - 97] += count; 
			zipped_chars[j].character = c;
			zipped_chars[j].occurence = count; 
			j += 1;
			count = 1;
		}
	}
	fprintf(stdout, "%c: %d\n\n",c,count);
	temp_cfreq[c - 97] += count;  
	zipped_chars[j].character = c;
	zipped_chars[j].occurence = count; 
	z->zipped->z_char_n = j;
	pthread_barrier_wait(&barrier); 
	//sum all the previous threads character occurences 
	// and then put your zippde characters 

	//lock mutex
	//combine arrays 
	//unlock
	

	free(z); 
	pthread_exit(NULL);
}


void pzip(int n_threads, char *input_chars, int input_chars_size,
	  struct zipped_char *zipped_chars, int *zipped_chars_count,
	  int *char_frequency)
{
	//init
	pthread_barrier_init (&barrier, NULL, n_threads);
	pthread_mutex_init(&lock,NULL); 

	//struct zipped_char * zipped_chars_2 = (struct zipped_char *) malloc(input_chars_size*sizeof(struct zipped_char)); 
	void *tret; 
	zipper* zipp = (zipper *)malloc(sizeof(zipper)); 
	zipp->str = input_chars; 
	//zipp->z_chars = zipped_chars;
	zipp->str_n = input_chars_size/n_threads; 

	i_zipp* zi = (i_zipp *)malloc(sizeof(i_zipp)); 
	zi->zipped = zipp; 

	pthread_t threads[n_threads]; 
	thread_arr = (zipper *)malloc(n_threads*sizeof(zipper)); 
	for(int i = 0; i <  n_threads; ++i){
		//string intialization
		/*
		i_zipp* zi = (i_zipp *)malloc(sizeof(i_zipp)); 
		zi->zipped = zipp; 
		zi->t_i = i; 
		*/

		//assign all things for zipp TODO
		thread_arr[i]
		if(pthread_create(&threads[i],NULL,*func,(void *)i)){
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
	pthread_mutex_destroy(&lock); 

	/*
	for(int i = 0; i < 2; ++i){
		fprintf(stdout,"%c: %u",zipped_chars_2->character,zipped_chars_2->occurence); 
	}
	*/
	//free(zipped_chars_2); 
	free(zipp); 
	exit(0);
}
