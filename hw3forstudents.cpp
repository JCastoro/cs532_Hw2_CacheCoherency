#include <cmath>
#include <chrono>
#include <iostream>
#include <unistd.h>
//#include <sched.h> for cpu set mabye??
#include <vector>

#include <thread>

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::microseconds;
using std::chrono::nanoseconds;

using std::cerr;
using std::cout;
using std::endl;

using std::log2;

using std::vector;
using std::thread;

#define MAX_ARRAY_SIZE 10000000
#define NUM_TIMES_SPIN 100000000

/* If you have access to a linux enviroment, 
you can enable the pin function to fix the binding of threads to cores. 
*/
// void pin(int core_id) {
//     cpu_set_t cpuset;
//     CPU_ZERO(&cpuset);
//     CPU_SET(core_id, &cpuset);
//
//     pthread_t current_thread = pthread_self();
//     pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
// }

void read_only_same_pos(int i, int *ptr) { // will sum the value in *ptr 100 mill times??
    // pin(i);
    int acc = 0;
    for(auto spin = 0; spin < NUM_TIMES_SPIN; ++spin) {
        acc += *ptr;
    }
}

void read_and_write_same_pos(int i, int *ptr) {//reads and writes to ptr mem location 100 mill times
    // pin(i);
    int acc = 0;
    for(auto spin = 0; spin < NUM_TIMES_SPIN; ++spin) {
        if(i == 0) {
            acc += *ptr;
        }
        else {
            acc -= *ptr;
        }
        *ptr = rand();// this is where we write a random number
    }
}

void read_and_write_same_cache(int i, int *ptr) {
    // pin(i);
    int acc = 0;
    for(auto spin = 0; spin < NUM_TIMES_SPIN; ++spin) {
        if(i == 0) {
            acc += ptr[i];
        }
        else {
            acc -= ptr[i];
        }
        ptr[i] = rand();
    }
}

void coherency_tests() {
    int max_threads = 4;
    vector<thread> workers;
    for(auto num_threads = 1; num_threads <= max_threads; ++num_threads) {
        //iterates up from 1 to 4 threads, so when num_threads is 3 for example there will be 3 threads doing
        //the function called in that test

        // test 1: This will call read only same pos
        int *test1 = new int[1];
        test1[0] = 3;
        auto start = high_resolution_clock::now();

        for(auto i = 0; i < num_threads; ++i) {
            workers.push_back(thread(&read_only_same_pos, i, test1));
            //above line will add the thread which is working on the function "read only same pos", this function
            // takes the arguments i and test1, this calls the function to run as well.

            //if num_threads is 3 here will will have 3 separate threads trying to run the read_only_same_pos func.

        }

        for(auto i = 0; i < num_threads; ++i) {
            workers[i].join();//waits for that thread to finish execution
        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start).count();
        auto duration_seconds = duration/1000000.00;
        cout << "Test 1 " << num_threads << " threads " <<
        "microseconds: " << duration << " seconds: "<< duration_seconds << endl;
        delete[] test1;
        workers.clear();

        // test 2: This will call read and write same pos
        int *test2 = new int[max_threads];//test2 is now a pointer to an int array on the heap of size 4
        for(auto i = 0; i < num_threads; ++i){
            test2[i] = 2;//fills array with 2's
        }
        start = high_resolution_clock::now();
        for(auto i = 0; i < num_threads; ++i) {
            workers.push_back(thread(&read_and_write_same_pos, i, test2));
        }
        for(auto i = 0; i < num_threads; ++i) {
            workers[i].join();
        }
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start).count();
	duration_seconds = duration/1000000.00;
        cout << "Test 2 " << num_threads << " threads " <<
             "microseconds: " << duration << " seconds: "<< duration_seconds << endl;
        delete[] test2;
        workers.clear();

        // test 3: This calls read and write same cache
        int *test3 = new int[max_threads];
        for(auto i = 0; i < num_threads; ++i){
            test3[i] = 2;
        }
        start = high_resolution_clock::now();
        for(auto i = 0; i < num_threads; ++i) {
            workers.push_back(thread(&read_and_write_same_cache, i, test3));
        }
        for(auto i = 0; i < num_threads; ++i) {
            workers[i].join();
        }
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start).count();
        duration_seconds = duration/1000000.00;
	cout << "Test 3 " << num_threads << " threads " <<
             "microseconds: " << duration << " seconds: "<< duration_seconds << endl;
        delete[] test3;
        workers.clear();
    }
}

int main(int argc, char **argv) {
    cout << "COHERENCY: " << endl;
    coherency_tests();
    return 1;
}



