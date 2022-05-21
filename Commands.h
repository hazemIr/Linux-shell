#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

typedef enum {FOREGROUND , BACKGROUND , STOPPED , DEAD} StatusOfJob;

using std::string;

using std::vector;
class Command {
// TODO: Add your data members
public:

    vector<string> arguments;
    const char* current_line;
    pid_t process_pid;

    int outputfile;

    //constructor
    Command(const char* cmd_line , pid_t pid); //TODO: should make it explicit?
    virtual ~Command()=default;
    virtual void execute() = 0;
    virtual FILE* prepare();
    virtual void cleanup();

    // TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command {
public:
    BuiltInCommand(const char* cmd_line , pid_t pid);
    virtual ~BuiltInCommand()=default;
};

class ExternalCommand : public Command {
public:
    ExternalCommand(const char* cmd_line , pid_t pid);
    virtual ~ExternalCommand()=default;
    void execute() override;
};
class SmallShell;
class PipeCommand : public Command {
    // TODO: Add your data members

    char* right_in_char;
    char*  left_in_char;
    SmallShell& my_shell;
    Command* left_cmd;
    Command* right_cmd;
    int ch;

public:
    PipeCommand(const char* cmd_line ,SmallShell& smash,  pid_t pid);
    virtual ~PipeCommand()=default;
    void execute() override;
};
/// BUILT IN COMMANDS

class ChPromptCommand : public BuiltInCommand { /// chprompt
public:
    SmallShell &my_shell;
    ChPromptCommand (const char* cmd_line , pid_t pid , SmallShell& smash);
    virtual ~ChPromptCommand() = default; //TODO: =default?
    void execute() override;
};

class lsCommand : public BuiltInCommand{
public:
    lsCommand(const char* line , pid_t pid);
    virtual ~lsCommand()=default;
    void execute() override ;
};


class ChangeDirCommand : public BuiltInCommand {  ///cd
public:
    char* lastPwd;

    ChangeDirCommand(const char* cmd_line , pid_t pid , char lastPwd[COMMAND_ARGS_MAX_LENGTH]);
    virtual ~ChangeDirCommand()=default;
    void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand { ///pwd
public:
    GetCurrDirCommand(const char* cmd_line , pid_t pid );
    virtual ~GetCurrDirCommand() =default;
    void execute() override;
};

class ShowPidCommand : public BuiltInCommand { ///showpid
public:
    ShowPidCommand(const char* cmd_line , pid_t pid);
    virtual ~ShowPidCommand()=default;
    void execute() override;
};

class JobsList;
class QuitCommand : public BuiltInCommand { ///quit

public:
    JobsList* jobs;

    QuitCommand(const char* cmd_line, JobsList* jobs , pid_t pid);
    virtual ~QuitCommand() =default;
    void execute() override;

};


class JobsList {
public:
    class JobEntry {
        // TODO: Add your data members
    public:
        pid_t pid;
        int my_id;
        unsigned long time_of_beginnig;
        unsigned long time_is_out;
        unsigned long time_is_stopped;
        StatusOfJob status_of_job;
        Command* cmd;

        JobEntry(pid_t pid,int id,StatusOfJob status,unsigned long out,Command* cmd= nullptr);
        ~JobEntry();

        int ApplySignal(int signal);
        void resume(StatusOfJob current_status);
        void stopp();
        void killl();

    };


    int max_id;
    vector<JobEntry*> all_jobs_vector;
public:
    JobsList();
    ~JobsList();
    JobEntry* addJob(Command* cmd , pid_t pid , StatusOfJob status , bool isStopped = false );
    void printJobsList();
    void killAllJobs();
    void removeFinishedJobs();
    JobEntry * getJobById(int jobId);
    // void removeJobById(int jobId);
    JobEntry * getLastJob();
    JobEntry *getLastStoppedJob();
    // TODO: Add extra methods or modify exisitng ones as needed
};

class JobsCommand : public BuiltInCommand { ///jobs
    // TODO: Add your data members
public:
    JobsList* jobs;

    JobsCommand(const char* cmd_line, JobsList* jobs ,pid_t pid);
    virtual ~JobsCommand()=default;
    void execute() override;
};

class KillCommand : public BuiltInCommand { ///kill
    // TODO: Add your data members
public:

    JobsList* jobs;

    KillCommand(const char* cmd_line, JobsList* jobs ,pid_t pid);
    virtual ~KillCommand() =default;
    void execute() override;
};

class ForegroundCommand : public BuiltInCommand { ///fg
    // TODO: Add your data members
public:
    JobsList* jobs;

    ForegroundCommand(const char* cmd_line, JobsList* jobs ,pid_t pid);
    virtual ~ForegroundCommand() {}
    void execute() override;
};

class BackgroundCommand : public BuiltInCommand { ///bg
public:
    JobsList* jobs;

    BackgroundCommand(const char* cmd_line, JobsList* jobs , pid_t pid);
    virtual ~BackgroundCommand() {}
    void execute() override;
};

class TailCommand : public BuiltInCommand {
public:
    TailCommand(const char* cmd_line , pid_t pid);
    virtual ~TailCommand() {}
    void execute() override;
};
class TouchCommand : public BuiltInCommand{
public:
		TouchCommand(const char* cmd_line, pid_t pid);
		virtual ~TouchCommand(){}
		void execute() override;
};

// TODO: ?????????
// TODO:

// TODO: lastPath usage?

class SmallShell {
private:
    char lastPath[COMMAND_ARGS_MAX_LENGTH] ;
    std::string my_name;
    SmallShell();

public:
    JobsList* jobslist;
    Command *CreateCommand(const char* cmd_line);
    SmallShell(SmallShell const&)      = delete; // disable copy ctor
    void operator=(SmallShell const&)  = delete; // disable = operator
    static SmallShell& getInstance() // make SmallShell singleton
    {
        static SmallShell instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    ~SmallShell();

    void executeCommand(const char* cmd_line);
    // ADDED
    string getName ()
    {
        return my_name;
    }

    void setName(string name_to_change)
    {
        my_name = name_to_change;
    }

    // TODO: add extra methods as needed
};

#endif //SMASH_COMMAND_H_
