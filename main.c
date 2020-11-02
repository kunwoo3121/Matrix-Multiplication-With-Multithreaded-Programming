#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/time.h>
#include"matrix.h"

double wtime();

int main(int argc, char** argv)
{
	int **a;
	int **b;
	int **c;
	int i, j, k;
	int len;
	int core;
	double start, stop;

	if(argc == 3)
	{
		len = atoi(argv[1]);
		core = atoi(argv[2]);
	}

	else
	{
		len = 4;
		core = 12;
	}

	srandom((unsigned int)time(NULL));
	
	init(&a, len);
	init(&b, len);
	init(&c, len);

//	print_matrix(a, "A", len);
//	print_matrix(b, "B", len);

	start = wtime();
	
	if( mat_mul(a, b, c, len) != 0 )
	{
		fprintf(stderr,"Failed to MAtrix multiplication\n");
		exit(1);
	}
	
	stop = wtime();
	
//	print_matrix(c, "C", len);
	
	printf("Processing time : %f\n", (stop - start));

	start = wtime();
	
	if( mat_mul_th(a, b, c, len, core) != 0 )
	{
		fprintf(stderr,"Failed to MAtrix multiplication\n");
		exit(1);
	}
	
	stop = wtime();

//	print_matrix(c, "C", len);
	
	free(a);
	free(b);
	free(c);

	printf("Processing time : %f\n", (stop - start));

	return 0;
}

double wtime()
{
	static int sec = -1;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	if(sec < 0) sec =tv.tv_sec;

	return(tv.tv_sec - sec) + 1.0e-6 * tv.tv_usec;
}
