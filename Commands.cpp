#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include "Commands.h"
#include "smash.h"
#include "signals.h"
#include "Commands.h"
#include <list>
#include <time.h>
#include <dirent.h>
#include <chrono>
#include<cstdio>
#include<iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <utime.h>
#include <sys/types.h>
#include <fstream>

using namespace std;

extern JobsList* jobs_out_of_time;

extern pid_t smash_pid;

extern pid_t fg_process_pid;

extern bool Is_Piped;

extern vector<pid_t>list_of_piped;

extern bool Is_smash;

const std::string WHITESPACE = " \n\r\t\f\v";

#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

string _ltrim(const std::string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string& s)
{
    return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char* cmd_line, char** args) {
    FUNC_ENTRY()
    int i = 0;
    std::istringstream iss(_trim(string(cmd_line)).c_str());
    for(std::string s; iss >> s; ) {
        args[i] = (char*)malloc(s.length()+1);
        memset(args[i], 0, s.length()+1);
        strcpy(args[i], s.c_str());
        args[++i] = NULL;
    }
    return i;

    FUNC_EXIT()
}

bool _isBackgroundComamnd(const char* cmd_line) {
    const string str(cmd_line);
    return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char* cmd_line) {
    const string str(cmd_line);
    // find last character other than spaces
    unsigned int idx = str.find_last_not_of(WHITESPACE);
    // if all characters are spaces then return
    if (idx == string::npos) {
        return;
    }
    // if the command line does not end with & then return
    if (cmd_line[idx] != '&') {
        return;
    }
    // replace the & (background sign) with space and then remove all tailing spaces.
    cmd_line[idx] = ' ';
    // truncate the command line string up to the last non-space character
    cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

int IndexOfPipe(const char* ahmad){
    string line_in_string = string(ahmad);
    unsigned int i= line_in_string.find('|');
    int x = 10;
    int y = 2;
    for(int j =0 ; j<y ; j++){
        x++;
    }
    if(x < 0){
        return -1;
    }
    //unsigned int the_size = line_in_string.size();
    if(i > 0){
        if(i < line_in_string.size())
            return i;
        else
            return -1;
    }
    return -1;
}

void wait_for_job(JobsList::JobEntry* hazem_j)
{
    int job_s;
    int sum = 0;
    int the_num = 4;
    for (int i = 0 ; i<the_num ; i++){
        sum++;
    }
    waitpid(hazem_j->pid,&job_s,WUNTRACED);
    if(WIFEXITED(job_s))
    {
        hazem_j->status_of_job = DEAD;
        if(WEXITSTATUS(job_s) == sum) {
            cout << "smash: " << hazem_j->cmd->current_line << " timed out!" << endl;
        }
    }
}

// TODO: Add your implementation for classes in Commands.h
Command::Command(const char* cmd_line , pid_t pid) : current_line(cmd_line), process_pid(pid) {
    char* copy_of_line = strdup(cmd_line);
    _removeBackgroundSign(copy_of_line);
    outputfile = -1;
    //removing redirection sign
    string heba_str(copy_of_line);
    unsigned int heba_indx = heba_str.find('>');
    if(heba_indx>0 && heba_indx<heba_str.size()) {
        copy_of_line[heba_indx] = '\0';
    }
    for(int i =0 ; i<3 ; i++){
        if(i > 5){
            return;
        }
        //std::cout << "HELLO!\n";
    }
    // put the string in a vector
    istringstream hazem_str(copy_of_line);
    vector<string> the_vec;
    string temp;
    while (hazem_str >> temp) {
        the_vec.push_back(temp);
    }
    arguments =the_vec;
    free(copy_of_line);
}

FILE * Command::prepare() {
    FILE* file;
    string tmp_line = string(this->current_line);
    unsigned int i = tmp_line.find('>');
    int i2= i;
    if((i <= 0 || i>=tmp_line.size())){
        file = stdout;
    }
    bool flag;
    if(i2>0){
        flag =true;
    }
    else{
        flag =false;
    }
    // std::cout << i2 << std::endl ;
    if(flag){
        //std::cout << "*******44****" << std::endl ;
        FILE* output;
        // std::cout << "*******44****" << std::endl ;
        if((tmp_line[i2]=='>') && (tmp_line[i2+1]=='>')){
            //	std::cout << "*******1****" << std::endl ;
            if(current_line + i2 + 2){
                //	std::cout << "*******1****" << std::endl ;
                string bag=_trim(string(current_line + i2 + 2 ));
                char filename[bag.length() + 1];
                strcpy(filename, bag.c_str());
                output = fopen(filename,  "a");
            }
        }
        else{
            //std::cout << "*******33****" << std::endl ;
            if(current_line+i2 +1){
                string bag =_trim(string(current_line + i2 + 1));
                char filename[bag.length() + 1];
                strcpy(filename, bag.c_str());
                output = fopen(filename,  "w");

            }
        }
        if(output == nullptr){
            //std::cout << "*******22****" << std::endl ;
            perror("smash error: open failed");
            return nullptr;
        }
        file=output;
    }
    // std::cout << "*******1****" << std::endl ;
    if(file!=stdout){
        this->outputfile=dup(fileno(stdout));
        close(fileno(stdout));
        dup(fileno(file));
        fclose(file);
    }
    return file;
}

void Command::cleanup() {
    int num;
    num = this->outputfile;
    int x= -1+3-3;
    for (int i=0; i< 5;i++){
        x++;
    }
    x-=5;
    if(num != x){
        close(1);
        dup(this->outputfile);
        close(this->outputfile);
    }
}

ExternalCommand::ExternalCommand(const char *cmd_line, pid_t pid) :Command(cmd_line, pid)
{

}

///green
void ExternalCommand::execute()
{
    char** arguments = new char*[3];
    arguments[0] = new char[10];
    arguments[1] = new char[3];
    int length=  strlen(this->current_line);
    arguments[2] = new char[length+1];
    strcpy(arguments[0], "/bin/bash");
    for(int i=0; i<5;i++);
    strcpy(arguments[1], "-c");
    strcpy(arguments[2], this->current_line);
    _removeBackgroundSign(arguments[2]);
    char* sign = arguments[2];
    string sign_str(sign);
    unsigned int theIindexOfTheSign = sign_str.find('>');
    if(theIindexOfTheSign > 0) {
        unsigned int lengthOfTheSignStr =sign_str.size();
        if(theIindexOfTheSign < lengthOfTheSignStr)
            arguments[2][theIindexOfTheSign] = '\0';
    }
    arguments[3] = nullptr;
    execv(arguments[0], arguments);
}

PipeCommand::PipeCommand(const char *cmd_line, SmallShell& smash, pid_t pid): Command(cmd_line, pid), my_shell(smash) {
    string str= string(cmd_line);
    int i = IndexOfPipe(cmd_line);
    //cout << i << endl;
    if((str[i]=='|' && str[i+1]=='&' && cmd_line+i+2) ){
        ch=2;
        right_in_char=strdup(cmd_line+i+2);
        left_in_char = strdup(cmd_line);
        left_in_char[i]='\0';
        //left_in_char=strdup(cmd_line);

        /*  int org_size;
          static char *dup;
          char *dup_offset;


          org_size = strlen(cmd_line);
          dup = (char *)malloc(sizeof(char)*org_size+1);
          if( dup == NULL)
              return;

          dup_offset = dup;
          while(*cmd_line)
          {
              *dup_offset = *cmd_line;
              dup_offset++;
              cmd_line++;
          }
          *dup_offset = '\0';
          left_in_char =dup;
          left_in_char[i]='\0';
          right_in_char=strdup(cmd_line+i+1);*/
    }
    else{
        //if((str[i]=='|' && str[i+1]=='&' && cmd_line+i+2)){

        // left_in_char=strdup(cmd_line);
        /* int org_size;
         static char *dup;
         char *dup_offset;


         org_size = strlen(cmd_line);
         dup = (char *)malloc(sizeof(char)*org_size+1);
         if( dup == NULL)
             return;

         dup_offset = dup;
         while(*cmd_line)
         {
             *dup_offset = *cmd_line;
             dup_offset++;
             cmd_line++;
         }
         *dup_offset = '\0';

         left_in_char =dup;*/

        ch=1;
        right_in_char = strdup(cmd_line + i +1);
        left_in_char=strdup(cmd_line);
        left_in_char[i]='\0';
        // }
    }
    for(int i=0; i< 10; i++);
    left_cmd= nullptr;
    right_cmd= nullptr;
}

void PipeCommand::execute(){
    int chan[2];
    pipe(chan);
    pid_t pid_left = fork();
    setpgrp();
    if(pid_left==0){
        close(chan[0]);
        close(this->ch);
        dup(chan[1]);

        left_cmd=my_shell.CreateCommand(left_in_char);
        if(left_cmd){
            left_cmd->execute();
        }
        //setpgrp();
    }
    else{
        pid_t pid_right= fork();
        setpgrp();
        if(pid_right==0){
            close(chan[1]);
            close(fileno(stdin));
            dup(chan[0]);
            right_cmd=my_shell.CreateCommand(right_in_char);
            if(right_cmd){
                right_cmd->execute();
            }
            // setpgrp();
        }
        else{
            close(chan[0]);
            close(chan[1]);
            list_of_piped.push_back(pid_left);
            list_of_piped.push_back(pid_right);
            wait(&pid_left);
            wait(&pid_right);
        }
    }
}
/*void PipeCommand::execute() {
    int filesArray[2];
    pipe(filesArray);
    pid_t pid_left = fork();
    setpgrp();
    if(pid_left==0){
        //for(int i=0;i<20; i++){}
        close(filesArray[0]);
        close(this->ch);
        dup(filesArray[1]);
        if(pid_left !=0){
            pid_t pid_right= fork();
            setpgrp();
            if(pid_right==0){
                close(filesArray[1]);
                close(fileno(stdin));
                dup(filesArray[0]);
                right_cmd = my_shell.CreateCommand(right_in_char);
                if(right_cmd){
                    right_cmd->execute();
        }
        left_cmd =my_shell.CreateCommand(left_in_char);
        if(left_cmd){
            left_cmd->execute();
        }
    }
    else{
        pid_t pid_right= fork();
        setpgrp();
        if(pid_right==0){
            close(filesArray[1]);
            close(fileno(stdin));
            dup(filesArray[0]);
            right_cmd = my_shell.CreateCommand(right_in_char);
            if(right_cmd){
                right_cmd->execute();
            }
            if(pid_right !=0){
                for(int i=0;i<20; i++){}
                close(filesArray[0]);
                close(this->ch);
                dup(filesArray[1]);

            }
        }
        else{
            close(filesArray[0]);
            close(filesArray[1]);
            list_of_piped.push_back(pid_left);
            list_of_piped.push_back(pid_right);
            wait(&pid_left);
            wait(&pid_right);
        }
    }
}
}
}*/

bool addToTimedoutList(JobsList::JobEntry* job) {
    bool flag =false;
    unsigned int i=0;
    auto itr = jobs_out_of_time->all_jobs_vector.begin();
    while(i <jobs_out_of_time->all_jobs_vector.size()){
        if(jobs_out_of_time->all_jobs_vector[i]->time_is_out > job->time_is_out){
            jobs_out_of_time->all_jobs_vector.insert(itr, job); // heba: I have changed itr to i
            //zmaniii m7ethaa
            flag=true;
            break;
        }
        i++;
        ++itr;
    }
    if(!flag)
    {
        jobs_out_of_time->all_jobs_vector.push_back(job);
    }
    return jobs_out_of_time->all_jobs_vector.front()==job;
}

BuiltInCommand::BuiltInCommand(const char* cmd_line, pid_t pid) : Command(cmd_line, pid)
{

}

ChPromptCommand::ChPromptCommand(const char *cmd_line, pid_t pid, SmallShell& smash) : BuiltInCommand(cmd_line, pid), my_shell(smash)
{

}

void ChPromptCommand::execute() {
    if(arguments.size()==1){
        my_shell.setName("smash");
    }
    else if(arguments.size() ==2){
        my_shell.setName(arguments[1]);
    }
    else if(arguments.size() ==3)
        for(unsigned int i=0; i<  arguments.size();i++) //HEBA: I have added i++ , checkkkk
            my_shell.setName(arguments[1]);
    else{
        my_shell.setName(arguments[1]);
    }
}

ShowPidCommand::ShowPidCommand(const char *cmd_line , pid_t pid) : BuiltInCommand(cmd_line, pid)
{

}
void ShowPidCommand::execute() {

    /*for(int i=0; i< 25; i++)
        cout<< "test in hereeeeeeeee\n";*/
    cout<< "smash pid is "<< smash_pid<<endl;

//cout<< "smash pid is (?P<pid>\\d+)" <<endl;
}

GetCurrDirCommand::GetCurrDirCommand(const char *cmd_line, pid_t pid) : BuiltInCommand(cmd_line, pid)
{

}

void GetCurrDirCommand::execute() {
    char new_path[PATH_MAX]={'\0'};
    if(getcwd(new_path, PATH_MAX) == nullptr){
        perror("smash error: getcwd failed");
    }
    cout << new_path << endl;
}

ChangeDirCommand::ChangeDirCommand(const char *cmd_line, pid_t pid, char *last) : BuiltInCommand(cmd_line,pid), lastPwd(last)
{

}
void ChangeDirCommand::execute() {

    if(arguments.size()>2 || arguments.size() > 3){
        /*for(i=0;i<25;i++) cout << "i'm hereeeeeee\n" ;*/
        std::cerr << "smash error: cd: too many arguments" << endl;
        return;
    }
    char new_path[PATH_MAX]={'\0'};

    getcwd(new_path, PATH_MAX);
    if(arguments[1]=="-"){

        if(strcmp(this->lastPwd, "") == 0){ //HEBA: rj3thaa zy ma kant
            cerr<<"smash error: cd: OLDPWD not set"<<endl;
            return;
        }
        int chdir_num =chdir(lastPwd);
        if(chdir_num >0 || chdir_num <0){
            perror("smash error: chdir failed");
            return;
        }
        else{
            strcpy(lastPwd , new_path);

            /*   if ((lastPwd==NULL)||(new_path==NULL)) //[1]

                   throw "Invalid argument(s)"; //[2]

               char * strDestCopy=lastPwd; //[3]

               while ((*lastPwd++=*new_path++)!='\0'); //[4]

               lastPwd = strDestCopy;*/

            // take a pointer pointing to the beginning of the destination string
            //char *ptr = lastPwd;

            // copy the C-string pointed by source into the array
            // pointed by destination
            /*	while (*new_path != '\0')
                {
                    *lastPwd = *new_path;
                    lastPwd++;
                    new_path++;
                }*/

            // include the terminating null character
            //*lastPwd = '\0';
            //lastPwd = ptr;
        }
    }
    else{
        int arg1_size =arguments[1].length();
        char path1[arg1_size+1];
        strcpy(path1,arguments[1].c_str());
        int path1_num=chdir(path1);
        if(path1_num==-1){
            perror("smash error: chdir failed");
            return;
        }
        else{
            strcpy(lastPwd , new_path);
        }
    }
}

JobsCommand::JobsCommand(const char* current_line, JobsList *jobs, pid_t process_pid):BuiltInCommand(current_line, process_pid),jobs(jobs)
{

}

void JobsCommand::execute() {
    jobs->removeFinishedJobs();
    jobs->printJobsList();
}

KillCommand::KillCommand(const char *current_line, JobsList *jobs, pid_t process_pid): BuiltInCommand(current_line, process_pid), jobs(jobs)
{

}

//todo change me
bool is_num(const std::string & s)
{
    if(s.empty()){
        return false;
    }
    if (!((isdigit(s[0])) || (s[0] == '-') || (s[0] == '+'))) return false;

    char * p;
    strtol(s.c_str(), &p, 10);
    bool flag_int;
    if(*p == 0){
        flag_int = true;
    }else{
        flag_int = false;
    }
    return flag_int;
}
///


void KillCommand::execute() {
    if (arguments.size()!=3 && (arguments.size() >3 || arguments.size() <3)){
        std::cerr << "smash error: kill: invalid arguments" << endl;
        return;
    }
    if(arguments.size() ==2){
        std::cerr << "smash error: kill: invalid arguments" << endl;
        return;
    }

    if(!is_num(arguments[1])){
        std::cerr << "smash error: kill: invalid arguments" << endl;
        return;
    }
    if(!is_num(arguments[2])){
        std::cerr << "smash error: kill: invalid arguments" << endl;
        return;
    }
    int arg1 =stoi(arguments[1]);
    int arg2 =stoi(arguments[2]);
    if(arg1>=0 ){//from string to int func
        std::cerr << "smash error: kill: invalid arguments" << endl;
        return;
    }
    if (jobs->getJobById(arg2)== nullptr){
        std::cerr << "smash error: kill: job-id " << arguments[2] << " does not exist" << endl;
        return ;
    }
    if( arg1<-31){
        std::cerr << "smash error: kill: invalid arguments" << endl;
        return ;
    }
    JobsList::JobEntry* job= jobs->getJobById(arg2);
    //int sig_num=stoi(arguments[1]);
    int minus_arg1=arg1/(-1);
    job->ApplySignal(minus_arg1);
    cout << "signal number " << minus_arg1 <<" was sent to pid " << to_string(job->pid)<< endl;

}

ForegroundCommand::ForegroundCommand(const char *current_line, JobsList *jobs, pid_t current_pid): BuiltInCommand(current_line,current_pid),jobs(jobs)
{

}

void ForegroundCommand::execute() {
    if ((jobs->all_jobs_vector.size()==0) && arguments.size()==1){
        // if(arguments.size() ==1)
        std::cerr << "smash error: fg: jobs list is empty" << endl;
        return ;
    }
    if(arguments.size()>1 || arguments.size() > 4){
        if(arguments.size()>2 || (arguments.size() >=2 && arguments.size() !=2))
        {
            if(arguments.size() >=2 && arguments.size() !=2){
                std::cerr << "smash error: fg: invalid arguments" << endl;
                return;
            }
            if(arguments.size() >2){
                std::cerr << "smash error: fg: invalid arguments" << endl;
                return;
            }
        }
        if(!is_num(arguments[1])) {
            std::cerr << "smash error: fg: invalid arguments" << endl;
            return;
        }
        int arg1 =stoi(arguments[1]);
        if(jobs->getJobById(arg1)== nullptr){
            std::cerr << "smash error: fg: job-id " << arguments[1] << " does not exist" << endl;
            return ;
        }
    }

    JobsList::JobEntry* job= nullptr;
    int job_id;

    if(arguments.size()==1){
        job = jobs->getLastJob();
        job_id=job->my_id;
    } else {
        job_id=stoi(arguments[1]);
        job = jobs->getJobById(job_id);

    }
    job->resume(FOREGROUND);
    fg_process_pid=job->pid;//what is this?
    //job->pid
    //هبه اذا بتقري هاي الهعرا معناتك رجعتي علوظيفة
    cout << job->cmd->current_line << " : " << job->pid<< endl;
    wait_for_job(job);
}
//TODO :arrived here
//--------------------------------------------------------------------------------------------------------
BackgroundCommand::BackgroundCommand(const char *current_line, JobsList *jobs, pid_t current_pid): BuiltInCommand(current_line, current_pid), jobs(jobs){}

void BackgroundCommand::execute() {

    if(arguments.size()>2 ) {
        std::cerr<< "smash error: bg: invalid arguments" << endl;
        return;
    }

    else if(arguments.size()==2){
        for(unsigned int i=0; i<arguments.size();i++){
            if(i==1)
                break;
        }
        if(!is_num(arguments[1])){
            std::cerr << "smash error: bg: invalid arguments" << endl;
            return;
        }
        if (jobs->getJobById(stoi(arguments[1]))== nullptr){
            std::cerr << "smash error: bg: job-id " << arguments[1] << " does not exist" << endl;
            return;
        }
        int arg1 =stoi(arguments[1]);
        if(jobs->getJobById(arg1)->status_of_job !=STOPPED){
            std::cerr<<"smash error: bg: job-id "<< arguments[1] <<" is already running in the background"<< endl;
            return;
        }
    }
    if(arguments.size()==1){
        if(jobs->all_jobs_vector.size()==0){
            std::cerr<< "smash error: bg: there is no stopped jobs to resume" << endl;
            return;
        }
        if (jobs->getLastStoppedJob()== nullptr){
            std::cerr<< "smash error: bg: there is no stopped jobs to resume" << endl;
            return;
        }
    }
    //now we need to execute


    JobsList::JobEntry* job= nullptr;
    // int job_id;
    if(arguments.size()==1)
        job = jobs->getLastStoppedJob();
        //    job_id=job->my_id;
    else
        //    job_id=stoi(arguments[1]);
        job = jobs->getJobById(stoi(arguments[1]));

    job->resume(BACKGROUND);
    cout << job->cmd->current_line<< " : " << job->pid << endl;
}

//-----------------------------------------------------------------------------------------------------------
QuitCommand::QuitCommand(const char *cmd_line, JobsList *jobs, pid_t pid):  BuiltInCommand(cmd_line,pid),jobs(jobs) {}

void QuitCommand::execute() {
    if (arguments.size()>1)
    {
        jobs->removeFinishedJobs();
        jobs->killAllJobs();
    }
    exit(0);
}

SmallShell::SmallShell() {

    smash_pid=getpid();

    my_name="smash";
    jobslist= new JobsList();
    int x=5;int sum=0;
    for(int i=0; i<x;i++){
        sum +=i;
    }
}

SmallShell::~SmallShell() {
    delete jobslist;
}
//----------------------------------------------------------------------------------------------------------------
///change
JobsList::JobEntry::JobEntry(pid_t current_pid, int my_id, StatusOfJob status, unsigned long timeOutTime, Command *command): pid(current_pid)//what is jobstatus
        , my_id(my_id), status_of_job(status)
{
    this->cmd=command;
    this->time_is_out=timeOutTime;
    time_of_beginnig=time(nullptr);
    time_is_stopped=0;

}

JobsList::JobEntry::~JobEntry() {
    delete(cmd);
}
void JobsList::JobEntry::stopp() {
    kill(pid, SIGSTOP);
    status_of_job=STOPPED;
    time_is_stopped=time(nullptr);
}
void JobsList::JobEntry::resume(StatusOfJob current_status) {
    if(IndexOfPipe(this->cmd->current_line)!=-1){
        kill(pid, SIGUSR2);

    }
    else{
        kill(pid, SIGCONT);

    }
    this->status_of_job=current_status;
}
///till here
void JobsList::JobEntry::killl() {
    if(IndexOfPipe(this->cmd->current_line)!=-1){
        kill(pid, SIGTERM);

    }
    else{
        kill(pid, SIGKILL);
    }
    this->status_of_job=DEAD;
}
int JobsList::JobEntry::ApplySignal(int sig) {
    if(sig==SIGSTOP){
        this->stopp();
    }
    if(sig==SIGKILL){
        this->killl();
    }

    if(sig==SIGCONT) {
        this->resume(BACKGROUND);
    }else {

        (kill(this->pid, sig) == -1);
    }
    return 0;

}
JobsList::JobsList() {
    this->max_id=1;
}

JobsList::~JobsList() {///check
    unsigned int i=0;
    while(i <this->all_jobs_vector.size()){
        delete this->all_jobs_vector[i];
        i++;
    }
}
int getNewMax(JobsList* jobs){
    int max=0;
    for(auto itr = jobs->all_jobs_vector.begin(); itr!=jobs->all_jobs_vector.end(); ++itr){
        if((*itr)->my_id>max){
            max=((*itr)->my_id);
        }
    }
    return max;
}

JobsList::JobEntry * JobsList::addJob(Command *cmd,pid_t pid,StatusOfJob status, bool isStopped) {
    unsigned long  timeOut=0;
    max_id=getNewMax(this);
    JobsList::JobEntry* job = new JobsList::JobEntry(pid,max_id+1,status,timeOut,cmd);
    this->all_jobs_vector.push_back(job);
    return job;
}


void JobsList::printJobsList() {

    for(auto itr=all_jobs_vector.begin();itr != all_jobs_vector.end();++itr)
    {
        if((*itr)->status_of_job==STOPPED)
        {
            cout<<"["<<(*itr)->my_id<<"] "<<(*itr)->cmd->current_line<<" : "<<(*itr)->pid<<" "<<time(nullptr)-(*itr)->time_of_beginnig<<" secs "<<"(stopped)"<<endl;
            // cout<<"\\["<<(*itr)->my_id<<"\\] "<<(*itr)->cmd->current_line<<" : "<<"\\d+"<<" "<< "\\d+" <<" secs "<<"(stopped)"<<endl;

        }else{
            cout<<"["<<(*itr)->my_id<<"] "<<(*itr)->cmd->current_line<<" : "<<(*itr)->pid<<" "<<time(nullptr)-(*itr)->time_of_beginnig<<" secs"<<endl;
            //cout<<"\\["<<(*itr)->my_id<<"\\] "<<(*itr)->cmd->current_line<<" : "<< "\\d+"  <<" "<< "\\d+"<<" secs"<<endl;

        }

    }
}

void JobsList::killAllJobs() {
    cout<<"smash: sending SIGKILL signal to "<<all_jobs_vector.size()<<" jobs:"<<endl;
    for(auto itr=all_jobs_vector.begin();itr != all_jobs_vector.end();++itr)
    {
        cout<<(*itr)->pid<<": "<<(*itr)->cmd->current_line<<endl;
        (*itr)->ApplySignal(SIGKILL);
    }
}
int GetProcessStatus(JobsList::JobEntry* job){
    int status;
    int x=5;
    int sum=0;
    for(int i=0; i<x;i++){
        sum +=i;
    }
    waitpid((job)->pid, &status, WUNTRACED|WNOHANG);
    if(WIFEXITED(status)){
        job->killl();
    }
    if(WIFSTOPPED(status)!=0){ //
        job->status_of_job=STOPPED;
    }

    return status+x-5+sum-sum;
}


void JobsList::removeFinishedJobs() {
    for(auto itr=all_jobs_vector.begin(); itr!=all_jobs_vector.end();){
        GetProcessStatus(*itr);
        if((*itr)->status_of_job==DEAD ){
            delete *itr;
            itr=all_jobs_vector.erase(itr);
        }
        else{
            ++itr;
        }
    }
}
JobsList::JobEntry* JobsList::getJobById(int jobId) {
    unsigned int i=0;
    while(i<all_jobs_vector.size()){
        if(all_jobs_vector[i]->my_id == jobId){
            return all_jobs_vector[i];
        }
        i++;
    }
    return nullptr;
}

JobsList::JobEntry* JobsList::getLastJob() {
    unsigned int i=0;
    JobsList::JobEntry* j;
    while(i<all_jobs_vector.size()){
        j =all_jobs_vector[i];
        i++;
    }
    return j;
}
JobsList::JobEntry* JobsList::getLastStoppedJob() {
    JobsList::JobEntry * j= nullptr;
    unsigned int i=0;
    while(i<all_jobs_vector.size()){
        if(all_jobs_vector[i] && all_jobs_vector[i]->status_of_job == STOPPED){
            j=all_jobs_vector[i];
        }
        i++;
    }
    return j;
}
TailCommand::TailCommand(const char *cmd_line, pid_t pid) : BuiltInCommand(cmd_line, pid) {

}


void TailCommand::execute() {
    int num_of_line =0;
    if(arguments.size() == 2){
        string line;
        int myfile;

        myfile = open(arguments[1].c_str() , O_RDONLY);
        //FILE* pfile;
        //pfile = fopen(arguments[1].c_str(), "r");
        if(myfile==-1){
            perror("smash error: open failed");
            return;
        }
        char buffer[1];
        ssize_t total_read;
        int count = 0;
        num_of_line =10;
        while((total_read = read(myfile , buffer , 1)) > 0){
            if(buffer[0] == '\n'){
                count++;
                
            }
        }
        close(myfile);
        myfile = open(arguments[1].c_str() , O_RDONLY);
        if(myfile==-1){
            perror("smash error: open failed");
            return;
        }
        
        //pfile.clear();
        //pfile.seekg(0);
        if (count <num_of_line){
            while((total_read = read(myfile , buffer , 1)) > 0){
                ssize_t cur_written = write(1,buffer,1);
                if(cur_written < 1) {
                    perror("smash error: write failed");
                    return;
                }
            }
        }
        int i=0;
        while((total_read = read(myfile , buffer , 1)) > 0){
            if(buffer[0] == '\n'){
                i++;
            }
            if(i >= count-num_of_line){
                ssize_t cur_written = write(1,buffer,1);
                if(cur_written < 1) {
                    perror("smash error: write failed");
                    return;
                }
            }
        }

        /*  while((!myfile.eof()) && (num_of_line<10)){
              getline(myfile,line);
              num_of_line++;
              if( line == "" || line == "\t" || line == "\n" || line.empty() ){
                  //cout << "HELLO THERE" << endl;
                          break;
                      }
              else
              cout << line << endl;
          }*/
        if(total_read < 0 ) {
            perror("smash error: read failed");
            return;
        }
        close(myfile);
        return;
    }
    if(arguments.size() == 3){
        if(is_num(arguments[1])){
            int the_num = stoi(arguments[1]);
            the_num = the_num * (-1);
            string line;
            int myfile;
            num_of_line =the_num;
            myfile = open(arguments[1].c_str() , O_RDONLY);
            //FILE* pfile;
			//pfile = fopen(arguments[1].c_str(), "r");
            if(myfile==-1){
                perror("smash error: open failed");
                return;
            }
            char buffer[1];
            ssize_t total_read;
            int count = 0;
            while((total_read = read(myfile , buffer , 1)) > 0){
                if(buffer[0] == '\n'){
                    count++;

                }

            }
            
            //pfile.clear();
            //pfile.seekg(0);
            close(myfile);
            myfile = open(arguments[1].c_str() , O_RDONLY);
            //FILE* pfile;
			//pfile = fopen(arguments[1].c_str(), "r");
            if(myfile==-1){
                perror("smash error: open failed");
                return;
            }
            if (count <num_of_line){
                while((total_read = read(myfile , buffer , 1)) > 0){
                    ssize_t cur_written = write(1,buffer,1);
                    if(cur_written < 1) {
                        perror("smash error: write failed");
                        return;
                    }
                }
            }
            int i=0;
            while((total_read = read(myfile , buffer , 1)) > 0){
                if(buffer[0] == '\n'){
                    i++;
                }
                if(i >= count-num_of_line){
                    ssize_t cur_written = write(1,buffer,1);
                    if(cur_written < 1) {
                        perror("smash error: write failed");
                        return;
                    }
                }
            }

            /*  while((!myfile.eof()) && (num_of_line<10)){
                  getline(myfile,line);
                  num_of_line++;
                  if( line == "" || line == "\t" || line == "\n" || line.empty() ){
                      //cout << "HELLO THERE" << endl;
                              break;
                          }
                  else
                  cout << line << endl;
              }*/
            if(total_read < 0 ) {
                perror("smash error: read failed");
                return;
            }
            close(myfile);
            return;
        }else{
            std::cerr << "smash error: tail: not enough arguments" << endl;
        }
    }else{
        std::cerr << "smash error: tail: not enough arguments" << endl;
    }
}
//----------------------------------------------------------------------------------------------------------------
TouchCommand::TouchCommand(const char *cmd_line, pid_t pid) : BuiltInCommand(cmd_line, pid){}
void TouchCommand::execute(){
    if(arguments.size() != 2){
        std::cerr << "smash error: touch: invalid arguments" << endl;
        return;
    }
    char *ptr = nullptr; // declare a ptr pointer
    ptr = strtok(ptr, ":");
    int count[6];
    int i=0;
    while (ptr != NULL)
    {
        count[i++]=stoi(ptr);
        ptr = strtok (NULL, ":");
    }
    struct tm *t;
    t->tm_sec=count[0];
    t->tm_min=count[1];
    t->tm_hour=count[2];
    t->tm_mday=count[3];
    t->tm_mon=count[4]-1;
    t->tm_year=count[5];
    struct utimbuf *x;
    time_t h =mktime(t);
    x->actime= h;
    x->modtime= h;
    int myfile;
    myfile = open(arguments[1].c_str() , O_RDONLY);
    if(myfile==-1){
        perror("smash error: open failed");
        return;
    }
    if(utime(arguments[1].c_str() ,x) !=0){
        perror("smash error: utime failed");
        return;
    }


}
Command* SmallShell::CreateCommand(const char* cmd_line) {
    //std::cout << "***********\n" ;
    char* heba = strdup(cmd_line);
    pid_t HazemPid = getpid();
    _removeBackgroundSign(heba);

    string heba_str(heba);

    unsigned int hazem_i = 0;

    hazem_i = heba_str.find('>');

    if((hazem_i > 0) && (hazem_i < heba_str.size())){
        heba[hazem_i] = '\0';
    }
    istringstream ssssstr(heba);
    //  string ssssstr = string(heba_str);
    vector<string> cmd;
    string temp22;

    while(ssssstr >> temp22){
        cmd.push_back(temp22);
    }
    //std::cout << "***********\n" ;
    jobslist->removeFinishedJobs();
//cout << cmd[0] << endl;
    if(IndexOfPipe(cmd_line)>0){
        //	cout << "pipe" << endl;
        return new PipeCommand(cmd_line, *this,HazemPid);
    }

    if(cmd[0] == "bg"){

        return new BackgroundCommand(cmd_line, jobslist,HazemPid);
    }
    if(cmd[0] == "cd"){

        return new ChangeDirCommand( cmd_line,HazemPid, this->lastPath); //we need to check "-" error inside
    }
    if (cmd[0] == "chprompt"){
        //cout << "IM HERE\n";
        return new ChPromptCommand(cmd_line, HazemPid, *this);
    }

    if(cmd[0] == "fg"){
        return new ForegroundCommand(cmd_line, jobslist,HazemPid);
    }
    if(cmd[0] == "tail"){
        return new TailCommand(cmd_line,HazemPid);
    }
    if (cmd[0] == "jobs"){
        return new JobsCommand(cmd_line, jobslist,HazemPid);
    }

    if (cmd[0] == "kill"){
        return new KillCommand(cmd_line, jobslist,HazemPid);
    }

    if(cmd[0] == "pwd"){
        return new GetCurrDirCommand(cmd_line,HazemPid);
    }
    if(cmd[0] == "showpid"){
        //cout << cmd[0] << endl;
        return new ShowPidCommand(cmd_line,HazemPid);
    }

    if(cmd[0] == "quit"){
        return new QuitCommand(cmd_line, jobslist,HazemPid);
    }
    else{
        return new ExternalCommand(cmd_line,HazemPid);
    }
}

void SmallShell::executeCommand(const char *cmd_line) {

    string cmd_str(cmd_line);
    if(cmd_str.empty())
    {
        return;
    }
    char* hazem_s = strdup(cmd_line);
    Command *our_cmd = CreateCommand(hazem_s);

    if(our_cmd == nullptr) return;

    bool flag_bg = _isBackgroundComamnd(cmd_line);
    bool flag_bIn;
//std::cout << "***********" << std::endl;
    if (dynamic_cast<BuiltInCommand*>(our_cmd) != nullptr ){
        flag_bIn = true;
    } else {
        flag_bIn = false;
    }

    if (flag_bIn) {
        //std::cout << "******1*****" << std::endl ;
        FILE *in_file=our_cmd->prepare();
//std::cout << "********2***" << std::endl ;
        if(in_file == nullptr)
        {
            return;
        }
        our_cmd->execute();

        our_cmd->cleanup();

        fg_process_pid = -1;

        delete  our_cmd;

        return;
    }

    pid_t  pid = fork();
    setpgrp();
    if(pid==0)//son
    {
        //std::cout << "****SON*******" << std::endl ;
        Is_smash=false;
        FILE *f=our_cmd->prepare();
        if(!f)
        {
            return;
        }

        our_cmd->execute();
        our_cmd->cleanup();
        exit(0);
    }else{
        //std::cout << "*****FATHER******" << std::endl ;

        JobsList::JobEntry* job=jobslist->addJob(our_cmd, pid, flag_bg ? BACKGROUND : FOREGROUND);
        int index =0;
        if(!flag_bg)
        {
            index = IndexOfPipe(cmd_line);
            if(index != -1){
                Is_Piped=true;
            }
            else{
                Is_Piped=false;
            }
            fg_process_pid = pid;
            wait_for_job(job);
        }

    }
    // std::cout << "***********" << std::endl ;
    fg_process_pid = -1;
}


