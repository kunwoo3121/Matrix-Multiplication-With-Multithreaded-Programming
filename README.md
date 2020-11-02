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

## mat_mul ( 행렬의 곱셈 / 쓰레드 미사용 )
```c
int mat_mul(int** src1, int** src2, int** dst, int len)
{
	int i, j ,k;
	int result;

	for( i = 0; i < len; i++ )
	{
		for( j = 0; j < len; j++)
		{
			result = 0;
			for ( k = 0; k < len; k++ )
			{
				result += src1[i][k] * src2[k][j];
			}
			dst[i][j] = result;
		}
	}
	
	return 0;
}
```
쓰레드를 사용하지 않고 코어 1개만 사용하여 행렬 곱셈을 하는 함수이다.

## mat_mul_th ( 행렬의 곱셈 / 쓰레드 사용 )
```c
typedef struct
{
	int **src1;
	int **src2;
	int **dst;
	int len;
	int start;
	int end;
}matmul_arg_t;

int mat_mul_th(int** src1, int** src2, int** dst, int len, int core)
{
	int i,res,len2,len_r,core2,s,e;
	matmul_arg_t *arg;
	pthread_t *a_thread;
	void *thread_result;

	a_thread = (pthread_t*)malloc(sizeof(pthread_t) * core);
	arg = (matmul_arg_t*)malloc(sizeof(matmul_arg_t) * core);

	len2 = len;
	core2 = core;
	s = 0;

	for( i = 0; i < core; i++)
	{
		len_r = len2/core2;
		e = s + len_r;

		arg[i].start = s;
		arg[i].end = e;
		arg[i].src1 = src1;
		arg[i].src2 = src2;
		arg[i].dst = dst;
		arg[i].len = len;
		res = pthread_create(a_thread + i, NULL, mat_mul_th_kernel, (void*)(arg + i));
		if(res != 0)
		{
			perror("Thread creation failed");
			exit(EXIT_FAILURE);
		}

		s = e;
		len2 -= len_r;
		core2--;
	}

	for( i = 0; i < core; i++ )
	{
		res = pthread_join(*(a_thread + i), &thread_result);
		if(res != 0)
		{
			perror("Thread join failed");
			exit(EXIT_FAILURE);
		}
	}

	free(a_thread);
	free(arg);

	return 0;
}
```
입력받은 만큼의 쓰레드를 생성하고 그 결과를 모아주는 함수이다.  
  
행렬 곱셈을 하는 핵심 함수는 mat_mul_th_kernel로 만들어준다.

## mat_mul_th_kernel
```c
void *mat_mul_th_kernel(void *arg)
{
	int i, j, k;
	matmul_arg_t *parg = (matmul_arg_t*)arg; 
	int start = parg->start;
	int end = parg -> end;
	int **src1 = parg->src1;
	int **src2 = parg->src2;
	int **dst = parg->dst;
	int len = parg->len;
	int result;

	for( i = start; i < end; i++)
	{
		for( j = 0; j < len; j++)
		{
			result = 0;
			for ( k = 0; k < len; k++ )
			{
				result += src1[i][k] * src2[k][j];
			}
			dst[i][j] = result;
		}
	}
	
	return 0;
}
```
mat_mul_th에서 행렬 곱셈을 시행하는 핵심 함수이다.

## print_matrix
```c
void print_matrix(int** matrix, char* c, int len)
{
	int i, j;

	printf("============= %s matrix =====================\n",c);

	for( i = 0; i < len; i++ )
	{
		for( j = 0; j < len; j++)
		{
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}
}
```
행렬을 출력해주는 함수이다.
