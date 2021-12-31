#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include<pthread.h>
#include<sys/time.h>
#include<atomic>
#include <fstream>
#include<math.h>
using namespace std;
atomic_flag lock = ATOMIC_FLAG_INIT;
float wait=0,worst=0;
//atomic_bool lock(0);
//atomic<bool> lock = (atomic<bool>)malloc(sizeof(bool));
double ran_expo(double lambda);
void* testCS(void*);

int n,k,l1,l2;

int t1=1,t2=1;
ofstream myfile;
int main()
{
    srand((unsigned)time(NULL));
    myfile.open ("tas-out.txt");
    FILE *input;
    input=fopen("inp-params.txt","r");
    fscanf(input,"%d %d %d %d",&n,&k,&l1,&l2);
    fclose(input);
    //printf("%d %d %d %d\n",n,k,l1,l2);
    pthread_t thread_ids[n];
    for(int i=0;i<n;i++)
    {
        pthread_create(&thread_ids[i],NULL,testCS,NULL);
    }
    cout<<"Outputing in the file tas-out.txt ........\n";
    cout<<"Do not open the tas-out.txt file before completing of execution..\n";
    for(int i=0;i<n;i++)
    {
        pthread_join(thread_ids[i],NULL);
    }
    wait=wait/(n*k);
    cout<<"Average waiting time - "<<wait<<endl;
    cout<<"Worst time - "<<worst<<endl;
    return 0;
}

string local_time(time_t epoch_time){
	tm *t = localtime(&epoch_time);
	string ct = to_string(t->tm_hour)+":"+to_string(t->tm_min)+":"+to_string(t->tm_sec);
	return ct;
}

double ran_expo(double lambda){
    double u;
    u = rand() / (RAND_MAX + 1.0);
    return -log(1- u) / lambda;
}

void* testCS(void* unused)
{
    pthread_t id = pthread_self();
    struct timeval request_time,enter_time,exit_time;
    for(int i=0;i<k;i++)
    {
        //===================ENTRY SECTION======================
        gettimeofday(&request_time,NULL);
        while(lock.test_and_set(std::memory_order_acquire));            //test_and_set - acquire lock
        //===================CRITICAL SECTION======================
        myfile<< i+1 <<"th CS Request at "<< local_time(request_time.tv_sec) <<" by thread "<< id<<endl; //print the thread request for critical section entry
        gettimeofday(&enter_time,NULL);
        myfile << i+1 << "th CS Entry at "<<local_time(enter_time.tv_sec) << " by thread "<<id<<endl;    //print the thread entry into critical section
        sleep(ran_expo(l1));                                                                   //Simulation of Critical section
        gettimeofday(&exit_time,NULL);
        myfile << i+1 << "th CS Exit at "<<local_time(exit_time.tv_sec) << " by thread "<<id<<endl;      //print the thread exit from critical section
        if(worst<enter_time.tv_sec-request_time.tv_sec + (enter_time.tv_usec-request_time.tv_usec)/1000000.0)
        {
            worst=enter_time.tv_sec-request_time.tv_sec + (enter_time.tv_usec-request_time.tv_usec)/1000000.0;
        }
        wait+=enter_time.tv_sec-request_time.tv_sec + (enter_time.tv_usec-request_time.tv_usec)/1000000.0;
        lock.clear(std::memory_order_release);                                       //release lock
        //===================EXIT SECTION======================
        sleep(ran_expo(l2));                                                                   //Simulation of the remaining section
    }
}