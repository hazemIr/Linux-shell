#ifndef SMASH__SIGNALS_H_
#define SMASH__SIGNALS_H_

void ctrlZHandler(int sig_num);
void ctrlCHandler(int sig_num);
void TerminateHandler(int sig_num);
void PipeHandlerCtlZ(int sig_num);
void continueHandler(int sig_num);

#endif //SMASH__SIGNALS_H_
