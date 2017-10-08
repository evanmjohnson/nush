#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "tokens.h"
void
execute(char* cmd, strarr* arr)
{
    int cpid;

    if ((cpid = fork())) {
        char* args[arr->size];

        // add individual tokens to an args array.
        for (int i = 0; i < arr->size; i++) {
          args[i] = strarr_get(arr, i);
        }  

        int status;
        waitpid(cpid, &status, 0);

        if (WIFEXITED(status)) {
          // TODO: error code reporting
        }
    }
    else {
        // child process
        char* args[arr->size];

        for (int i = 0; i < arr->size; i++) {
          args[i] = strarr_get(arr, i);
        }

        execvp(args[0], args);
    }
}

// tokenize cmd and pass that into the execute function
int
main(int argc, char* argv[])
{
    char cmd[256];

    if (argc == 1) {
      while(1) {
        printf("nush$ ");
        fflush(stdout);

        char* rv = fgets(cmd, 256, stdin);
        if (!rv) {
          break;
        }
        strarr* arr = get_tokens(cmd);
        execute(cmd, arr);
      }
    }
    else {
        memcpy(cmd, "echo", 5);
    }

//    strarr* arr = get_tokens(cmd);
//    execute(cmd, arr);

    return 0;
}
