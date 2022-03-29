#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "image.h"
#include "ppm.h"
#include "texture_synthesis.h"

int main( int argc , char *argv[] )
{
	// Get the time at the start of execution
	clock_t start_clock = clock();

	//check the correct number of arguments
	if(argc!=6){
		printf("ERROR: Incorrect number of input provided!\n");
		return 1;
	}

	//check whether the input file works
	FILE* read = fopen(argv[1],"r");
	if(!read || ferror(read)){
		printf("ERROR: Input file IO error!\n");
		return 2;
	}

	//check whether the output file works
	FILE* write = fopen(argv[2],"w");
	if(!write || ferror(write)){
		printf("ERROR: Output file IO error!\n");
		return 4;
	}
	// Seed the random number generator so that the code produces the same results on the same input.
	srand(14);

	Image * img = ReadPPM(read);
	if(!img){
		printf("ERROR: The Input file cannot be read as a PPM file!\n");
		return 3;
	}

	// Read outWidth, outHeight, windowRadius from cmd line
	char *next;
	int outWidth = (int) strtol(argv[3], &next, 10);
	int outHeight = (int) strtol(argv[4], &next, 10);
	int windowRadius = (int) strtol(argv[5], &next, 10);

	if (!outWidth || !outHeight || !windowRadius) {
		printf("ERROR: The width or height or windowRadius of the output image are incorrectly specified!\n");
		return 5;
	}


	// Call SynthesizeFromExemplar
	Image * synthesized = SynthesizeFromExemplar(img, outWidth, outHeight, windowRadius);
	WritePPM(write, synthesized);
	fclose(read);
	fclose(write);
	FreeImage(&synthesized);
	FreeImage(&img);
	

	// Get the time at the end of the execution
	clock_t clock_difference = clock() - start_clock;

	// Convert the time difference into seconds and print
	printf( "Synthesized texture in %.2f(s)\n" , (double)clock_difference/CLOCKS_PER_SEC );
	return 0;
}

