#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;


//array to hold arguments
const int MAX_ARGS = 256;

enum PipeRedirect {PIPE, REDIRECT, SEQUENTIAL, NEITHER};

// splits user's command into two commands
PipeRedirect parse_command(int, char**, char**, char**);

// pipes the cmd1 stdout into cmd2 stdin
void pipe_cmd(char**, char**);

// reads input and returns arguments
int read_args(char**);

// redirects output into given file
void redirect_cmd(char**, char**);

// runs command
void run_cmd(int, char**);

// determines if user wishes to exit
bool want_to_quit(string);