#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <cstring>

#include "Commands.h"
#include "signals.h"
#include "smash.h"


pid_t smash_pid =-1;

JobsList* jobs_out_of_time;

pid_t fg_process_pid=-1;

bool Is_Piped = false;

vector<pid_t>list_of_piped;

bool Is_smash =true;


int main(int argc, char* argv[]) {
    if(signal(SIGTSTP, ctrlZHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-Z handler");
    }
    if(signal(SIGINT , ctrlCHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-C handler");
    }
    if(signal(SIGTERM , TerminateHandler)==SIG_ERR) {
        perror("smash error: failed to set SIGTERM handler");
    }
    if(signal(SIGUSR1 , PipeHandlerCtlZ)==SIG_ERR) {
        perror("smash error: failed to set SIGUSR1 handler");
    }
    if(signal(SIGUSR2 , continueHandler)==SIG_ERR) {
        perror("smash error: failed to set SIGUSR2 handler");
    }



    SmallShell& smash = SmallShell::getInstance();
    while(true) {
        std::cout << smash.getName() << "> " ;
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        smash.executeCommand(cmd_line.c_str());
    }
    return 0;
}
