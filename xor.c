/*

Perform bitwise XOR on binary files
Changes file2 such that file2new = file2old ^ file1

Written by Kevin Durette
	https://www.linkedin.com/in/kevindurette/
Original code by Philipp Hagemeister
	https://stackoverflow.com/questions/6889067/how-to-perform-bitwise-operations-on-files-in-linux
Licensed under Creative Commons CC BY-SA 2.0

Two argument syntaxes are supported.

Recommended build line:
gcc -Wall -Wextra -march=native -mtune=native -O3 -oxor xor.c

*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

int main(int argc, char* argv[]) {
	int FP1 = 0;
	int FP2 = 0;
	int FP3 = 0;
	struct stat St1;
	struct stat St2;
	struct stat St3;
	char *File1 = NULL;
	char *File2 = NULL;
	char *File3 = NULL;

	unsigned short int progressPercent = 0;

	ssize_t outSize = 0;

	if (argc != 4 && argc != 3) {
		printf("File XOR\n");
		printf("Syntax 1: %s File1 File2 File3\n", argv[0]);
		printf("Performs bitwise XOR on two files, writing result to File3.\n");
		printf("File3 := File1 ^ File2\n");
		printf("\n");
		printf("Syntax 2: %s File1 File2\n", argv[0]);
		printf("Performs bitwise XOR on two files, overwriting File2.\n");
		printf("File2new := File2old ^ File1\n");
		printf("File2 must be at least as large as File1.\n");
		exit(1);
	}

	for (int i = 0; i < 100; ++i) {
		printf("=");
	}
	printf("\n");

	if (argc == 4) {
		// Opening and mapping File1
		FP1 = open(argv[1], O_RDWR);
		if (FP1 == -1) {
			perror("Error opening file1 for writing");
			exit(2);
		}

		if (fstat(FP1, &St1) == -1) {
			perror("Can't determine file1 size");
			exit(3);
		}

		File1 = (char*) mmap(NULL, St1.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, FP1, 0);
		if (File1 == MAP_FAILED) {
			perror("Can't map file1");
			exit(4);
		}
		// /File1

		// Opening and mapping File2
		FP2 = open(argv[2], O_RDWR);
		if (FP2 == -1) {
			perror("Error opening file2 for writing");
			exit(2);
		}

		if (fstat(FP2, &St2) == -1) {
			perror("Can't determine file2 size");
			exit(3);
		}

		File2 = (char*) mmap(NULL, St2.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, FP2, 0);
		if (File2 == MAP_FAILED) {
			perror("Can't map file2");
			exit(4);
		}
		// /File2

		if (St2.st_size > St1.st_size) {
			outSize = St2.st_size;
		} else {
			outSize = St1.st_size;
		}

		// Zeroing out File3. We don't want a sparse file.
		FILE *fZero;
		fZero = fopen(argv[3], "w");
		progressPercent = 0;
		for (int i = 0; i < outSize; ++i) {
			fputc((char) 0, fZero);
			if ((100 * i / outSize) > progressPercent) {
				printf("0");
				fflush(stdout);
				++progressPercent;
			}
		}
		fclose(fZero);
		printf("0\n");

		// Opening and mapping File3
		FP3 = open(argv[3], O_RDWR, 0600);
		if (FP3 == -1) {
			perror("Error opening file3 for writing");
			exit(2);
		}

		if (fstat(FP3, &St3) == -1) {
			perror("Can't determine file3 size");
			exit(3);
		}


		if (outSize != St3.st_size) {
			perror("Size of file3 must be the same as the larger of file1 or file2.");
			exit(5);
		}

		File3 = (char*) mmap(NULL, St3.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, FP3, 0);
		if (File3 == MAP_FAILED) {
			perror("Cant map file3");
			exit(4);
		}
		// /File3


		// XOR
		ssize_t i = 0;
		if (St1.st_size <= St2.st_size) {
			progressPercent = 0;
			for (i = 0; i < St1.st_size; ++i) {
				File3[i] = File1[i] ^ File2[i];
				if ((100 * i / St1.st_size) > progressPercent) {
					printf(".");
					fflush(stdout);
					++progressPercent;
				}
			}
			printf(".\n");
			// Usual case is when the file sizes match, so don't bother with
			// progress on the top gap.
			for (i = St1.st_size; i < St2.st_size; ++i) {
				File3[i] = File2[i];
			}
		} else {
			progressPercent = 0;
			for (i = 0; i < St2.st_size; ++i) {
				File3[i] = File1[i] ^ File2[i];
				if ((100 * i / St2.st_size) > progressPercent) {
					printf(".");
					fflush(stdout);
					++progressPercent;
				}
			}
			printf(".\n");
			// Usual case is when the file sizes match, so don't bother with
			// progress on the top gap.
			for (i = St2.st_size; i < St1.st_size; ++i) {
				File3[i] = File1[i];
			}
		}
		// /XOR

		munmap(File1, St1.st_size);
		munmap(File2, St2.st_size);
		munmap(File3, St2.st_size);
		close(FP1);
		close(FP2);
		close(FP3);
		return 0;
	} // if (argc == 4)

	if (argc == 3) {
		// Opening and mapping File1
		FP1 = open(argv[1], O_RDWR);
		if (FP1 == -1) {
			perror("Error opening file1 for writing");
			exit(2);
		}

		if (fstat(FP1, &St1) == -1) {
			perror("Can't determine file1 size");
			exit(3);
		}

		File1 = (char*) mmap(NULL, St1.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, FP1, 0);
		if (File1 == MAP_FAILED) {
			perror("Can't map file1");
			exit(4);
		}
		// ======================

		// Opening and mapping File2
		FP2 = open(argv[2], O_RDWR);
		if (FP2 == -1) {
			perror("Error opening file2 for writing");
			exit(2);
		}

		if (fstat(FP2, &St2) == -1) {
			perror("Can't determine file2 size");
			exit(3);
		}

		File2 = (char*) mmap(NULL, St2.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, FP2, 0);
		if (File2 == MAP_FAILED) {
			perror("Can't map file2");
			exit(4);
		}
		// ======================

		// Do XOR
		ssize_t i = 0;
		if (St1.st_size <= St2.st_size) {
			for (i = 0; i < St1.st_size; i ++) {
				File2[i] = File1[i] ^ File2[i];
				if ((100 * i / St1.st_size) > progressPercent) {
					printf(".");
					fflush(stdout);
					++progressPercent;
				}
			}
			printf(".\n");
		} else {
			perror("The larger file should be the second argument.");
			munmap(File1, St1.st_size);
			munmap(File2, St2.st_size);
			close(FP1);
			close(FP2);
			exit(5);
		}
		// ======================

		munmap(File1, St1.st_size);
		munmap(File2, St2.st_size);
		close(FP1);
		close(FP2);
		return 0;
	} // if (argc == 3)
}
