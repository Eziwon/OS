// 2분반 22100574 이지원
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define TRUE 1
#define FALSE 2

#define MAX_PATH 256
#define BUFFER_SIZE 512

int CheckAndCreateDirectory(char *file_path);
int CopyFile(char *src_path, char *dest_path);

int main(int argc, char *argv[]){
		// display the command line arguments
		printf("%d arguments:\n", argc);
		for (int i=0; i < argc; i++)
				printf("argv[%d] = %s\n", i, argv[i]);

		//if argc < 3, display the usage and terminate.
		if (argc < 3) {
				printf("Usage: %s <src_path> <dest_path>\n", argv[0]);
				return 0;
		}
		
		char *src_path = argv[1];
		char *dest_path = argv[2];

		// test if the source file exists and is readible.
		if (access(src_path, R_OK) != 0) {
				printf("Error! File %s does not exist or is not readible!\n", src_path);
				exit(-1);
		}

		// check whether the destination directory exits. otherwise, create the directory
		printf("Preparing directory...\n");
		int success = CheckAndCreateDirectory(dest_path);
		printf("result = %d\n", success);

		// copy file
		printf("Copying file %s to %s\n", src_path, dest_path);
		if (success)
				success = CopyFile(src_path, dest_path);

		// on success, run 'ls -al <dest_path>' by calling the system function
		if (success) {
				printf("%s was successfully copied to %s\n", src_path, dest_path);

				char cmd[MAX_PATH] = "";
				sprintf(cmd, "ls -al %s", dest_path);
				system(cmd);
		} else {
				printf("Failed to copy %s to %s!\n", src_path, dest_path);
		}

		return 0;
}

int CheckAndCreateDirectory(char *file_path)
{
		static char subdir[MAX_PATH] = "";

		for (int i=0; i<strlen(file_path); i++) {
				if (file_path[i] == '/') {
						strncpy(subdir, file_path, i);
						subdir[i] = '\0';

						if (access(subdir, W_OK) == 0) printf("Found Directory %s.\n", subdir);
						else {
								if (mkdir(subdir, 0766) == 0) printf("Directory %s was created.\n", subdir);
								else {
										printf("Error!\n");
										return FALSE;
								}
						}
				}
		}
		return TRUE;
}

int CopyFile(char *src_path, char *dest_path)
{
		int src = open(src_path, O_RDONLY, 0);
		printf("src = %d\n", src);

		// on failure, display an error message and return FALSE
		if (src < 0) {
				printf("Failed to open Source File!\n");
				return FALSE;
		}

		int dest = open(dest_path, O_WRONLY | O_CREAT, 0644);
		printf("dest = %d\n", dest);

		// on failure, display an error message and return FALSE
		if (dest < 0) {
				printf("Failed to open Destination!\n");
				return FALSE;
		}

		static char buffer[BUFFER_SIZE];
		ssize_t read_size = 0, written_size = 0;

		while(1) {
				read_size = read(src, buffer, BUFFER_SIZE);

				// 피드백: read, write 함수도 error checking 추가해주면 좋을 것 같다.
				// updated code
				if (read_size == -1) printf("Error! Failed to read a src File.\n");

				if (read_size == 0) {
						printf("read size = %ld, written size = %ld\n", read_size, written_size);
						break;
				}
				written_size = write(dest, buffer, read_size);
				// updated code
				if (written_size == -1) printf("Error! Failed to write dest File.\n");

				printf("read size = %ld, written size = %ld\n", read_size, written_size);
		}
		close(src);
		close(dest);
		return TRUE;
}
