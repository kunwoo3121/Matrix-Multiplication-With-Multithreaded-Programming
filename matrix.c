#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<time.h>
#include"matrix.h"

typedef struct
{
	int **src1;
	int **src2;
	int **dst;
	int len;
	int start;
	int end;
}matmul_arg_t;

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
