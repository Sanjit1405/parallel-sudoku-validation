#include <iostream>
#include <fstream>
#include <vector>
#include <pthread.h>
#include <atomic>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cmath>

using namespace std;

// parameters for thread function
struct parameters{   
	int threadID;
	int** sudoku;
};

ofstream output_file("output.txt");
atomic<bool> invalid(false);
int counter = 0;
int K, N, taskInc;
vector <double> CS_entry_times,CS_exit_times;
pthread_mutex_t output_mutex; // for outputting to the file safely
atomic <bool> lock_CAS(false); //for CAS implementation
bool waiting[32];

string getCurrentTime();
void row_check(int start,int end, parameters* p,stringstream &buffer);
void column_check(int start,int end, parameters* p,stringstream &buffer);
void subgrid_check(int start,int end, parameters* p,stringstream &buffer);
void* thread_function(void* arg);

int main(){
	double sum_entry_time = 0,sum_exit_time = 0,wc_entry_time = 0,wc_exit_time = 0;
	pthread_mutex_init(&output_mutex, nullptr); // Initialize mutex
    ifstream input_file("input.txt");
    input_file >> K >> N >> taskInc;

    for (size_t i = 0; i < 32; i++){
		waiting[i] = false;
    }
    

    int** sudoku = new int*[N];
    for (int i = 0; i < N; ++i){
        sudoku[i] = new int[N];
    }

    for (int i = 0; i < N; ++i){
        for (int j = 0; j < N; ++j) {
            input_file >> sudoku[i][j];
        }
    }
    input_file.close();

    pthread_t thread[K];
    parameters thread_parameters[K];

	auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < K; i++){
        thread_parameters[i].threadID = i + 1;
		thread_parameters[i].sudoku = sudoku;
        pthread_create(&thread[i], nullptr, thread_function, (void*)(&thread_parameters[i]));
    }

	for (int i = 0; i < K; i++){
		pthread_join(thread[i],nullptr);
	}

	auto end = chrono::high_resolution_clock::now();

	auto time_taken = chrono::duration_cast<chrono::microseconds>(end - start);
	
	if(invalid) output_file << "Sudoku is invalid" << '\n';
	else output_file << "Sudoku is valid" << '\n';

	for (int i = 0; i < CS_entry_times.size(); i++){
		sum_entry_time+=CS_entry_times[i];
		if(CS_entry_times[i] > wc_entry_time) wc_entry_time = CS_entry_times[i];
	}
	double avg_entry_time = sum_entry_time/CS_entry_times.size();

	for (int i = 0; i < CS_exit_times.size(); i++){
		sum_exit_time+=CS_exit_times[i];
		if(CS_exit_times[i] > wc_exit_time) wc_exit_time = CS_exit_times[i];
	}
	double avg_exit_time = sum_exit_time/CS_exit_times.size();

	output_file << "The total time taken is " << time_taken.count() << " us" << '\n';
	cout << "The total time taken is " << time_taken.count() << " us" << '\n';
	output_file << "Average time taken by a thread to enter the CS is " << avg_entry_time << " us" << '\n';
	cout << "Average time taken by a thread to enter the CS is " << avg_entry_time << " us" << '\n';
	output_file << "Average time taken by a thread to exit the CS is " << avg_exit_time << " us" << '\n';
	cout << "Average time taken by a thread to exit the CS is " << avg_exit_time << " us" << '\n';
	output_file << "Worst case time taken by a thread to enter the CS is " << wc_entry_time << " us" << '\n';
	cout << "Worst case time taken by a thread to enter the CS is " << wc_entry_time << " us" << '\n';
	output_file << "Worst case time taken by a thread to exit the CS is " << wc_exit_time << " us" << '\n';
	cout << "Worst case time taken by a thread to exit the CS is " << wc_exit_time << " us" << '\n';
	
	for (int i = 0; i < N; i++){
		delete[] sudoku[i];
	}
	delete[] sudoku;
	pthread_mutex_destroy(&output_mutex);
    return 0;
}

string getCurrentTime(){
    auto present = chrono::system_clock::now();
    time_t present_c = chrono::system_clock::to_time_t(present);
    tm* localTime = localtime(&present_c);
	auto ms = chrono::duration_cast<chrono::milliseconds>(present.time_since_epoch()) % 1000;
    stringstream ss;
    ss << put_time(localTime, "%H:%M:%S") << "." << setw(3) << setfill('0') << ms.count();
    return ss.str();
}

void row_check(int start,int end, parameters* p,stringstream &buffer){
    int id = p->threadID;
	int row = start;
	while(row != end + 1){
		// declare an int which tells about the validity of the row
		int flag = 1;
		// array to store whether the number has appeared or not
		int tracker[10000] = {0}; 
		for (int i = 0; i < N; i++){
			// check for repetition
			if(tracker[p->sudoku[row][i]-1] == 1){ 
				flag = 0;
				// set invalid to true 
				invalid = true;
				break;
			}
			else tracker[p->sudoku[row][i]-1] = 1;
		}
		if(flag == 1){
    		buffer << "Thread " << id << " completes checking of row " << row << " and finds it valid at " << getCurrentTime() << '\n';
		}
		else{
    		buffer << "Thread " << id << " completes checking of row " << row << " and finds it invalid at " << getCurrentTime() << '\n';
			break;
		}
		row++;
	}
}

void column_check(int start,int end, parameters* p,stringstream &buffer){
    int id = p->threadID;
	int col = start;
	while(col != end + 1){
		// declare an int which tells about the validity of the column
		int flag = 1; 
		// array to store whether the number has appeared or not
		int tracker[10000] = {0}; 
		for(int i = 0; i < N; i++){
			// check for repetition
			if(tracker[p->sudoku[i][col]-1] == 1){
				flag = 0;
				// set invalid to true
				invalid = true;
				break;
			}
			else tracker[p->sudoku[i][col]-1] = 1;
		}
		if(flag == 1){
    		buffer << "Thread " << id << " completes checking of column " << col << " and finds it valid at " << getCurrentTime() << '\n';
		}
		else{
    		buffer << "Thread " << id << " completes checking of column " << col << " and finds it invalid at " << getCurrentTime() << '\n';
    		break;
		}
		col++;
	}
}

void subgrid_check(int start,int end, parameters* p,stringstream &buffer){
    int id = p->threadID;
	int subgrid = start;
	int n = (int)(sqrt(N));
	while(subgrid != end + 1){
		// declare an int which tells about the validity of the subgrid
		int flag = 1; 
		// array to store whether the number has appeared or not
		int tracker[100] = {0}; 
		for (int i = n * (subgrid/n); i < n * (subgrid/n) + n; i++){
			for (int j = n * (subgrid % n); j < n * (subgrid % n) + n; j++){
				// check for repetition
				if(tracker[p->sudoku[i][j]-1] == 1){ 
					flag = 0;
					// set invalid to true
					invalid = true;
					break;
				}
				else tracker[p->sudoku[i][j]-1] = 1;
			}
			if(flag == 0) break;	
		}
		if(flag == 1){
    		buffer << "Thread " << id << " completes checking of subgrid " << subgrid << " and finds it valid at " << getCurrentTime() <<'\n';
		}
		else{
    		buffer << "Thread " << id << " completes checking of subgrid " << subgrid << " and finds it invalid at " << getCurrentTime() << '\n';
    		break;
		}
		subgrid++;
	}	
}

void* thread_function(void* arg){
    parameters *p = (parameters*)(arg);
    int id = p->threadID;
	stringstream buffer;
    while(true){
		if(counter >= 3 * N - 1 || invalid) break;
		string s = getCurrentTime();
    	buffer << "Thread " << id << " requests to enter CS at " << s << '\n';
		// CAS implementation
        // Entry Section
        waiting[id-1] = true;
        bool expected;
		auto start_entry = chrono::high_resolution_clock::now();
		do{
            expected = false;
        }while (waiting[id-1] && !lock_CAS.compare_exchange_strong(expected, true));

        //Critical Section
        waiting[id-1] = false; 
		auto end_entry = chrono::high_resolution_clock::now();
		auto time_taken_entry = chrono::duration_cast<chrono::microseconds>(end_entry - start_entry);
		CS_entry_times.push_back(time_taken_entry.count());
    	buffer << "Thread " << id << " enters CS at " << getCurrentTime() << '\n';
        
        //Check for value of counter,exit if counter >= total tasks-1,since counter starts from 0
        if (counter >= 3 * N - 1){
            // Special Exit Section if counter exceeds the total tasks
			int j = (id + 1) % K;
            while (j != id && !waiting[j-1]) j = (j + 1) % K;

            if (j == id) lock_CAS.store(false);
            else waiting[j-1] = false;

            break;
        }

        int pre = counter;
        counter += taskInc;
		int post = counter;

		if(post <= N) buffer << "Thread " << id << " grabs rows from " << pre << " to " << post-1 << " at " << getCurrentTime() << '\n';
		else if(post <= 2*N) buffer << "Thread " << id << " grabs columns from " << pre-N << " to " << post-N-1 << " at " << getCurrentTime() << '\n';
		else buffer << "Thread " << id << " grabs subgrids from " << pre-2*N << " to " << post-2*N-1 << " at " << getCurrentTime() << '\n';

        // Exit Section
        int j = (id + 1) % K;
        while (j != id && !waiting[j-1]) j = (j + 1) % K;

        if (j == id) lock_CAS.store(false);
        else waiting[j-1] = false;

		auto end_exit = chrono::high_resolution_clock::now();
		auto time_taken_exit = chrono::duration_cast<chrono::microseconds>(end_exit - end_entry);
		buffer << "Thread " << id << " exits CS at " << getCurrentTime() << '\n';
		pthread_mutex_lock(&output_mutex);
		CS_exit_times.push_back(time_taken_exit.count());
    	pthread_mutex_unlock(&output_mutex);

		if(invalid) break;
        
		if(N%taskInc == 0){
			if(post <= N) row_check(pre,post-1,p,buffer);
			else if(post > N && post <= 2*N) column_check(pre-N,post-1-N,p,buffer);
			else subgrid_check(pre-2*N,post-1-2*N,p,buffer);
		}
		else{
			if(post <= N) row_check(pre,post-1,p,buffer);
			else if(post > N && post <= 2*N) column_check(pre-N,post-1-N,p,buffer);
			else if(post - taskInc < N && post <= 2*N){
				row_check(pre,N-1,p,buffer);
				column_check(0,post - N-1,p,buffer);
			}
			else if(post > 2*N && post <= 3*N) subgrid_check(pre-2*N,post-1-2*N,p,buffer);
			else if(post - taskInc < 2*N && post <= 3*N){
				column_check(pre-N,N-1,p,buffer);
				subgrid_check(0,post - 2*N-1,p,buffer);
			}
			else subgrid_check(pre-2*N,N-1,p,buffer);
		}

	}
	pthread_mutex_lock(&output_mutex);
	output_file << buffer.str();
	pthread_mutex_unlock(&output_mutex);
    return nullptr;
}