#ifndef FUNCTIONS_H

//FUNCTIONS
int splitcmnd(char* cmd, char** params);
void execpipe(char ** argv1, char ** argv2);
void execrdr(char **argv);
int help(char **args);

#define FUNCTIONS_H
#endif