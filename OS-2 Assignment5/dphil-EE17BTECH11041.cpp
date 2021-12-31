#include <iostream>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <fstream>
#include<stdlib.h>
#include<vector>
#include <semaphore.h>
#include<string>
#include<math.h>
#include<algorithm>
#define THINKING 0
#define EATING 1
#define HUNGRY 2
using namespace std;

int h,u_cs,u_rem,n;

int *state;

pthread_cond_t* cond;
pthread_mutex_t lock;
pthread_mutex_t l;
ofstream out_log;

double avgWaiting_time;
double worstCase_time;

vector <double> wait_phil;
vector <double> worst_wait_phil;

double ran_expo(double lambda){
    double u;
    u = rand() / (RAND_MAX + 1.0);
    return -log(1- u) / lambda;
}

string local_time(time_t epoch_time){
	tm *t = localtime(&epoch_time);
	string ct = to_string(t->tm_hour)+":"+to_string(t->tm_min)+":"+to_string(t->tm_sec);
	return ct;
}

void test(int i){
	if ((state[(i + n-1) % n] != EATING) &&
		(state[i] == HUNGRY) &&
		(state[(i + 1) % n] != EATING)) {
		state[i] = EATING;
		pthread_cond_signal(&cond[i]);
	}
}

void pickup(int i){
	state[i] = HUNGRY;
	test(i);
	pthread_mutex_lock(&lock);
	if(state[i] != EATING)
		pthread_cond_wait(&cond[i],&lock);
	pthread_mutex_unlock(&lock);
}

void putdown(int i){
	state[i] = THINKING;
	test((i+n-1)%n);
	test((i+1)%n);
}


void *philosopher(void *c) {

	struct timeval reqTime,enterTime,exitTime;
	int *id = (int *)c;
	double waiting_time;

	for(int i = 0;i<h;i++){
		gettimeofday(&reqTime,NULL);
		pickup(*id); // Request for both the chopsticks
		gettimeofday(&enterTime,NULL);

		waiting_time = enterTime.tv_sec + enterTime.tv_usec/1000000.0 - (reqTime.tv_sec + reqTime.tv_usec/1000000.0);

		pthread_mutex_lock(&l);

		avgWaiting_time += waiting_time;
		if(worstCase_time < waiting_time)
        {
			worstCase_time = waiting_time;
		}

		out_log << i+1 << "th" << " eat request by philosopher thread " << (*id)+1 << " at " << local_time(reqTime.tv_sec) << "\n";
		out_log << i+1 << "th" << " CS " << "entered by philosopher thread " << (*id)+1 << " at " << local_time(enterTime.tv_sec) << "\n";

		pthread_mutex_unlock(&l);

		usleep(ran_expo(u_cs)*1000000.0);

		gettimeofday(&exitTime,NULL);
		putdown(*id);

		pthread_mutex_lock(&l);
		out_log << i+1 << "th" << " CS " << "exited by philosopher thread " << (*id)+1 << " at " << local_time(exitTime.tv_sec) << "\n" ;
		pthread_mutex_unlock(&l);

		usleep(ran_expo(u_rem)*1000000.0);

        double time_thread=enterTime.tv_sec-reqTime.tv_sec + (enterTime.tv_usec-reqTime.tv_usec)/1000000.0;
        wait_phil[*id]+=time_thread;
        if(worst_wait_phil[*id]<time_thread)
        {
            worst_wait_phil[*id]=time_thread;
        }
	}
}

int main(){

	srand(time(0));
	ifstream input;
	ofstream time_log;

	input.open("inp-params.txt");
	out_log.open("Phil-log.txt");

	input >> n >> h >> u_cs >> u_rem ;

	cond = (pthread_cond_t *)malloc(n*sizeof(pthread_cond_t));
	state = (int *)malloc(n*sizeof(int));

	pthread_t phil[n];
	int ids[n];

	for(int i=0;i<n;i++){
		state[i] = THINKING;
	}

	for(int i=0;i<n;i++){
		ids[i] = i;
        wait_phil.push_back(0.0);
        worst_wait_phil.push_back(0.0);
		pthread_create(&phil[i],NULL,philosopher,(void *)&ids[i]);

	}
    double total_wait_phil=0.0;
    double total_worst_phil;
	for(int i=0;i<n;i++){
		pthread_join(phil[i],NULL);
        total_wait_phil+=wait_phil[i];
	}
    total_worst_phil=worst_wait_phil[*max_element(worst_wait_phil.begin(),worst_wait_phil.end())];
    time_log.open("Times.txt");
	time_log << "Average waiting time " << total_wait_phil/(n*h) << "\n";
	time_log << "Worst case waiting time " << total_worst_phil << "\n";
    time_log.close();
	cout << "Average waiting time " << total_wait_phil/(n*h) << "\n";
	cout << "Worst case waiting time " << total_worst_phil << "\n";
}
