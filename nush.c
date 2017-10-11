#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "tokens.h"
void
execute(strarr* arr)
{
    int cpid;

    // if the inputted command is "exit"
    if (strcmp(strarr_get(arr, 0), "exit") == 0) {
      exit(0);
    }
    
    int redir_in = 0; // if there is a ">" operator, store its index
    int redir_out = 0; // if there is a "<" operator, store its index
    int fd_copy; // need to store a copy of the redirected file descriptor
    FILE* file; // the file that is used in the redirect 
    
    // check if there is a redirect in the tokens
    int i; 
    for (i = 0; i < arr->size; i++) {
      if (strcmp(strarr_get(arr, i), "<") == 0) {
        redir_in = i;
      }
      else if (strcmp(strarr_get(arr, i), ">") == 0) {
        redir_out = i;
      }  
    }  

    // close stdin and put the given file at fd(0)
    if (redir_in > 0) {
      fd_copy = dup(0); // copy stdin
      close(0);
      file = fopen(strarr_get(arr, redir_in + 1), "r"); // open the given file
    }              

    // close stdout and put the given file at fd(1)
    else if (redir_out > 0) {
      fd_copy = dup(1);
      close(1);
      file = fopen(strarr_get(arr, redir_out + 1), "w");
    }

    // if the command is cd, chdir and don't fork
    if (strcmp(strarr_get(arr, 0), "cd") == 0) {
      chdir(strarr_get(arr, 1));
    }

    else if ((cpid = fork())) {
      int status;

      waitpid(cpid, &status, 0);
    
      // once the child process is done, restore stdin/stdout
      if (redir_in > 0) {
        fclose(file);
        dup2(fd_copy, 0);
        close(fd_copy);
      }
      else if (redir_out > 0) {
        fclose(file);
        dup2(fd_copy, 1);
        close(fd_copy);
      } 
 
    }
    else {
      // child process

      // if there is a redirect, don't need to use all tokens in args
      if (redir_in > 0 || redir_out > 0) {
        char* args[arr->size - 1];
        for (i = 0; i < arr->size - 2; i++) {
          args[i] = strarr_get(arr, i);
        }
        args[arr->size - 2] = NULL;
        execvp(args[0], args); 
      }
      else {
        char* args[arr->size];
        for (i = 0; i < arr->size; i++) {
          args[i] = strarr_get(arr, i); 
        }
        args[arr->size] = NULL;
        execvp(args[0], args);
      }
    }
}

int
main(int argc, char* argv[])
{
    char cmd[256];
    FILE* fp;

    // if nush is in interactive mode
    if (argc == 1) {
      fp = stdin;
    }

    // nush was given a script as an argument
    else { 
      fp = fopen(argv[1], "r");
    }

    while(1) {
      if (argc == 1) {
        printf("nush$ ");
      }
      fflush(stdout);

      char* rv = fgets(cmd, 256, fp);
      if (!rv) {
        break;
      }
      strarr* arr = get_tokens(cmd);
      execute(arr);
      free_strarr(arr);
    }
    return 0;
}
