#include <stdio.h>

int fib(int n);

int main(){
	int n, result;
	printf("Please input a number: \n");
	scanf("%d", &n);
	result = fib(n);
	printf("The final result is:  %d\n", result);
}

int fib(int n){
	int i, j;
	if (n < 2)
		return n;
	else{
		#pragma omp task shared(i)
			i = fib(n-1);
		#pragma omp task shared(j)
			j = fib(n-2);
		#pragma omp taskwait
			return i+j;
	}	
}
