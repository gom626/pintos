#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "syscall.h"
#include "../lib/user/syscall.h"

int main(int argc, char *argv[]){
	
	int a=atoi(argv[1]);
	int b=atoi(argv[2]);
	int c=atoi(argv[3]);
	int d=atoi(argv[4]);
//	printf("%d %d %d %d",a,b,c,d);	
	printf("%d %d\n",fibonacci(a),sum_of_four_integers(a,b,c,d));
}
