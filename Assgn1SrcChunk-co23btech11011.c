#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <pthread.h>


void *row_check(void * arg); // thread functions to check validity of rows,columns and subgrids
void *col_check(void * arg); 
void *subGrid_check(void * arg); 

int N,K,K1,K2,K3; 
int valid = 1;  // to determine whether the sudoku is valid or not
int *arr;

typedef struct{    // data to be passed to the thread functions
	int threadID;
	int start; //start index(row or column or subgrid)
	int end; //end index(row or column or subgrid)
	int** sudoku;
	char buffer[4000];
}parameters;

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
	fclose(i_f); // close the input file

	K1 = K/3;    // divide threads equally for rows,columns and subgrids 
	K2 = K/3;
	K3 = K - (K1 + K2);
	
	start = clock(); // start the clock
	
	pthread_t rowThread[K1];  // initialisation and creation of row threads
	parameters rowp[K1];
	for (int i = 0; i < K1; i++){
		rowp[i].threadID = i+1; 
		rowp[i].start = i * (N/K1);     //assign the work to the row threads in chunks where chunk size is N/K1
		if(i == K1-1) rowp[i].end = N-1;
		else rowp[i].end = (i+1) * N/K1 - 1;
		rowp[i].sudoku = ptr;
		memset(rowp[i].buffer, 0, sizeof(rowp[i].buffer));
		pthread_create(&rowThread[i], NULL, row_check, (void*)(&rowp[i]));
	}
	pthread_t colThread[K2]; // initialisation and creation of column threads
	parameters colp[K2];
	for (int i = 0; i < K2; i++){
		colp[i].threadID = i+1+K1;
		colp[i].start = i * (N/K2);
		if(i == K2-1) colp[i].end = N-1;  //assign the work to the column threads in chunks where chunk size is N/K2
		else colp[i].end = (i+1) * N/K2 - 1;
		colp[i].sudoku = ptr;
		memset(colp[i].buffer, 0, sizeof(colp[i].buffer));
		pthread_create(&colThread[i], NULL, col_check, (void*)(&colp[i]));
	}
	pthread_t subGridThread[K3];  // initialisation and creation of subgrid threads
	parameters subGridp[K3];
	for (int i = 0; i < K3; i++){
		subGridp[i].threadID = i+1+K1+K2;
		subGridp[i].start = i * (N/K3);  //assign the work to the subgrid threads in chunks where chunk size is N/K3
		if(i == K3-1) subGridp[i].end = N-1;
		else subGridp[i].end = (i+1) * N/K3 - 1;
		subGridp[i].sudoku = ptr;
		memset(subGridp[i].buffer, 0, sizeof(subGridp[i].buffer));
		pthread_create(&subGridThread[i], NULL, subGrid_check, (void*)(&subGridp[i]));
	}

	for (int i = 0; i < K1; i++) { // joining the threads after their completion
    	pthread_join(rowThread[i], NULL);
    	fputs(rowp[i].buffer, o_f); // output the buffer
	}
	for (int i = 0; i < K2; i++) {
    	pthread_join(colThread[i], NULL);
    	fputs(colp[i].buffer, o_f); //output the buffer

	}
	for (int i = 0; i < K3; i++) {
    	pthread_join(subGridThread[i], NULL);
    	fputs(subGridp[i].buffer, o_f); //output the buffer
	}
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
void *row_check(void * arg){
	parameters *p = (parameters*)(arg);
	int index = 0;
	int row = p->start;
	while(row != p->end + 1){
		int flag = 1; // tracks whether the row is valid or not
		int tracker[100] = {0}; // array to store 1 or 0, 1 if a number is there 0 if its not
		for (int i = 0; i < N; i++){
			if(tracker[p->sudoku[row][i]-1] == 1){ // if there is a duplicate stop checking that row update the value of valid
				flag = 0;
				break;
			}
			else tracker[p->sudoku[row][i]-1] = 1;
		}
		if(flag == 1){
			arr[row] = 1;
			index += sprintf(p->buffer + index, "Thread %d checks row %d and is valid\n", p->threadID, row + 1); // index tracks where the next character is to be stored
		}
		else{
			arr[row] = 0;
			index += sprintf(p->buffer + index, "Thread %d checks row %d and is invalid\n", p->threadID, row + 1);
		}
		row++;
	}
	return NULL;
}
void *col_check(void * arg){
	parameters *p = (parameters*)(arg);
	int index = 0;
	int col = p->start;
	while(col != p->end + 1){
		int flag = 1; // tracks whether the column is valid or not
		int tracker[100] = {0}; // array to store 1 or 0, 1 if a number is there 0 if its not
		for (int i = 0; i < N; i++){
			if(tracker[p->sudoku[i][col]-1] == 1){ // if there is a duplicate stop checking that column update the value of valid
				flag = 0;
				break;
			}
			else tracker[p->sudoku[i][col]-1] = 1;
		}
		if(flag == 1){
			arr[col + N] = 1;
			index += sprintf(p->buffer + index, "Thread %d checks column %d and is valid\n", p->threadID, col + 1); // index tracks where the next character is to be stored
		}
		else{
			arr[col + N] = 0;
			index += sprintf(p->buffer + index, "Thread %d checks column %d and is invalid\n", p->threadID, col + 1);
		}
		col++;
	}
	return NULL;
}
void *subGrid_check(void * arg){
	parameters *p = (parameters*)(arg);
	int index = 0;
	int subGrid = p->start;
	int n = (int)(sqrt(N));
	while(subGrid != p->end + 1){
		int flag = 1; // tracks whether the subgrid is valid or not
		int tracker[100] = {0}; // array to store 1 or 0, 1 if a number is there 0 if its not
		for (int i = n * (subGrid/n); i < n * (subGrid/n) + n; i++){
			for (int j = n * (subGrid % n); j < n * (subGrid % n) + n; j++){
				if(tracker[p->sudoku[i][j]-1] == 1){ // if there is a duplicate stop checking that subgrid update the value of valid
					flag = 0;
					break;
				}
				else tracker[p->sudoku[i][j]-1] = 1;
			}
			if(flag == 0) break;	
		}
		if(flag == 1){
			arr[subGrid + 2*N] = 1;
			index += sprintf(p->buffer + index, "Thread %d checks subgrid %d and is valid\n", p->threadID, subGrid + 1); // index tracks where the next character is to be stored
		}
		else{
			arr[subGrid + 2*N] = 0;
			index += sprintf(p->buffer + index, "Thread %d checks subgrid %d and is invalid\n", p->threadID, subGrid + 1);
		}
		subGrid++;
	}
	return NULL;
}