#include "general.h"


PipeRedirect parse_command(int argc, char** argv, char** cmd1, char** cmd2) {
  // Aassume no pipe or redirect or sequential
  PipeRedirect result = NEITHER;

  // holds the index of argv where the pipe or redirect or sequential
  int split = -1;

  // go through the array
  for (int i=0; i<argc; i++) {
    // pipe is found
    if (strcmp(argv[i], "|") == 0) {
      result = PIPE;
      split = i;

    // redirect is found
    } else if (strcmp(argv[i], ">>") == 0) {
      result = REDIRECT;
      split = i;
        //sequential is found
    } else if (strcmp(argv[i], ";") == 0) {
        result = SEQUENTIAL;
        split = i;
    }
  }

  // If anything is found
  if (result != NEITHER) {
    // Go through the array of arguments up to the point where it was found and store each of those arguments in cmd1.
    for (int i=0; i<split; i++)
      cmd1[i] = argv[i];

    // Go through the array of arguments from the point where the pipe/redirect/sequential was found through the end of the array of arguments and store each in cmd2.
    int count = 0;
    for (int i=split+1; i<argc; i++) {
      cmd2[count] = argv[i];
      count++;
    }

    // end cmd1 and 2 with NULL
    cmd1[split] = NULL;
    cmd2[count] = NULL;
  }

  // Return showing whether a pipe, redirect, sequential, or neither was found.
  return result;
}

// This pipes the output of cmd1 into cmd2.
void pipe_cmd(char** cmd1, char** cmd2) {
  int fds[2]; // file descriptors
  pipe(fds);
  pid_t pid;

  // child process #1
  if (fork() == 0) {
    // Reassign stdin to fds[0] end of pipe.
    dup2(fds[0], 0);

    close(fds[1]);

    // Execute cmd2
    execvp(cmd2[0], cmd2);
    perror("execvp failed");

  // child 2
  } else if ((pid = fork()) == 0) {
    // Reassign stdout to fds[1] end of pipe.
    dup2(fds[1], 1);


    close(fds[0]);

    // Execute cmd1
    execvp(cmd1[0], cmd1);
    perror("execvp failed");

  // parent process
  } else
    waitpid(pid, NULL, 0);
}


// get input from user, split into arguments and make array, and then return num of args as int
int read_args(char **argv) {
  char *cstr;
  string arg;
  int argc = 0;

  // Read in arguments till the user hits enter
  while (cin >> arg) {
    // Let the user exit
    if (want_to_quit(arg)) {
      cout << "Exiting...\n";
      exit(0);
    }

    // Convert that string into a C string.
    cstr = new char[arg.size()+1];
    strcpy(cstr, arg.c_str());
    argv[argc] = cstr;

    // Increment counter
    argc++;

    // stop input if user hits enter
    if (cin.get() == '\n')
      break;
  }

  // last argument must be NULL
  argv[argc] = NULL;

  // return num of arguments
  return argc;
}

void redirect_cmd(char** cmd, char** file) {
  int fds[2]; // file descriptors
  int count;  // used for reading from stdout
  int fd;     // single file descriptor
  char c;     // writing and reading a character at a time
  pid_t pid;  // process id

  pipe(fds);

  // child process 1
  if (fork() == 0) {

    fd = open(file[0], O_RDWR | O_CREAT, 0666);

    // -1 if error
    if (fd < 0) {
      printf("Error: %s\n", strerror(errno));
      return;
    }

    dup2(fds[0], 0);


    close(fds[1]);

    // Read from stdout
    while ((count = read(0, &c, 1)) > 0)
      write(fd, &c, 1); // Write to file.


    execlp("echo", "echo", NULL);

  // child process #2
  } else if ((pid = fork()) == 0) {
    dup2(fds[1], 1);

    // end of pipe
    close(fds[0]);

    // output to stdout
    execvp(cmd[0], cmd);
    perror("execvp failed");

  // parent process
  } else {
    waitpid(pid, NULL, 0);
    close(fds[0]);
    close(fds[1]);
  }
}

// creates process and runs arguments

void run_cmd(int argc, char** argv) {
  pid_t pid;
  const char *amp;
  amp = "&";
  bool found_amp = false;

  // If ampersand is the last argument, set flag
  if (strcmp(argv[argc-1], amp) == 0)
    found_amp = true;

  // fork
  pid = fork();

  // throw error
  if (pid < 0)
    perror("Error (pid < 0)");

  // child process
  else if (pid == 0) {
    // Remove the ampersand
    if (found_amp) {
      argv[argc-1] = NULL;
      argc--;
    }

    execvp(argv[0], argv);
    perror("execvp error");

  // parent process
  } else if (!found_amp)
    waitpid(pid, NULL, 0); // oif no ampersand, wait
}

// If found quit or exit, exit the shell
bool want_to_quit(string choice) {
  // Lowercase the user input
  for (unsigned int i=0; i<choice.length(); i++)
    choice[i] = tolower(choice[i]);

  return (choice == "quit" || choice == "exit");
}