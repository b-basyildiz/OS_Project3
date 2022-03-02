#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"
#include "pzip.h"

int main(int argc, char *argv[])
{
	if (argc < 4) {
		printf("Error: Missing number of arguments!!\n");
		printf("\tUsage: pzip INPUT_FILE OUTPUT_FILE N_THREADS [--debug]\n");
		exit(EXIT_FAILURE);
	}

	const char *input_file = argv[1];
	char *output_file = argv[2];
	int n_threads = atoi(argv[3]);

	if (n_threads < 1) {
		printf("Error: Invalid input for n_threads\n");
		exit(EXIT_FAILURE);
	}

	int debug = 0;

	if (argc > 4 && !strcmp(argv[4], "--debug"))
		debug = 1;

	char *input_chars;
	int input_chars_size;
	struct zipped_char *zipped_chars;
	int zipped_chars_count = 0;
	int char_frequency[26] = { 0 };

	/* open & mmap() input file and find the number of characters */
	int input_fd = open(input_file, O_RDONLY);

	if (input_fd < 0) {
		perror("Error opening input file");
		exit(EXIT_FAILURE);
	}
	struct stat statbuf;

	fstat(input_fd, &statbuf);
	input_chars_size = statbuf.st_size;
	size_t map_size = input_chars_size * sizeof(char);

	input_chars = mmap(NULL, map_size, PROT_READ, MAP_SHARED, input_fd, 0);
	if (input_chars == MAP_FAILED) {
		close(input_fd);
		perror("Error mmapping the input file");
		exit(EXIT_FAILURE);
	}
	close(input_fd);

	/* Check input size */
	if (input_chars_size / n_threads < 1 ||
	    input_chars_size % n_threads != 0) {
		printf("Error: Number of characters in the input file should be a product of n_threads and a positive integer\n");
		printf("- n_threads: %d\n", n_threads);
		printf("- inputSize: %d\n", input_chars_size);
		exit(EXIT_FAILURE);
	}

	/* mmap() output file */
	int max_output_size = input_chars_size * sizeof(struct zipped_char);
	int output_fd;

	if (!debug) {
		output_fd = open(output_file, O_RDWR | O_CREAT | O_TRUNC,
				 (mode_t)0600);
		if (output_fd < 0) {
			perror("Error opening the output file");
			exit(EXIT_FAILURE);
		}

		/* Stretch the file size to the size of the (mmapped) array */
		map_size = max_output_size + 1;
		if (lseek(output_fd, map_size - 1, SEEK_SET) < 0 ||
		    write(output_fd, "", 1) < 0) {
			close(output_fd);
			perror("File operation error");
			exit(EXIT_FAILURE);
		}

		zipped_chars = mmap(NULL, map_size, PROT_READ | PROT_WRITE,
				    MAP_SHARED, output_fd, 0);
		if (zipped_chars == MAP_FAILED) {
			close(output_fd);
			perror("Error mmapping the output file");
			exit(EXIT_FAILURE);
		}
	} else {
		zipped_chars = malloc(max_output_size);
	}

	/* Call student's function */
	pzip(n_threads, input_chars, input_chars_size, zipped_chars,
	     &zipped_chars_count, char_frequency);

	if (!debug) {
		/* Truncate the file size back to the compressed size */
		if (ftruncate(output_fd,
			      zipped_chars_count * sizeof(struct zipped_char)) <
		    0) {
			close(output_fd);
			perror("ftruncate error");
			exit(EXIT_FAILURE);
		}
		close(output_fd);
	} else {
		/* Write zipped_chars to output file */
		FILE *output_fd = fopen(output_file, "w+");

		if (output_fd == NULL) {
			perror("Error opening the output file");
			exit(EXIT_FAILURE);
		}

		for (int i = 0; i < zipped_chars_count; i++) {
			struct zipped_char temp = zipped_chars[i];

			fprintf(output_fd, "%c %d\n", temp.character,
				temp.occurence);
		}
		fclose(output_fd);
		free(zipped_chars);
	}

	/* Print debug information */
	if (debug) {
		printf("DEBUG INFORMATION:\n");
		printf("- n_threads: %d\n", n_threads);
		printf("- inputSize: %d\n", input_chars_size);
		printf("- zipped_chars_count: %d\n", zipped_chars_count);
		printf("- char_frequency: ");
		for (int i = 0; i < ARRAY_SIZE(char_frequency); i++)
			printf("[%c,%d]", i + 97, char_frequency[i]);

		printf("\n");
		printf("- pzip output is written into the output file in plain text, type cat %s to display.\n",
		       output_file);
	}

	return 0;
}
