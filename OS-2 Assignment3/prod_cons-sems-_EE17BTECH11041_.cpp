#include<iostream>
#include<fstream>
#include<stdlib.h>
#include <unistd.h>
#include<pthread.h>
#include<sys/time.h>
#include<vector>
#include <semaphore.h>
#include<cstring>
#include<math.h>
#include<algorithm>
using namespace std;

sem_t mutex,empty,full;
ofstream output;

vector <double> times_prod;
vector <double> times_cons;
vector<bool> buffer;
int in,out;

struct arg_struct {
    int cap;
    int cnt;
    float u;
    int index;
};

double ran_expo(double lambda){
    double u;
    u = rand() / (RAND_MAX + 1.0);
    return -1000000 * log(1- u) / lambda;
}

string convert_time(time_t epoch_time){
	tm *t = localtime(&epoch_time);
	string ct = to_string(t->tm_hour)+":"+to_string(t->tm_min)+":"+to_string(t->tm_sec);
	return ct;
}


void* producer(void* arguments)
{
    struct timeval produce_time,start_time,end_time;
    gettimeofday(&start_time,NULL);
    struct arg_struct *args = (struct arg_struct*)arguments;
    int capacity=args->cap;
    int cnt_p=args->cnt;
    float u_p=args->u;
    int index=args->index;
    for(int i=0;i<cnt_p;i++)
    {
        sem_wait(&empty);
        sem_wait(&mutex);
        gettimeofday(&produce_time,NULL);
        buffer[in] = true;
        cout<<i+1<<"th item produced by thread-"<<index +1<<" at "<<convert_time(produce_time.tv_sec)<<" into location "<<in+1<<"."<<endl;
        in = (in + 1) %capacity;
        sem_post(&mutex);
        sem_post(&full);
        usleep(ran_expo(u_p));
    }
    gettimeofday(&end_time,NULL);
    times_prod[index]=end_time.tv_sec - start_time.tv_sec + end_time.tv_usec/1000000.0 - start_time.tv_usec/1000000.0 ;
}

void* consumer(void *arguments)
{
    struct timeval consume_time,start_time,end_time;
    gettimeofday(&start_time,NULL);
    pthread_t Id = pthread_self();
    struct arg_struct *args = (struct arg_struct*)arguments;
    int capacity=args->cap;
    int cnt_c=args->cnt;
    float u_c=args->u;
    int index=args->index;
    for(int i=0;i<cnt_c;i++)
    {
        sem_wait(&full);
        sem_wait(&mutex);
        bool next_consumed = buffer[out];
        gettimeofday(&consume_time,NULL);
        cout<<i+1<<"th item consumed by thread-"<<index +1<<" at "<<convert_time(consume_time.tv_sec)<<" into location "<<out+1<<"."<<endl;
        out = (out + 1) % capacity;
        sem_post(&mutex);
        sem_post(&empty);
        usleep(ran_expo(u_c));
    }
    gettimeofday(&end_time,NULL);
    times_cons[index]=end_time.tv_sec - start_time.tv_sec + end_time.tv_usec/1000000.0 - start_time.tv_usec/1000000.0 ;
}

int main()
{
    int capacity,n_p,n_c,cnt_p,cnt_c;
    float u_p,u_c;

    ifstream input;
    input.open("input.txt");
    input>>capacity;
    input>>n_p;
    input>>n_c;
    input>>cnt_p;
    input>>cnt_c;
    input>>u_p;
    input>>u_c;
    for(int i=0;i<capacity;i++)
    {
        buffer.push_back(false);
    }
    in=0;
    out=0;
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, capacity);
    sem_init(&full, 0, 0);
    pthread_t t_c[n_c],t_p[n_p];
    struct arg_struct args_c[n_c];
    for(int i=0;i<n_c;i++)
    {
        times_cons.push_back(0);
        args_c[i].cap=capacity;
        args_c[i].cnt=cnt_c;
        args_c[i].u=u_c;
        args_c[i].index=i;
    }
    struct arg_struct args_p[n_p];
    for(int i=0;i<n_p;i++)
    {
        times_prod.push_back(0);
        args_p[i].cap=capacity;
        args_p[i].cnt=cnt_p;
        args_p[i].u=u_p;
        args_p[i].index=i;
    }
//
    for(int i=0;i<n_c;i++)
    {
        pthread_create(&t_c[i],NULL,consumer,(void *)&args_c[i]);   
    }
    for(int i=0;i<n_p;i++)
    {
        pthread_create(&t_p[i],NULL,producer,(void *)&args_p[i]);
    }
    double time_cons=0;
    double time_prod=0;
    for(int j=0;j<n_c;j++)
    {
        pthread_join(t_c[j], NULL);
        time_prod+=times_prod[j];
    }

    for(int j=0;j<n_p;j++)
    {
        pthread_join(t_p[j], NULL);
        time_cons+=times_cons[j];
    }
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    cout<<"Avg time for each producer - "<<time_prod/n_p<<endl<<"Avg time for each Consumer - "<<time_cons/n_c<<endl;
    return 0;
}