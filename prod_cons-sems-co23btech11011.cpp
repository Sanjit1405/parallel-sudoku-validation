#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <random>
#include <iomanip>
#include <thread>
#include <pthread.h>
#include <semaphore.h>

/*declare a buffer and
declare and two ints that tell the location to put or get(in or from the buffer) for the producer and consumer threads*/
std::vector<int> buffer;
int putIndex,getIndex;

//declare vectors that store time taken by producers and consumers in milliseconds to calculate avg
std::vector<double> producerTimes,consumerTimes;

//input parameters
int capacity,np,nc,cntp,cntc,mu_p,mu_c;

//declare semaphores
sem_t empty;
sem_t full;
sem_t m;

//function declarations
std::string getSysTime();
double exponentialDelay(double mean);
void put(int value);
void get();
void* producer(void* arg);
void* consumer(void* arg);

//main function
int main(){
    std::srand(std::time(nullptr));

    //redirect stdout to the output file
    freopen("output.txt", "w", stdout);

    //read the input parameters from input.txt
    std::ifstream inputFile("input.txt");
    inputFile >> capacity >> np >> nc >> cntp >> cntc >> mu_p >> mu_c;
    inputFile.close();

    //resize the vectors according to input parameters
    buffer.resize(capacity);
    producerTimes.resize(np);
    consumerTimes.resize(nc);

    //initialize the semaphores
    sem_init(&empty,0,capacity);
    sem_init(&full,0,0);
    sem_init(&m,0,1);

    //create threads
    pthread_t threads[np+nc];
    for (int i = 0; i < np+nc; i++){
        if(i < np){
            int* id = new int(i+1);
            pthread_create(&threads[i], nullptr, producer, id);
        }
        else{
            int* id = new int(i+1-np);
            pthread_create(&threads[i], nullptr, consumer, id);
        }
    }

    //wait for all threads
    for (int i = 0; i < np+nc; i++){
        pthread_join(threads[i], nullptr);
    }

    //calculate the total and avg times 
    double avgProducerTime,avgConsumerTime,totalProducerTime = 0, totalConsumerTime = 0;
    for (size_t i = 0; i < np; i++){
        totalProducerTime += producerTimes[i];
    }
    for (size_t i = 0; i < nc; i++){
        totalConsumerTime += consumerTimes[i];
    }
    avgProducerTime = totalProducerTime/(np*cntp);
    avgConsumerTime = totalConsumerTime/(nc*cntc);

    std::cout << "average time taken by producer threads is " << avgProducerTime << " ms\n";
    std::cout << "average time taken by consumer threads is " << avgConsumerTime << " ms\n";
    
    //redirect stdout to the terminal
    freopen("/dev/tty", "w", stdout);

    //output the average time taken by prod and cons to the standard output
    std::cout << "average time taken by producer threads is " << avgProducerTime << " ms\n";
    std::cout << "average time taken by consumer threads is " << avgConsumerTime << " ms\n";


    //destroy the semaphores
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&m);

    return 0;
}

//Get system time 
std::string getSysTime(){
    auto now = std::chrono::system_clock::now();
    time_t currentTime = std::chrono::system_clock::to_time_t(now);
    tm* systemTime = localtime(&currentTime);
    std::stringstream ss;
    ss << std::put_time(systemTime, "%H:%M:%S");
    return ss.str();
}

//Get an exponential delay from mean
double exponentialDelay(double mean){
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::exponential_distribution<double> d(1.0 / mean);
    return d(gen);
}

//Put an item into the buffer
void put(int value){
    buffer[putIndex] = value;
    putIndex = (putIndex + 1) % capacity;
}

//Get an item from the buffer
void get(){
    getIndex = (getIndex + 1) % capacity;
}

//Producer function
void* producer(void* arg){
    int id = *(int*)arg;
    delete (int*)arg; 
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < cntp; i++){
        sem_wait(&empty);
        sem_wait(&m);
        int item = std::rand() % 100 + 1;
        put(item);
        std::string producerTimePoint = getSysTime();
        sem_post(&m);
        sem_post(&full);
        printf("item %d produced by thread %d at %s into buffer location %d\n",i+1,id,
                producerTimePoint.c_str(),putIndex);
        std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(exponentialDelay(mu_p)));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    producerTimes[id - 1] = timeTaken.count();

    return nullptr;
}

//Consumer function
void* consumer(void* arg){
    int id = *(int*)arg;
    delete (int*)arg; 
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < cntc; i++){
        sem_wait(&full);
        sem_wait(&m);
        get();
        std::string consumerTimePoint = getSysTime();
        sem_post(&m);
        sem_post(&empty);
        printf("item %d consumed by thread %d at %s from buffer location %d\n",i+1,id,
                consumerTimePoint.c_str(),getIndex);
        std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(exponentialDelay(mu_c))); 
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    consumerTimes[id - 1] = timeTaken.count();

    return nullptr;
}
