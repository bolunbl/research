#include <stdio.h>
#include <math.h>
#include <list>

using namespace std;

//typedef list <long> llist;

int main(int argc, char **argv){
	list <int> l;			
	int sum = 0;
	for(int i=0; i<10; i++){
		sum += i;
		//l.push_back(sum);
	}
	printf("%d\n", sum);
}


