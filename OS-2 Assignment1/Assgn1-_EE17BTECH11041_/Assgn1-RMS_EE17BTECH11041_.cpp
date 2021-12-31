#include<stdio.h>
#include<iostream>
#include<string.h>
#include <bits/stdc++.h>
#include<algorithm>
#include<unistd.h>
using namespace std;

typedef struct 
{
    int Pid,t,p,k;
    int start_index;
    int status;
}process;

int main()
{
    int TIMER=0;
    FILE *inp,*log_out,*stat_out;
    int n;              //Number of processes in the file
    inp=fopen("inp-params.txt","r");
    int total_completed=0,total_missed=0,total_arrived=0,cpu_idle=0;
    if(inp==NULL)
    {
        cout<<"Error opening the file\n";
        return 1;
    }
    fscanf(inp,"%d",&n);
    int id,tim,per,no,i,current=0,avg_wait[n],inif=1;
    vector<process> RMqueue;
    vector<process> priority;
    //while(!feof(inp))//
    i=0;
    process new_process;
    while(i<n)
    {
        fscanf(inp,"%d",&id);
        fscanf(inp,"%d",&tim);
        fscanf(inp,"%d",&per);
        fscanf(inp,"%d",&no);
        new_process.Pid=id;
        new_process.t=tim;
        new_process.p=per;
        new_process.k=no;
        RMqueue.push_back(new_process);
        priority.push_back(new_process);
        i++;
    }
    fclose(inp);
    log_out=fopen("RM-Log.txt","w");
    int changed=1;
    sort(RMqueue.begin(),RMqueue.end(),[](process& first,process& second)   //Sort the vector of processes with period   
    {
        return first.p < second.p;                 //Compare with the value of period
    });

    sort(priority.begin(),priority.end(),[](process& first,process& second)   //Sort the vector of processes with period   
    {
        return first.p < second.p;                 //Compare with the value of period
    });
    for(i=0;i<n;i++)
    {
        avg_wait[i]=0;
        total_arrived+=RMqueue[i].k;
        priority[i].start_index=i;
        priority[i].status=1;
        RMqueue[i].start_index=i;
    }

    current=RMqueue[0].Pid;

    while(!priority.empty())
    {
        int now=0;
        for(i=0;i<priority.size();i++)
        {
            if(priority[i].t>0 && priority[i].status==1)
            {
                now=i;
                if(priority[i].Pid!=current && current!=-1)
                {
                    printf("Process %d is preempted by process %d at time %d.\n",current,priority[now].Pid,TIMER);
                    fprintf(log_out,"Process %d is preempted by process %d at time %d.\n",current,priority[now].Pid,TIMER);
                    changed=1;
                }
                break;
            }
        }
        if(priority[now].t>0 && priority[now].status==1)
        {
            if(cpu_idle>0)
            {
                printf("CPU is idle till %d.\n",TIMER-1);
                fprintf(log_out,"CPU is idle till %d.\n",TIMER-1);
                cpu_idle=0;
            }
            if(changed==1)
            {
                if(priority[now].t==RMqueue[priority[now].start_index].t)
                {
                    printf("Process %d starts executing at time %d.\n",priority[now].Pid,TIMER);
                    fprintf(log_out,"Process %d starts executing at time %d.\n",priority[now].Pid,TIMER);
                }
                else
                {
                    printf("Process %d resumes executing at time %d.\n",priority[now].Pid,TIMER);
                    fprintf(log_out,"Process %d resumes executing at time %d.\n",priority[now].Pid,TIMER);
                }
                changed=0;
                //avg_wait[now]+=
            }
            current=priority[now].Pid;
            priority[now].t--;
            TIMER++;
            if(priority[now].k==0)
            {
                priority[now].p=-1;
                priority[now].t=-1;
                priority[now].status=0;
            }
            if(priority[now].t==0)
            {
                printf("Process %d finishes execution at time %d.\n",priority[now].Pid,TIMER);
                fprintf(log_out,"Process %d finishes execution at time %d.\n",priority[now].Pid,TIMER);
                total_completed++;
                priority[now].t=-1;
                priority[now].k--;
                priority[now].status=0;
                changed=1;
                current=-1;
                if(priority[now].k==0)
                {
                    priority[now].p=-1;
                    priority[now].t=-1;
                    priority[now].status=0;
                }
            }
            for(i=0;i<priority.size();i++)
            {
                if(TIMER==priority[i].p)
                {
                    if(priority[i].t>0)
                    {
                        printf("Process %d missed deadline at time %d.\n",priority[i].Pid,TIMER);
                        fprintf(log_out,"Process %d missed deadline at time %d.\n",priority[i].Pid,TIMER);
                        total_missed++;
                        priority[i].t=-1;
                        priority[i].k--;
                        if(i==now)
                        {
                            changed=1;
                            current=-1;
                        }
                        priority[i].status=0;
                        if(priority[i].k==0)
                        {
                            priority[i].p=-1;
                            priority[i].t=-1;
                            priority[i].status=0;
                            inif=0;
                        }
                    }
                    if(inif)
                    {
                        priority[i].p+=RMqueue[priority[i].start_index].p;
                        priority[i].t=RMqueue[priority[i].start_index].t;
                        priority[i].status=1;
                    }
                    inif=1;
                }
                if(i!=now && priority[i].status==1)
                {
                    avg_wait[i]++;
                }
            }
        }
        else
        {
            TIMER++;
            cpu_idle++;
//            changed=1;
            for(i=0;i<priority.size();i++)
            {
                if(priority[i].p==TIMER)
                {
                    priority[i].p+=RMqueue[priority[i].start_index].p;
                    priority[i].t=RMqueue[priority[i].start_index].t;
                    priority[i].status=1;
                }
            }
        }
        for(i=0;i<priority.size();i++)
        {
            if(priority[i].p==-1 && priority[i].t==-1 && priority[i].k==0 && priority[i].status==0)
            {
                priority.erase(priority.begin()+i);
                i-=1;
            }
        }
    }
    fclose(log_out);
    stat_out=fopen("RM-Stats.txt","w");
    fprintf(stat_out,"Number of Processes came into the system - %d\n",total_arrived);
    fprintf(stat_out,"Number of processes that successfully completed - %d\n",total_completed);
    fprintf(stat_out,"Number of processes that missed their deadlines - %d\n",total_missed);
    int suu=0;
    for(i=0;i<n;i++)
    {
        suu+=avg_wait[i];
        fprintf(stat_out,"Wait time of Process %d - %d\n",RMqueue[i].Pid,avg_wait[i]);
    }
    fprintf(stat_out,"Total Avg waiting time - %d\n",suu);
    fclose(stat_out);
    return 0;
}