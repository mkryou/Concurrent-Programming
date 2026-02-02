/* matrix summation using OpenMP
usage with gcc (version 4.2 or higher required):
gcc -O -fopenmp -o queensMP-openmp queensMP.c
./matrixSum-openmp size numWorkers
*/
#include <omp.h>
#include <stdio.h>
#include <stdbool.h>
#define NUMWORKERS 1 /* maximum number of workers */
#define MAXSIZE 8
double start_time, end_time;



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
    if (row == MAXSIZE)
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

int main(){
    int total = 0;
    double start_time, end_time;
    omp_set_num_threads(NUMWORKERS);

    start_time = omp_get_wtime();

    #pragma omp parallel for reduction(+:total)
        for(int col = 0; col < MAXSIZE; col++){
            int matrix[MAXSIZE][MAXSIZE] = {0};
            matrix[0][col] = 1;
            total += placeQueens(1, matrix);
        }
    end_time = omp_get_wtime();
    printf("Total solutions: %d\n", total);
    printf("it took %g seconds\n", end_time - start_time);
}