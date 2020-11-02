# Matrix-Multiplication-With-Multithreaded-Programming

목표 : 쓰레드를 활용하여 행렬곱셈에 걸리는 시간을 줄인다. 
  
원하는 행렬의 크기와 생성할 쓰레드의 수를 입력받아서 행렬곱셈을 하는 프로그램을 만든다.

## main 함수

# 입력
```c
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
```
