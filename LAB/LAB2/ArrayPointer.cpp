#include<iostream>

using namespace std;

int main(){
	int array [10];
	array[0] = 1;
	int *arraypointer [10];
	arraypointer[0] = array;
	cout << *(arraypointer[0] + 1); 
} 
