#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <pthread.h>

struct quicksortData{
  int low;
  int high;
  int arrLen;
  int array[];
};

/*Prototypes*/
void swap(int*, int*);
int partition(int*, int*, int*);
void* qsort2(void*);
void quicksort(int*, int, int);
void printArray(int*, int);

void swap(int *a, int *b)
{
  int t = *a;
  *a = *b;
  *b = t;
}

int partition(int array[], int *low, int *high)
{
  int pivot = array[*high];
  int i = (*low -1);

  for(int j=*low;j<*high;j++)
  {
    if (array[j] <= pivot)
    {
      i++;
      swap(&array[i], &array[j]);
    }
  }
  swap(&array[i+1], &array[*high]);
  return (i+1);
}

void *qsort2(void *data)
{
  struct quicksortData *qd = data;
  int low = qd->low;
  int high = qd->high;
  int *arr;
  /*int size = sizeof(qd->array)/sizeof(qd->array[0]);*/
  int size = qd->arrLen;
  arr = malloc(size * sizeof(int));
  memcpy(arr, qd->array, size);
  quicksort(arr, low, high);
  
  free(arr);
  return NULL;
}

void quicksort(int array[], int low, int high)
{
  if (low < high)
  {
    int pi = partition(array, &low, &high);
    quicksort(array, low, pi -1);
    quicksort(array, pi+1, high);
  }
}

void printArray(int array[], int size)
{
  for(int i=0;i<size;i++)
  {  printf("%d ", array[i]);  }
  printf("\n");
}

int main()
{
  int data[] = {8,7,2,1,0,9,6};
  int n = sizeof(data)/sizeof(data[0]);
  int ncores = get_nprocs();
  pthread_t pids[ncores];
  struct quicksortData qsd;

  memcpy(qsd.array, data, sizeof(data));
  qsd.low = 0;
  qsd.high = n;
  qsd.arrLen = sizeof(data)/sizeof(data[0]);

  /*Get number of cores, div by 2 to get actual cores.
    In loop, each time create threads, do quicksort, then >>=1
    Until down to 1 thread, then exit loop*/
    ncores = get_nprocs();
  while (ncores > 1)
  {
    for(int i=0; i<=ncores;i++)
    {
      qsd.low = (int)((i*n)/ncores);
      qsd.high = (int)(((i+1)*n)/ncores);
      qsd.high = (qsd.high > n) ? n : qsd.high;
      printf("%d threads, %d low, %d high\n", ncores, qsd.low, qsd.high);
      pthread_create(&pids[i], NULL, &qsort2, &qsd);
    }
    for(int i=0; i<=ncores;i++)
    {
      pthread_join(pids[i], NULL);
    }
    ncores >>= 1;
  }

  printArray(data, n);
  /*quicksort(data,0,n-1);*/
  
  printArray(data, n);

  return 0;
}

