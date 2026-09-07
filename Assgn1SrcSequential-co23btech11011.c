#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>


void row_check(int ** sudoku,int size); // functions to check validity of rows,columns and subgrids
void col_check(int ** sudoku,int size); 
void subGrid_check(int ** sudoku,int size); 

int N,K;
int * arr;
int valid = 1;  // to determine whether the sudoku is valid or not


int main(){
	clock_t start, end;
    double time_taken;

	FILE* i_f = fopen("input.txt","r");   // open the files
	FILE* o_f = fopen("output.txt","w");
	
	fscanf(i_f,"%d %d", &K, &N);
	arr = (int*)(malloc(sizeof(int) * 3 * N));
	int **ptr = (int **)malloc(N * sizeof(int *)); //allocate memory for the sudoku
	for (int i = 0; i < N; i++) {
        ptr[i] = (int *)malloc(N * sizeof(int));
    }


	for (int i = 0; i < N; i++){  // read the sudoku from the input file
		for (int j = 0; j < N; j++){
			fscanf(i_f,"%d", &ptr[i][j]);
		}
	}
	fclose(i_f);

	start = clock(); // start the clock

	row_check(ptr,N); 
	col_check(ptr,N); 
	subGrid_check(ptr,N); 

	for (int i = 0; i < 3 * N; i++){
	 	if(arr[i] == 0){
	 		printf("Sudoku is invalid\n");
			fprintf(o_f,"Sudoku is invalid\n");
			valid = 0;
			break;
	 	}
	 }
	 if(valid == 1){
		printf("Sudoku is valid\n");
		fprintf(o_f,"Sudoku is valid\n");
	}
	
	free(arr);
	for (int i = 0; i < N; i++) { //free the dynamically allocated memory
        free(ptr[i]);
    }
    free(ptr);

    end = clock(); //end the clock
    time_taken = (double)(end - start)/CLOCKS_PER_SEC;
    printf("The total time taken is %f microseconds\n",time_taken * 1e6);
	fprintf(o_f,"The total time taken is %f microseconds\n",time_taken * 1e6);


    fclose(o_f);

	return 0;
}
void row_check(int ** sudoku,int size){
	int row = 0;
	while(row < N){
		int flag = 1; // tracks whether the row is valid or not
		int tracker[100] = {0}; // array to store 1 or 0, 1 if a number is there 0 if its not
		for (int i = 0; i < N; i++){
			if(tracker[sudoku[row][i]-1] == 1){ // if there is a duplicate stop checking that row update the value of valid
				flag = 0;
				break;
			}
			else tracker[sudoku[row][i]-1] = 1;
		}
		if(flag == 1) arr[row] = 1;
		else arr[row] = 0;

		row++;
	}
}
void col_check(int ** sudoku,int size){
	int col = 0;
	while(col < N){
		int flag = 1; // tracks whether the column is valid or not
		int tracker[100] = {0}; // array to store 1 or 0, 1 if a number is there 0 if its not
		for (int i = 0; i < N; i++){
			if(tracker[sudoku[i][col]-1] == 1){ // if there is a duplicate stop checking that column update the value of valid
				flag = 0;
				break;
			}
			else tracker[sudoku[i][col]-1] = 1;
		}
		if(flag == 1) arr[col + N] = 1;
		else arr[col + N] = 0;
		
		col++;
	}
}
void subGrid_check(int ** sudoku,int size){
	int subGrid = 0;
	int n = (int)(sqrt(N));
	while(subGrid < N){
		int flag = 1; // tracks whether the subgrid is valid or not
		int tracker[100] = {0}; // array to store 1 or 0, 1 if a number is there 0 if its not
		for (int i = n * (subGrid/n); i < n * (subGrid/n) + n; i++){
			for (int j = n * (subGrid % n); j < n * (subGrid % n) + n; j++){
				if(tracker[sudoku[i][j]-1] == 1){ // if there is a duplicate stop checking that subgrid update the value of valid
					flag = 0;
					break;
				}
				else tracker[sudoku[i][j]-1] = 1;
			}
			if(flag == 0) break;	
		}
		if(flag == 1) arr[subGrid + 2*N] = 1;
		else arr[subGrid + 2*N] = 0;
		subGrid++;
	}
}