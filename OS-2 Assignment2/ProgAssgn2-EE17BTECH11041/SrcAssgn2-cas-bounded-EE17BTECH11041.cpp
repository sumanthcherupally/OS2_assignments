#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include<pthread.h>
#include<sys/time.h>
#include<atomic>
#include <fstream>
#include<math.h>
#include<vector>

using namespace std;

atomic_flag lock = ATOMIC_FLAG_INIT;
ofstream myfile;
int n,k,l1,l2;
double wait=0,worst=0;
vector<bool> waiting;
void* testCS(void*);

int main()
{
    srand((unsigned)time(NULL));
    myfile.open ("cas-bounded-out.txt");
    FILE *input;
    input=fopen("inp-params.txt","r");
    fscanf(input,"%d %d %d %d",&n,&k,&l1,&l2);
    fclose(input);
    for(int i=0;i<n;i++)
    {
        waiting.push_back(0);
    }
    //printf("%d %d %d %d\n",n,k,l1,l2);
    pthread_t thread_ids[n];
    int id_numbers[n];
    for(int i=0;i<n;i++)
    {
        id_numbers[i]=i;
        //cout<<id_numbers[i]<<"===\n";
        
        pthread_create(&thread_ids[i],NULL,testCS,(void*)&id_numbers[i]);
 //       cout<<(void*)&i;
    }
    cout<<"Outputing in the file cas-bounded-out.txt ........\n";
    cout<<"Do not open the cas-bounded-out.txt file before completing of execution..\n";
    for(int i=0;i<n;i++)
    {
        pthread_join(thread_ids[i],NULL);
    }
    wait=wait/(n*k);
    cout<<"Average waiting time - "<<wait<<endl;
    cout<<"Worst time - "<<worst<<endl;
    return 0;
}

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

void* testCS(void *post)
{
    int position = *(int*)post;
    //cout<<position<<"-\n";
    //int position = (int)_position;
    pthread_t id = pthread_self();
    struct timeval request_time,enter_time,exit_time;
    for(int i=0;i<k;i++)
    {
        //===================ENTRY SECTION======================
        waiting[position]=true;
        gettimeofday(&request_time,NULL);
        bool key=true;
        while(waiting[position] && key)
        {
            key=lock.test_and_set(std::memory_order_acquire);
        }
        waiting[position]=false;
        //=====================CRITICAL SECTION==================
        myfile<< i+1 <<" th CS Request at "<< local_time(request_time.tv_sec) <<" by thread "<< id<<endl; //print the thread request for critical section entry
        gettimeofday(&enter_time,NULL);
        myfile << i+1 << " th CS Entry at "<<local_time(enter_time.tv_sec) << " by thread "<<id<<endl;    //print the thread entry into critical section
        sleep(ran_expo(l1));                                                                   //Simulation of Critical section
        //cout<<ran_expo(l1)<<endl;
        gettimeofday(&exit_time,NULL);
        myfile << i+1 << " th CS Exit at "<<local_time(exit_time.tv_sec) << " by thread "<<id<<endl;      //print the thread exit from critical section
        //=====================EXIT SECTION=======================
        if(worst<enter_time.tv_sec-request_time.tv_sec + (enter_time.tv_usec-request_time.tv_usec)/1000000.0)
        {
            worst=enter_time.tv_sec-request_time.tv_sec + (enter_time.tv_usec-request_time.tv_usec)/1000000.0;
        }
        wait+=enter_time.tv_sec-request_time.tv_sec + (enter_time.tv_sec-request_time.tv_sec)/1000000.0;
        int j=(position+1)%n;
        while((j!=position) && !waiting[j])
        {
            j=(j+1)%n;
        }
        if(j==position)
        {
            lock.clear(std::memory_order_release);
        }
        else
        {
            waiting[j]=false;
        }
        sleep(ran_expo(l2));                                                                   //Simulation of the remaining section
    }
}