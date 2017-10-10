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

    if (strcmp(strarr_get(arr, 0), "cd") == 0) {
      chdir(strarr_get(arr, 1));
    }

    // if the inputted command is "exit"
    else if (strcmp(strarr_get(arr, 0), "exit") == 0) {
      exit(0);
    }
    
    else if ((cpid = fork())) {
      int status;

      waitpid(cpid, &status, 0);
        //printf("%d", status);
    }
    else {
      // child process
      char* args[arr->size];

      for (i = 0; i < arr->size; i++) {
        args[i] = strarr_get(arr, i); 
      }

      args[arr->size] = NULL; // null terminate array
      execvp(args[0], args);
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
    }
    return 0;
}
