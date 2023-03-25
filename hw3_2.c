#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>

#define _DEBUG

#define MAX_PATH 256

#define TRUE 1
#define FALSE 2

int CheckAndCreateDirectory(char *file_path);
int SplitCommandLine2(char *cmd, char argv[]);

char working_dir[256] = "Working/";

int main() 
{
    printf("Welcome to myshell.\n");
    printf("All commands will be executed in \"%s\".\n", working_dir);
    printf("Type \"quit\" to quit.\n");

    int ret = CheckAndCreateDirectory(working_dir);
    it (ret==FALSE) {
        printf("Failed to find or create directory \"%s\".\n", working_dir\);
        return -1;
   }


   while(1) {
        char cmd[256] = "";
        int argc = 0;
        char *argv[128];

        printf("$ ");
        fgets(cmd, 256, stdin);
        cmd[strlen(cmd) - 1] = 0;

        if (strcmp(cmd, "quit") == 0)
            break;

        argc = SplitCommandLine2(cmd, argv);
    
ifdef   _DEBUG
        printf("argc = %d\n", argc);
        
        for (int i=0; i<argc; i++)
            printf("argv[%d] = %s\n", i, argv[i]);
        printf("argv")
   }
}