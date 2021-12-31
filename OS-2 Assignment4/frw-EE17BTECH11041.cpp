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

sem_t rw_mutex,mutexin,mutexout;
int ctrin=0,ctrout=0;
bool wait=0;
ofstream output;

vector <double> wait_writers;
vector <double> wait_readers;

vector <double> worst_wait_writer;
vector <double> worst_wait_reader;

struct arg_struct {
    int cnt;
    float u_cs;
    float u_rem;
    int index;
};

double ran_expo(double lambda){
    double u;
    u = rand() / (RAND_MAX + 1.0);
    return -1000000 * log(1- u) / lambda;
}

string convert_local(time_t epoch_time){
	tm *t = localtime(&epoch_time);
	string ct = to_string(t->tm_hour)+":"+to_string(t->tm_min)+":"+to_string(t->tm_sec);
	return ct;
}

void* reader(void *arguments)
{
    struct timeval reqTime,enterTime,exitTime;
    struct arg_struct *args = (struct arg_struct*)arguments;
    int cnt=args->cnt;
    float lcs=args->u_cs;
    float lr=args->u_rem;
    int index=args->index;
    for(int i=0;i<cnt;i++)
    {
        gettimeofday(&reqTime,NULL);
        sem_wait(&mutexin);
        ctrin++;
        sem_post(&mutexin);
        gettimeofday(&enterTime,NULL);
        output<< i <<" th CS request by Reader Thread "<< index <<" at "<<convert_local(reqTime.tv_sec)<<endl;
        output<< i << " th CS Entry by Reader Thread "<<index<<" at "<<convert_local(enterTime.tv_sec)<<endl;
        usleep(ran_expo(lcs));
        sem_wait(&mutexout);
        ctrout++;
        gettimeofday(&exitTime,NULL);
        output<< i << " th CS Exit by Reader Thread "<< index <<" at "<< convert_local(exitTime.tv_sec) << endl ;
        if(wait==1 && ctrin==ctrout)
        {
            sem_post(&rw_mutex);
        }
        sem_post(&mutexout);
        usleep(ran_expo(lr));
        double time_thread=enterTime.tv_sec-reqTime.tv_sec + (enterTime.tv_usec-reqTime.tv_usec)/1000000.0;
        wait_readers[index]+=time_thread;
        if(worst_wait_reader[index]<time_thread)
        {
            worst_wait_reader[index]=time_thread;
        }
    }
}

void* writer(void *arguments)
{
    struct timeval reqTime,enterTime,exitTime;
    struct arg_struct *args = (struct arg_struct*)arguments;
    int cnt=args->cnt;
    float lcs=args->u_cs;
    float lr=args->u_rem;
    int index=args->index;
    for(int i=0;i<cnt;i++)
    {
        gettimeofday(&reqTime,NULL);
        sem_wait(&mutexin);
        sem_wait(&mutexout);
        if(ctrin==ctrout)
        {
            sem_post(&mutexout);
        }
        else
        {
            wait=1;
            sem_post(&mutexout);
            sem_wait(&rw_mutex);
            wait=0;
        }
        output<<i<< " th CS request by Writer Thread "<<index<<" at "<<convert_local(reqTime.tv_sec)<<endl;
        gettimeofday(&enterTime,NULL);
        output<<i<<" th CS Entry by Writer Thread "<<index<<" at "<<convert_local(enterTime.tv_sec)<<endl;
        usleep(ran_expo(lcs));
        gettimeofday(&exitTime,NULL);
        output <<i<< " th CS Exit by Writer Thread " << index <<" at "<<convert_local(exitTime.tv_sec)<<endl;
        sem_post(&mutexin);
        usleep(ran_expo(lr));
        double time_thread=enterTime.tv_sec-reqTime.tv_sec + (enterTime.tv_usec-reqTime.tv_usec)/1000000.0;
        wait_writers[index]+=time_thread;
        if(worst_wait_writer[index]<time_thread)
        {
            worst_wait_writer[index]=time_thread;
        }
    }
}

int main()
{
    ifstream input;
    int nw,nr,kw,kr;
    float u_cs,u_rem;
    input.open("inp-params.txt");
    input>>nw;
    input>>nr;
    input>>kw;
    input>>kr;
    input>>u_cs;
    input>>u_rem;
    sem_init(&rw_mutex, 0, 0);
    sem_init(&mutexin, 0, 1);
    sem_init(&mutexout, 0, 1);

    pthread_t t_w[nw],t_r[nr];
    struct arg_struct args_w[nw];
    for(int i=0;i<nw;i++)
    {
        args_w[i].cnt=kw;
        args_w[i].u_cs=u_cs;
        args_w[i].u_rem=u_rem;
        args_w[i].index=i;
    }
    struct arg_struct args_r[nr];
    for(int i=0;i<nr;i++)
    {
        args_r[i].cnt=kr;
        args_r[i].u_cs=u_cs;
        args_r[i].u_rem=u_rem;
        args_r[i].index=i;
    }
    output.open("FairRW-log.txt");
    for(int i=0;i<nw;i++)
    {
        wait_writers.push_back(0.0);
        worst_wait_writer.push_back(0.0);
        pthread_create(&t_w[i],NULL,writer,(void *)&args_w[i]);
    }
    for(int i=0;i<nr;i++)
    {
        wait_readers.push_back(0.0);
        worst_wait_reader.push_back(0.0);
        pthread_create(&t_r[i],NULL,reader,(void *)&args_r[i]);
    }
    
    double total_wait_writer=0.0;
    double total_wait_reader=0.0;
    double total_worst_writer,total_worst_reader;
    for(int j=0;j<nr;j++)
    {
        pthread_join(t_r[j], NULL);
        total_wait_reader+=wait_readers[j];
    }
    total_worst_reader=worst_wait_reader[*max_element(worst_wait_reader.begin(),worst_wait_reader.end())];
    for(int j=0;j<nw;j++)
    {
        pthread_join(t_w[j], NULL);
        total_wait_writer+=wait_writers[j];
    }
    total_worst_writer=worst_wait_writer[*max_element(worst_wait_writer.begin(),worst_wait_writer.end())];
    ofstream time_log;
    time_log.open("avg_times_fair.txt");
    time_log<<"Avg waiting time readers : "<<total_wait_reader/(kr*nr)<<" sec"<<endl;
    time_log<<"Avg waiting time writers : "<<total_wait_writer/(kw*nw)<<" sec"<<endl;
    time_log<<"Worst waiting time readers : "<<total_worst_reader<<" sec"<<endl;
    time_log<<"Worst waiting time writers : "<<total_worst_writer<<" sec"<<endl;
    time_log.close();
    cout<<"Avg waiting time readers : "<<total_wait_reader/(kr*nr)<<" sec"<<endl;
    cout<<"Avg waiting time writers : "<<total_wait_writer/(kw*nw)<<" sec"<<endl;
    cout<<"Worst waiting time readers : "<<total_worst_reader<<" sec"<<endl;
    cout<<"Worst waiting time writers : "<<total_worst_writer<<" sec"<<endl;
    sem_destroy(&mutexin);
    sem_destroy(&mutexout);
    sem_destroy(&rw_mutex);
    
    return 0;
}