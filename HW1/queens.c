//Hej bananna
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#define QUEENS 8
#define NUM_WORKERS 8
#define MAXSIZE 8
int numArrived = 0;
int sums[NUM_WORKERS]; /* partial sums */
double start_time, end_time;


pthread_mutex_t barrier;
pthread_cond_t go;
void *Worker(void *);

void Barrier() {
pthread_mutex_lock(&barrier);
    numArrived++;
    if (numArrived == NUM_WORKERS) {
        numArrived = 0;
        pthread_cond_broadcast(&go);
    } else
        pthread_cond_wait(&go, &barrier);
    pthread_mutex_unlock(&barrier);
}


// Timing functions
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if (!initialized) {
        gettimeofday(&start, NULL);
        initialized = true;
    }
    gettimeofday (&end, NULL);
    // tv_sec: seconds
    // tv_usec: microseconds
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

int main(int argc, char *argv[]) {
    pthread_t workerid[NUM_WORKERS];
    pthread_attr_t attr;

    /* set global thread attributes */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    /* initialize mutex and condition variable */
    pthread_mutex_init(&barrier, NULL);
    pthread_cond_init(&go, NULL);
    /* starting timer */
    start_time = read_timer();

    /*Create workers */
    for (long l = 0; l < NUM_WORKERS; l++){
        pthread_create(&workerid[l], &attr, Worker, (void *) l);
    }
        pthread_exit(NULL);
}

bool isSafe(int row, int col, int matrix[MAXSIZE][MAXSIZE]) { 
    /* Check column above */
    for (int i = 0; i < row; i++) {
        if(matrix[i][col] == 1)
            return false;
    }
    /* Left diagonal check */
    for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--) {
        if (matrix[i][j] == 1)
            return false;
    }
    /* Right diagonal check */
    for (int i = row - 1, j = col + 1; i >= 0 && j < MAXSIZE; i--, j++) {
        if (matrix[i][j] == 1)
            return false;
    }
    return true;
}

int placeQueens(int row, int matrix[MAXSIZE][MAXSIZE]){
    if (row == 8)
        return 1;

        int count = 0;

    for(int col = 0; col < MAXSIZE; col++){
        if (isSafe(row, col, matrix)){
            matrix[row][col] = 1;
            count += placeQueens(row + 1, matrix);
            matrix[row][col] = 0;
        }
    }
    return count;
}

void *Worker(void *arg){
    long myid = (long) arg;
    int total, i;
    int matrix[MAXSIZE][MAXSIZE] = {0};
    #ifdef DEBUG
       printf("worker %ld (pthread id %lu) has started\n", myid, (unsigned long)pthread_self());
    #endif
    matrix[0][myid] = 1;

    sums[myid] = placeQueens(1, matrix);
    Barrier();
    if (myid == 0) { 
        end_time = read_timer();
        total = 0;
        for (i = 0; i < NUM_WORKERS; i++){
            printf("Worker %d found %d solutions\n", i, sums[i]);
            total += sums[i];
        }
        printf("Total solutions: %d\n", total);
         printf("The execution time is %g sec\n", end_time - start_time);
    }

    //print execution time + number of solutions found by workers
    return NULL;
}

