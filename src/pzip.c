#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pzip.h"

//my inclusion
#include "pthread.h"

//thread struct
typedef struct zipped_str {
	//char *  str; 
	struct zipped_char *z_chars;
	//int str_n; 
	int z_char_n; 
}zipper;

//global variables 
pthread_barrier_t barrier; 
int * glob_cfreq;
zipper * thread_arr; 
struct zipped_char *global_chars; 
pthread_mutex_t lock[26]; 
int thread_count;
int *zipp_n;
int str_n;
char * str;
//TODO creaeete mutex lock array


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


static void * 
func(void *arg){
	int temp_cfreq[26] = {0};
	long tn = (long)arg;
	char c;
	int count = 1;
	thread_arr[tn].z_chars = (struct zipped_char *) malloc(str_n*sizeof(struct zipped_char));
	thread_arr[tn].z_char_n = 0;
	for(int i = str_n*tn; i < str_n*(tn+1)-1; ++i){
		//c = thread_arr[tn].str[i]; 
		c = str[i]; 
		//if(thread_arr[tn].str[i] == thread_arr[tn].str[i+1]){
		if(str[i] == str[i+1]){
			count = count + 1; 
		}else{
			temp_cfreq[c - 97] += count; 
			thread_arr[tn].z_chars[thread_arr[tn].z_char_n].character = c;
			thread_arr[tn].z_chars[thread_arr[tn].z_char_n].occurence = count; 
			thread_arr[tn].z_char_n +=1; 
			count = 1;
		}
	}
	temp_cfreq[c - 97] += count;  
	thread_arr[tn].z_chars[thread_arr[tn].z_char_n].character = c;
	thread_arr[tn].z_chars[thread_arr[tn].z_char_n].occurence = count; 
	thread_arr[tn].z_char_n +=1; 
	pthread_barrier_wait(&barrier);
	if(tn == 0){
		int arr_size = 0;
		for(int i = 0; i < thread_count; ++i){
			arr_size += thread_arr[i].z_char_n; 
		}
		*zipp_n = arr_size; 
	}
	int zchars_i = 0;
	for(int i = 0; i < tn; ++i){
		zchars_i += thread_arr[i].z_char_n; 
	}
	for(int i = zchars_i; i < zchars_i + thread_arr[tn].z_char_n; ++i){
		global_chars[i].character = thread_arr[tn].z_chars[i - zchars_i].character;
		global_chars[i].occurence = thread_arr[tn].z_chars[i - zchars_i].occurence;
	}

	//lock mutex
	for(int i = 0; i <  26; ++i){
		if(temp_cfreq[i] != 0){
			if(pthread_mutex_lock(&lock[i])){
				fprintf(stderr,"Error: mutex locking");
				exit(-1);
			}
			glob_cfreq[i] += temp_cfreq[i];

			if(pthread_mutex_unlock(&lock[i])){
				fprintf(stderr,"Error: mutex unlocking"); 
				exit(-1);
			}
		}
	}

	
	free(thread_arr[tn].z_chars); 
	pthread_exit(NULL);
}


void pzip(int n_threads, char *input_chars, int input_chars_size,
	  struct zipped_char *zipped_chars, int *zipped_chars_count,
	  int *char_frequency)
{
	//global stuff
	global_chars = zipped_chars;
	glob_cfreq = char_frequency;
	zipp_n = zipped_chars_count; 
	str = input_chars;
	//init
	if(pthread_barrier_init (&barrier, NULL, n_threads)){
		fprintf(stderr,"Error: barier initialization");
		exit(-1);
	}
	for(int i = 0; i < 26; ++i){
		if(pthread_mutex_init(&lock[i],NULL)){
			fprintf(stderr,"Error: mutex initialization"); 
			exit(-1);
		}
	}
	void *tret; 

	thread_count = n_threads; 
	pthread_t threads[n_threads]; 
	str_n = input_chars_size/n_threads; 
	thread_arr = (zipper *)malloc(n_threads*sizeof(zipper)); 
	for(long i = 0; i <  n_threads; ++i){
		//thread_arr[i].str = input_chars;
		//str_n = input_chars_size/n_threads; 

		if(pthread_create(&threads[i],NULL,*func,(void *)i)){
			fprintf(stderr,"Error: thread creation"); 
			exit(-1); 
		} 
	}
	for(int i = 0; i < n_threads; ++i){
		if(pthread_join(threads[i],&tret)){
			fprintf(stderr,"Error: thread joining"); 
			exit(-1);
		}
	}
	if(pthread_barrier_destroy (&barrier)){
		fprintf(stderr,"Error: barrier destruction");
		exit(-1);
	} 
	for(int i = 0; i < 26; ++i){
		if(pthread_mutex_destroy(&lock[i])){
		fprintf(stderr,"Error: mutex destruction");
		exit(-1);
	}
	}
	
	free(thread_arr); 
}
