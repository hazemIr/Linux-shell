#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cmath>
#include <chrono>

#include "signals.h"
#include "Commands.h"
#include "smash.h"


using namespace std;

extern JobsList* jobs_out_of_time;

extern pid_t smash_pid;


extern pid_t fg_process_pid;

extern bool Is_Piped;

extern vector<pid_t>list_of_piped;

extern bool Is_smash;

int GetProcessStatus(JobsList::JobEntry* job);
static volatile sig_atomic_t sig=0;

void derive(pid_t pid , StatusOfJob s)
{
    SmallShell& smash = SmallShell::getInstance();
    for(auto itr = smash.jobslist->all_jobs_vector.begin();itr!=smash.jobslist->all_jobs_vector.end();++itr)
    {
        if((*itr)->pid==pid)
        {
            (*itr)->status_of_job=s;
        }
    }
}

void ctrlZHandler(int sig_num) {
    sig=sig_num;
    if(Is_smash) {
        cout << "smash: got ctrl-Z" << endl;
        if(fg_process_pid!=-1) {
            if (Is_Piped) {
                kill(fg_process_pid, SIGUSR1);
            } else {
                kill(fg_process_pid, 19);
                derive(fg_process_pid,STOPPED);
            }
            cout << "smash: process " << fg_process_pid << " was stopped" << endl;
        }
    }
}
void PipeHandlerCtlZ(int sig_num){
    for(auto itr = list_of_piped.begin(); itr!=list_of_piped.end(); ++itr){
        kill(*itr, SIGSTOP);
        derive(*itr,STOPPED);
    }
    raise(SIGSTOP);
}

void ctrlCHandler(int sig_num) {

    if(Is_smash) {
        cout << "smash: got ctrl-C" << endl;
        if( fg_process_pid!=-1) {
            if (Is_Piped) {
                kill(fg_process_pid, SIGTERM);
                derive(fg_process_pid,DEAD);
            } else {
                kill(fg_process_pid, SIGKILL);
                derive(fg_process_pid,DEAD);
            }
            cout << "smash: process " << fg_process_pid << " was killed" << endl;
        }
    }
}
void TerminateHandler(int sig_num){
    for(auto itr = list_of_piped.begin(); itr != list_of_piped.end(); ++itr){
        kill(*itr , SIGKILL);
        derive(*itr,DEAD);
        wait(&(*itr));
    }
    raise(SIGKILL);
}
void continueHandler(int sig_num) {
    if(Is_Piped) {
        for (auto it = list_of_piped.begin(); it != list_of_piped.end(); ++it) {
            kill(*it, SIGCONT);
        }
    }
    raise(SIGCONT);
}
