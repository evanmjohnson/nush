#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "tokens.h"
void 
exec_pipe(strarr* arr, int pipe_index) 
{
  int cpid, rv;
  int stdin_copy, stdout_copy;

  
  if ((cpid = fork())) {
    // parent
    
    
    int status;
    waitpid(cpid, &status, 0);
  }
  else {
    // child
    
    // split arr into two new strarr's on the pipe
    int i;
    strarr* left = create_strarr();
    strarr* right = create_strarr();
    for (i = 0; i < pipe_index; i++) {
      strarr_add(left, strarr_get(arr, i));
    }
    for (i = pipe_index + 1; i < arr->size; i++) {
      strarr_add(left, strarr_get(arr, i));
    }
      
    int pipes[2];
    rv = pipe(pipes);

    stdin_copy = dup(0);
    stdout_copy = dup(1);

    int gpid;
    // fork to exec left
    if ((gpid = fork())) {
      // parent

      close(0);
      dup2(pipes[0], 0); // read from read-end of pipe, not stdin

      int gstatus; // status of child process (grandchild of original func)

      // create args array for right
      char* rightargs[right->size];
      for (i = 0; i < right->size; i++) {
        rightargs[i] = strarr_get(right, i);
      }
      rightargs[right->size] = NULL;
      
      waitpid(gpid, &gstatus, 0);

      close(pipes[1]); // close write end of pipe
      dup2(stdout_copy, 1); // output to stdout

      execvp(rightargs[0], rightargs); 
    } 
    else {
      // child

      // set up write end of pipe and exec left
      close(1);
      dup2(pipes[1], 1);

      // create args array for left 
      char* leftargs[left->size];
      for (i = 0; i < left->size; i++) {
        leftargs[i] = strarr_get(left, i);
      }
      
      leftargs[left->size] = NULL;
      execvp(leftargs[0], leftargs);
    }
    close(pipes[0]);

    dup2(stdin_copy, 0);

    free_strarr(left);
    free_strarr(right);
  }
    
}

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
    
    // check if there is a redirect or a semicolon in the tokens
    int i; 
    for (i = 0; i < arr->size; i++) {
      if (strcmp(strarr_get(arr, i), "<") == 0) {
        redir_in = i;
        break;
      }
      else if (strcmp(strarr_get(arr, i), ">") == 0) {
        redir_out = i;
        break;
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

void
exec_and(strarr* arr, int index) 
{
  // execute left in child; if waitpid == 0, exec right  

  strarr* right = create_strarr();
  strarr* left = create_strarr();

  int i;
  for (i = 0; i < index; i++) {
    strarr_add(left, strarr_get(arr, i));
  }
  for (i = index + 1; i < arr->size; i++) {
    strarr_add(right, strarr_get(arr, i));
  }      
  
  int cpid;
  if ((cpid = fork())) {
    int status;
    
    if (waitpid(cpid, &status, 0) > 0 &&
        strcmp(strarr_get(left, 0), "false") != 0) {
      int gpid;
      if ((gpid = fork())) {
        int gstatus;
        waitpid(gpid, &gstatus, 0);
        free_strarr(right);
        free_strarr(left); 
      }
      else {
        char* args[right->size];
  
        for (i = 0; i < right->size; i++) {
          args[i] = strarr_get(right, i);
        }
        args[right->size] = NULL;

        execvp(args[0], args);
      } 
    }
    else {
      free_strarr(right);
      free_strarr(left);
    }
  }
  else {
    char* args[left->size];
    
    for (i = 0; i < left->size; i++) {
      args[i] = strarr_get(left, i);
    }
    args[left->size] = NULL;
    execvp(args[0], args);
  }
}

void
semicolon(strarr* arr, int semicolon_index) 
{
  // create two new strarr's: one for left of semicolon, one for right
  strarr* left = create_strarr();
  strarr* right = create_strarr();

  // add elements to right and left
  int i;
  for (i = 0; i < semicolon_index; i++) {
    strarr_add(left, strarr_get(arr, i)); 
  }
  for (i = semicolon_index + 1; i < arr->size; i++) {
    strarr_add(right, strarr_get(arr, i));
  }
  
  // execute the commands on the left of the semicolon before right
  execute(left);
  execute(right);
  
  free_strarr(left);
  free_strarr(right);
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
      int i; 
      for (i = 0; i < arr->size; i++) {
        if (strcmp(strarr_get(arr, i), "|") == 0) {
          exec_pipe(arr, i);  
          break;
        }
        else if (strcmp(strarr_get(arr, i), ";") == 0) {
          semicolon(arr, i);
          break;
        }
        else if (strcmp(strarr_get(arr, i), "&&") == 0) {
          exec_and(arr, i);
          break;
        }
        else if (strcmp(strarr_get(arr, i), "||") == 0) {
 //         exec_or(arr, i);
          break;
        }
        else if (i == arr->size - 1) {
          execute(arr);
        }
      }
      free_strarr(arr);
    }
    return 0;
}
