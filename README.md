# Matrix-Multiplication-With-Multithreaded-Programming

목표 : 쓰레드를 활용하여 행렬곱셈에 걸리는 시간을 줄인다. 
  
원하는 행렬의 크기와 생성할 쓰레드의 수를 입력받아서 행렬곱셈을 하는 프로그램을 만든다.

프로그래밍은 WSL2 에서 진행하였다.

# main 함수
```c
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
```
원하는 행렬의 크기와 쓰레드 수를 입력받는다. 입력하지 않을 경우 기본 크기로 4, 쓰레드 수는 12개로 지정한다.  
  
입력받은 크기의 행렬을 3개 생성한다. 

쓰레드를 사용하지 않은 행렬 곱셈의 시간을 측정하고 쓰레드를 사용한 행렬 곱셈의 시간을 측정하여 비교한다.

# matrix.c
## init (행렬 생성 및 초기화)
```c
void init(int*** a, int len)
{

	int i, j;

	*a = (int**)malloc(len * sizeof(int*));

	for( i = 0; i < len; i++ )
	{
		(*a)[i] = (int*)malloc(len * sizeof(int));
	}

	for( i = 0; i < len; i++)
	{
		for( j = 0 ; j < len; j++)
		{
			(*a)[i][j] = random() % 10;
		}
	}
}
```
행렬을 입력 받은 길이 만큼의 크기로 생성하고 랜덤으로 값을 부여한다.

