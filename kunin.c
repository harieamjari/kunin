// Written by Al-buharie Amjari
// Copyright (C) 2019, Project Kunin

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>
#include <stddef.h>

#include "wavfile.h"

#define _USE_MATH_DEFINES

int __SUM;
int iteration = 50;
int b = 0;

void err(){printf("Usage: [options] [outfile]\nrun \"kunin -h\"\n");}

void option_dec(){
    printf("-f                   frequency (default = 100)\n"
           "-v                   volume (default = 2200)\n");
}
void summation(int volume, double frequency, double t){
    for (int n = 0; n < iteration; n++){
        double partialSUM = (((2*(1-(pow(-1,n)))))/n*M_PI)*(volume*sin(frequency*t*n*2*M_PI));
        //printf(" SUB SUM --> %d\n", sub_data);
        __SUM += partialSUM;
    }
}
double fq_val[] = {
	130.81, 146.83, 164.81, 174.61, 196.0,  220.0,  246.94, -1
};
char *fq_key =
	"C"     "D"     "E"     "F"     "G"     "A"     "B";
static double get_fq(char *key){
	return fq_val[strcspn(fq_key, key)];
}

int main(int argc, char *argv[]){

    if (argc == 1){
        err();
        exit(1);
    }
	char *filename;
	filename = NULL;

	int duration = 2;
	double frequency = 150.0;
	double BPM = 123.0;
	int volume = 2200;
	
	int opt;
	int option_index = 0;

    static struct option long_options[] =
        {
          /* These options set a flag. */
        {"help", no_argument,       0, 'h'},
        {"frequency",   required_argument,       0, 1},
        {"volum",     required_argument,       0, 2},
        {"output",     required_argument,       0, 'o'},
		{"iterate",     required_argument,       0, 0},
		{"duration", required_argument, 0, 3},
        {0, 0, 0, 0}
        };

    while ((opt = getopt_long(argc, argv, "f:v:o:hd:0:", long_options, &option_index)) != -1) {
		printf("%d\n", opt);
        switch (opt) {
		case 0:
			iteration = atoi(optarg);
			break;
        case 1:		
			if (strlen(optarg) == 1){

				frequency = get_fq(optarg);
				printf("%.2f\n", get_fq(optarg));
				break;			
			}
			else{
				frequency = atoi(optarg);
				printf("%.2f\n", frequency);
				break;
			}
			
            break;
        case 2:
            volume = atoi(optarg);
            break;
        case 'o':
            filename = optarg;
            break;
        case 'h':
            err();
            option_dec();
            exit(1);
            break;
		case 3:
	    duration = atoi(optarg);
	    break;
        case '?': /* '?' */
            err();
            exit(EXIT_FAILURE);
        }
    }

	const int NUM_SAMPLES = (WAVFILE_SAMPLES_PER_SECOND*duration);
    int length = NUM_SAMPLES;
	short waveform[NUM_SAMPLES];

	for(int x = 0; x < length; x++) {
		double t = ((double) x / WAVFILE_SAMPLES_PER_SECOND);	
		summation(volume, frequency, t);
		waveform[ x ] = __SUM;
		int progress = ( 900 )*b;
		//printf("p=%d b=%d\n", progress, b);
		if (progress == x){
			b++;
			printf("Writing: %d/%d amplitude=\"%d\"\n", x, length, __SUM);
		}
		__SUM = 0;
	}

	for (int p = 0; p < 3; p++){
		printf("Value at %d is %d\n", p, waveform[p]);
	}
	if (filename == NULL){
		filename = argv[argc-1];
	}


	FILE * f = wavfile_open(filename);
	if(!f) {
		printf("couldn't create wavfile for writing: %s",strerror(errno));
		return 1;
	}
	wavfile_write(f,waveform,length);
	wavfile_close(f);
	printf("Done Writing!!\n");
	free(f);
	free(filename);
	return 0;
}
