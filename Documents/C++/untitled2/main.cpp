#include "general.h"

// reads user input until exiting shell
int main() {
    char *argv[MAX_ARGS], *cmd1[MAX_ARGS], *cmd2[MAX_ARGS];
    PipeRedirect pipe_redirect;
    int argc;

    // keep returning the user to the prompt until entering 'quit' or 'exit'
    while (true) {
        // Display a prompt.
        cout << "myshell> ";

        // Read in a command from the user.
        argc = read_args(argv);

        // Decipher the command we just read in and split it, if necessary, into
        // cmd1 and cmd2 arrays.  Set pipe_redirect to a PipeRedirect enum value to
        // indicate whether the given command pipes, redirects, or does neither.

        //read command and split into cmd1 and cmd2 if necessary
        pipe_redirect = parse_command(argc, argv, cmd1, cmd2);

        // read commands
        if (pipe_redirect == PIPE)          // piping
            pipe_cmd(cmd1, cmd2);
        else if (pipe_redirect == REDIRECT) // redirecting
            redirect_cmd(cmd1, cmd2);
        else
            run_cmd(argc, argv);              // neither

        // reset argv
        for (int i=0; i<argc; i++)
            argv[i] = NULL;
    }

    // done
    return 0;
}