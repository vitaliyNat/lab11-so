


#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <time.h>

#include <errno.h>
#include <math.h>


typedef  struct{
    float sum;
    HANDLE mutex;
} SharedD;

typedef struct {
    float * tab;
    float sum;
    int elements;
    DWORD id;
    SharedD *sharedData;
} ThreadR;

DWORD WINAPI calculate(LPVOID data){
    ThreadR* r = (ThreadR*) data;
    r->id = GetCurrentThreadId();
    printf("Thread %lu\t%d\t\n",r->id,r->elements);
    float rangeSum = 0.0;
    for(int i =0; i< r->elements;i++){
        rangeSum += r->tab[i];
    }
    r->sum = rangeSum;

    WaitForSingleObject(&r->sharedData->mutex,INFINITE);
    r->sharedData->sum +=rangeSum;
    ReleaseMutex(&r->sharedData->mutex);

    return 0;

}

    HANDLE mutex;

int main(int argc, char ** argv) {
    if(argc !=3){
        fprintf(stderr,"False input argument! Input must contain  Two arguments!\n");
        exit(0);
    }
    int len = strlen(argv[1]);
    if(len>25 || len < 1){
        fprintf(stderr,"False input argument! Input length must be from 1 to 25!\n");
        exit(0);
    }
    char * end;
    int numb1 = strtol(argv[1],&end,10);
    errno = 0;
    if(errno || *end){
        fprintf(stderr,"False first input argument! Input must included only numbers!\n");
        exit(0);
    }
    int numb2 = strtol(argv[2],&end,10);
    errno = 0;
    if(errno || *end){
        fprintf(stderr,"False second input argument! Input must included only numbers!\n");
        exit(0);
    }

    if(numb2 > numb1){
        fprintf(stderr,"False input arguments! Input iterations must be >= threads!\n");
        exit(0);
    }
    if(numb2 > 100){
        fprintf(stderr,"False input arguments! Number of threads must be smaller that 100!\n");
        exit(0);
    }
















    SharedD shared = {0.0 ,CreateMutex(NULL,FALSE,NULL)};
    HANDLE  threads[numb2];
    DWORD threadid[numb2];
    ThreadR ranges[numb2];
    int IterPerThread = floor(numb1/numb2);
    int IterPerLastThread = IterPerThread + (numb1 % numb2);
    int curr = 0;




    srand(0);
    float * arr = (float *) malloc(sizeof(float)*numb1);
    for(int i = 0; i<numb1;i++){
        arr[i] = 1000*rand()/RAND_MAX;
    }



    for(int i  = 0; i < numb2-1; i++){
        ranges[i].sharedData = &shared;
        ranges[i].tab = (float *) malloc(sizeof(float) * IterPerThread);
        ranges[i].elements = IterPerThread;
        for(int j = 0; j<IterPerThread;j++){
            ranges[i].tab[j] = arr[curr];
            curr++;
        }
    };

    ranges[numb2-1].sharedData = &shared;
    ranges[numb2-1].tab = (float *) malloc(sizeof(float) * IterPerLastThread);
    ranges[numb2-1].elements = IterPerLastThread;
    for(int j = 0; j<IterPerLastThread;j++){

        ranges[numb2-1].tab[j] = arr[curr];
        curr++;
    }

    for(int i = 0; i<numb2; i++){
    threads[i] = CreateThread(NULL,0,calculate,ranges+i,0,threadid+i);

    }


    clock_t startClock = clock();
    for(int i = 0; i<numb2;i++){

        WaitForSingleObject(threads[i], INFINITE);
        printf("Thread %lu\t%.20f\n",ranges[i].id,ranges[i].sum);
        CloseHandle(threads[i]);
    }

    clock_t endClock = clock();
    float seconds = (float)(endClock - startClock) / CLOCKS_PER_SEC;

    printf("wT  Pi = %.20f ; Time: %.6f\n",shared.sum,seconds);


    startClock = clock();
    float rangeSum = 0.0;
    for(int i = 0; i< numb1;i++){
        rangeSum += arr[i];
    }
    endClock = clock();
    seconds = (float)(endClock - startClock) / CLOCKS_PER_SEC;
    printf("woT  Pi = %.20f ; Time: %.10f\n",rangeSum,seconds);




    return 0;
}
