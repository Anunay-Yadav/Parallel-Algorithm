#include<stdio.h>
#include <stdlib.h> 
#include<pthread.h>
#include <time.h>
#include <assert.h>
const int size = 1e5;
const int no_of_threads = 4;
const int RUNS = 25;
int a[size],b[size];
pthread_mutex_t lock;
int part = -1,off;
void merge(int* arr, int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 =  r - m; 
    int L[n1], R[n2]; 
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1+ j]; 
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) 
    { 
        if (L[i] <= R[j]) 
        { 
            arr[k] = L[i]; 
            i++; 
        } 
        else
        { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
    while (i < n1) 
    { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
    while (j < n2) 
    { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
} 

void mergeSort(int* arr, int l, int r,int off) 
{ 
	if(r - l <= off)
		return;
    if (l < r) 
    { 
        int m = l+(r-l)/2; 
        mergeSort(arr, l, m,off); 
        mergeSort(arr, m+1, r,off); 
        merge(arr, l, m, r); 
    } 
} 
int min(int a,int b){
	if(a < b) return a;
	return b;
}
void* mergesort(void* f){
	int part = *((int*) f);
	mergeSort(b,part*off,min((part + 1)*off - 1,size-1),0);
	pthread_exit(NULL);
}

int offset(){
	int k = size;
	int mov = 1;
	while(2*mov <= no_of_threads){
		mov *= 2;
		k /= 2;
	}
	return k;
}
int main(){
	double mean = 0.0;
	double mean2 = 0.0;
	for (int i1 = 0; i1 < RUNS; ++i1)
	{
		clock_t start,end;
		off = offset();
		for (int i = 0; i < size; ++i)
		{
			a[i] = rand()%100;
			b[i] = a[i];
		}
		start = clock();
		mergeSort(a,0,size-1,0);
		end = clock();

		int parts[no_of_threads + 1];
		for (int i = 0; i < no_of_threads; ++i)
		{
			parts[i] = i;
		}

		// printf("time elapsed by mergesort seq: %f\n",double((end- start)) / double(CLOCKS_PER_SEC));
		double e1 =  double((end- start)) / double(CLOCKS_PER_SEC);
		mean += e1;
		start = clock();
		pthread_t pid[no_of_threads + 1];
		int cur = 0;
		for (int i = 0; i < size; i += off)
		{
			pthread_create(&pid[cur],NULL,mergesort,(void*) (parts + cur));
			cur++;
		}
		for (int i = 0; i < cur; ++i)
		{
			pthread_join(pid[i],NULL);
		} 
		mergeSort(b,0,size-1,off);
		end = clock();

		// printf("time elapsed by mergesort parrallel with %d threads: %f\n",no_of_threads,double((end- start)) / double(CLOCKS_PER_SEC));
		mean2 +=  double((end- start)) / double(CLOCKS_PER_SEC);
		for (int i = 0; i < size; ++i)
		{
			if(b[i] != a[i]) printf("%d\n", i);
			assert(b[i] == a[i]);
		}
		printf("%s %d\n", "check run : ",i1 + 1);
	}
	printf("time taken by seq avg (%d runs) : %f\n", RUNS,mean/(RUNS));
	printf("time taken by parrallel with with %d threads (%d runs) : %f\n",no_of_threads,RUNS ,mean2/(RUNS));
}