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
//    int status;
}process;

int main()
{
    int TIMER=0;
    FILE *inp,*log_out,*stat_out;
    int n;              //Number of processes in the file
    int total_completed=0,total_missed=0,total_arrived=0;
    inp=fopen("inp-params.txt","r");
    if(inp==NULL)
    {
        cout<<"Error opening the file\n";
        return 1;
    }
    fscanf(inp,"%d",&n);
    int id,tim,per,no,i,current=0,avg_wait[n],cpu_idle=0;
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
    log_out=fopen("EDF-Log.txt","w");
    int changed=1,shift=0,infor=0;
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
    }
    while(!priority.empty())
    {
        if(priority[0].k==0)
        {
            priority.erase(priority.begin());
        }
        if(priority[0].t>0)
        {
            if(cpu_idle>0)
            {
                printf("CPU is idle till %d.\n",TIMER-1);
                fprintf(log_out,"CPU is idle till %d.\n",TIMER-1);
                cpu_idle=0;
            }
            if(changed==1)
            {
                if(priority[0].t==RMqueue[priority[0].start_index].t)
                {
                    printf("Process %d starts execution at time %d.\n",priority[0].Pid,TIMER);
                    fprintf(log_out,"Process %d starts execution at time %d.\n",priority[0].Pid,TIMER);
                }
                else
                {
                    printf("Process %d resumes execution at time %d.\n",priority[0].Pid,TIMER);
                    fprintf(log_out,"Process %d resumes execution at time %d.\n",priority[0].Pid,TIMER);
                }
                changed=0;
            }
            current=priority[0].Pid;
            priority[0].t--;
            TIMER++;
            if(priority[0].t==0)
            {
                printf("Process %d finishes execution at time %d.\n",priority[0].Pid,TIMER);
                fprintf(log_out,"Process %d finishes execution at time %d.\n",priority[0].Pid,TIMER);
                total_completed++;
                priority[0].t=-1;
                priority[0].k--;
                changed=1;
                current=-1;
                if(priority[0].k==0)
                {
                    priority.erase(priority.begin());
                }
                sort(priority.begin(),priority.end(),[] (const process& first, const process& second)
                {
                        return first.p < second.p;
                });
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
                        changed=1;
//                      current=-1;
                    }
                    priority[i].p+=RMqueue[priority[i].start_index].p;
                    priority[i].t=RMqueue[priority[i].start_index].t;
                    infor=1;
                }
                if(i!=priority[0].Pid && priority[i].t>0)
                {
                    avg_wait[i]++;
                }
            }
            if(infor)
            {
                sort(priority.begin(),priority.end(),[] (const process& first, const process& second)
                {
                    return first.p < second.p;
                });
                if(current!=priority[0].Pid && shift==0 && current!=-1)
                {
                    printf("Process %d is preempted by process %d at time %d.\n",current,priority[0].Pid,TIMER);
                    fprintf(log_out,"Process %d is preempted by process %d at time %d.\n",current,priority[0].Pid,TIMER);
                    changed=1;
                }
                infor=0;
            }
            shift=0;
        }
        else
        {
            priority.push_back(priority[0]);
            priority.erase(priority.begin());
            changed=1;
            shift++;
            if(shift==n)
            {
                TIMER++;
                cpu_idle++;
                for(i=0;i<priority.size();i++)
                {
                    if(priority[i].p==TIMER)
                    {
                        priority[i].p+=RMqueue[priority[i].start_index].p;
                        priority[i].t=RMqueue[priority[i].start_index].t;
                        infor=1;
                    }
                }
                if(infor)
                {
                    sort(priority.begin(),priority.end(),[] (const process& first, const process& second)
                    {
                        return first.p < second.p;
                    });
                    infor=0;
                }
                shift=0;
            }
        }
    }
    fclose(log_out);
    stat_out=fopen("EDF-Stats.txt","w");
    fprintf(stat_out,"Number of Processes came into the system - %d\n",total_arrived);
    fprintf(stat_out,"Number of processes that successfully completed - %d\n",total_completed);
    fprintf(stat_out,"Number of processes that missed their deadlines - %d\n",total_missed);
    int suu=0;
    for(i=0;i<n;i++)
    {
        suu+=avg_wait[i];
        fprintf(stat_out,"Wait time of Process %d - %d\n",RMqueue[i].Pid,avg_wait[i]);;  
    }
    fprintf(stat_out,"Total Avg waiting time - %d\n",suu);
    fclose(stat_out);
    return 0;
}