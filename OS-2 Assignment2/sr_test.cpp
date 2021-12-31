#include <iostream>
#include <thread>
#include <sys/time.h>
#include <atomic>
#include <unistd.h>
#include <math.h>
#include <fstream>

using namespace std;

atomic<int> lock(0);

double average_time = 0,worst_time = -1;

int k, l1, l2, n;

ofstream ofile;

double ran_expo(double lambda){
    double u;
    u = rand() / (RAND_MAX + 1.0);
    return -log(1- u) / lambda;
}


void testCS(int threadId){
	struct timeval request_time,e_time,c_time;
	double waiting_time;
	for(int i = 0; i< k ;i++){
		//entry section
		gettimeofday(&request_time,NULL);
		int exp,des;
		while(1){
			exp = 0,des = 1;
			if(lock.compare_exchange_strong(exp,des)){break;}
		}
		//critical section
		gettimeofday(&c_time,NULL);
		waiting_time = c_time.tv_sec - request_time.tv_sec + ((c_time.tv_usec - request_time.tv_usec)/1000000.0);
		ofile << "Thread " << threadId << " requested CS at " << request_time.tv_sec<< " for " << i+1 << "th time" << "\n";
		ofile << "Thread " << threadId << " entered CS at " << c_time.tv_sec << "\n";
		sleep(ran_expo(l1));
		gettimeofday(&e_time,NULL);
		if(worst_time < waiting_time){ worst_time = waiting_time;}
		average_time += waiting_time;
		ofile << "Thread " << threadId << " exited CS at " << e_time.tv_sec << "\n";
		lock = 0;
		//exit section
		sleep(ran_expo(l2));
	}
}

int main(){
	srand(time(0));
	ifstream file;
	file.open("input.txt");
	ofile.open("output.txt");
	file >> n >> k >> l1 >> l2;
	thread threads[n];
	for(int i = 0; i< n;i++){
		threads[i] = thread(testCS,i+1);
	}
	for(int i = 0;i<n;i++){
		threads[i].join();
	}
	cout << "Average Waiting time " << average_time/(n*k) << "\n";
	cout << "worst case time " << worst_time << "\n";
}