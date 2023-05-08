// 2분반 22100574 이지원
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
#define FALSE 0

int CheckAndCreateDirectory(char *file_path);
int SplitCommandLine2(char *cmd, char *argv[]);

char working_dir[256] = "Working/";

int main()
{
		printf("Welcome to myshell.\n");
		printf("All commands will be executed in \"%s\".\n", working_dir);
		printf("Type \"quit\" to quit.\n");

		int ret = CheckAndCreateDirectory(working_dir);
		if(ret == FALSE){
			printf("Failed to find or create directory \"%s\".\n", working_dir);
			return -1;
		}

		// To Do: move into the working directory (call chdir())
		//		on failure, displat an error message
    
        // previous code
        /*
		int ch = chdir(working_dir);
		if (ch == -1) printf("Failed to change directory.\n");
        */
    
        // updated code
        // 기존에 2줄로 작성했던 코드를 한 줄로 줄이기 + directory 바꾸기를 실패하면 코드 실행 중단하는 기능 추가하기
        if (chdir(working_dir) == -1) {
            printf("Failed to change directory.\n");
            exit(-1);
        }
    
		while(1){
			char cmd[256] = "";
			int argc = 0;
			char *argv[128];

			//read a command line
			printf("$ ");
			fgets(cmd, 256, stdin);
			cmd[strlen(cmd) - 1] = 0;

			// if the command is "quit", break the loop
			if(strcmp(cmd, "quit") == 0)
					break;

			argc = SplitCommandLine2(cmd, argv);

#ifdef	_DEBUG
			printf("argc = %d\n", argc);
			for (int i=0; i<argc; i++)
					printf("argv[%d] = %s\n", i, argv[i]);
			printf("argv[%d] = %s\n", argc, argv[argc]);

#endif	//	_DEBUG
			// TO DO: if argc is greater than zero, run the command using fork() and execvp()
			if (argc > 0) {
				pid_t child = fork();
				if (child < 0) {
					printf("Failed to create a child process.\n");
					exit(-1);
				}
				else if (child == 0) {	// child process
					int ex = execvp(argv[0], argv);
					if (ex == -1){
						printf("Failed to exe\n");
						exit(-1);
					}
					else exit(0);
				}
				else wait(NULL);
			}
		}
		printf("Bye!\n");

		return 0;
}


int CheckAndCreateDirectory(char *file_path)
{
	static char subdir[MAX_PATH] = "";
	for(int i=0; i<strlen(file_path); i++) {
		if (file_path[i] == '/'){
			strncpy(subdir, file_path, i);
			subdir[i] = '\0';
			
			if (access(subdir, F_OK) == -1){
				if (mkdir(subdir, 0755) == 0) printf("Directory %s was created.\n", subdir);
				else {
					printf("Failed to create derectory \"%s\".", subdir);
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}


int SplitCommandLine2(char *cmd, char *argv[])
{
// TO DO: cut the command into arguments
	int argc = 0;
	int len = strlen(cmd);
	for (int i=0; i<len; i++){
		if (cmd[i] == '\"' || cmd[i] == '\''){
			i++;
			argv[argc] = cmd+i;
			while(cmd[i]!='\0' && cmd[i]!='\"' && cmd[i]!='\''){
				if (cmd[i] == '/') argv[argc] = cmd+i+1;
				i++;
			}
			argc++;
			if (cmd[i] == '\0') i--;
			else cmd[i] = 0;
			if (strcmp(argv[argc-1], "..") == 0) strcpy(argv[argc-1], ".");
		}
		else{
			if (isspace(cmd[i]) == 0) {	
				argv[argc] = cmd + i;
				while(cmd[i] != '\0' && isspace(cmd[i]) == 0){
					if (cmd[i] == '/') argv[argc] = cmd+i+1;
					i++;
				}
				argc++;
				if (cmd[i] == '\0') i--;
				else cmd[i] = 0;
				if (strcmp(argv[argc-1], "..") == 0) strcpy(argv[argc-1], ".");
			}	
		}
	}
	argv[argc] = NULL;
	return argc;
}
