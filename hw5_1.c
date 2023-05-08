#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define	_DEBUG

#define TRUE 1
#define FALSE 0

#define P2C 0				
#define C2P 1				

#define MAX_LEN 128

typedef struct {
	int is_loaded;			
	char buffer[MAX_LEN];	
} SharedBuffer;

void* Parent(void *arg);
void* Child(void *arg);
int Evaluate(char *expression);

int main(int argc, char *argv[])
{
		int seg_id;
		key_t key = ftok(".", 'S');
		ssize_t size = sizeof(SharedBuffer)*2;
		SharedBuffer *shm_buffer;
		if ((seg_id = shmget(key, size, IPC_CREAT|0666)) == -1) {
				printf("shmget() error!\n");
				return 0;
		}
		if ((shm_buffer = shmat(seg_id, 0, 0)) == (SharedBuffer *)-1) {
				printf("shmat() error!\n");
				shmctl(seg_id, IPC_RMID, 0);
				return 0;
		}
		printf("shmid = %d\n", seg_id);

	shm_buffer[P2C].is_loaded = FALSE;
	shm_buffer[P2C].buffer[0] = 0;
	shm_buffer[C2P].is_loaded = FALSE;
	shm_buffer[C2P].buffer[0] = 0;

	pid_t child = fork();
	if (child < 0) {
			printf("fork() error!\n");
			shmdt(shm_buffer);
			shmctl(seg_id, IPC_RMID, 0);
			return 0;
	}
	else if (child == 0) {
			Child(shm_buffer);
			shmdt(shm_buffer);
			exit(0);
	}
	else {
			Parent(shm_buffer);
			shmdt(shm_buffer);
			shmctl(seg_id, IPC_RMID, 0);
	}

	system("ipcs");		
						
	printf("Bye!\n");

	return 0;
}

void* Parent(void *arg)

{
	SharedBuffer *shm_buffer = (SharedBuffer *)arg;

	char expression[MAX_LEN] = "";
	char result[MAX_LEN] = "";

	while(1){
			printf("[parent] Input an integer binary expressions(type \"quit\" to finish.): ");
			fgets(expression, MAX_LEN, stdin);
			if (expression != NULL) {
					expression[strlen(expression) - 1] = '\0';

					while(shm_buffer[P2C].is_loaded == TRUE);
					strcpy(shm_buffer[P2C].buffer, expression);
					shm_buffer[P2C].is_loaded = TRUE;

					if (strcmp(expression, "quit") == 0) break;

					while(shm_buffer[C2P].is_loaded == FALSE);
					strcpy(result, shm_buffer[C2P].buffer);
					shm_buffer[C2P].is_loaded = FALSE;
					printf("[parent] %s = %s\n", expression, result);
			}
	}

	printf("Terminating Parent.\n");
}

void* Child(void *arg)
{
	SharedBuffer *shm_buffer = (SharedBuffer *)arg;

	char expression[MAX_LEN] = "";
	char result[MAX_LEN] = "";

	while(1) {
			while(shm_buffer[P2C].is_loaded == FALSE);
			strcpy(expression, shm_buffer[P2C].buffer);
			shm_buffer[P2C].is_loaded = FALSE;
			printf("[child] received expression \"%s\"\n", expression);
			if (strcmp(expression, "quit") == 0) break;
			sprintf(result, "%d", Evaluate(expression));
			printf("[child] sending result \"%s\"\n", result);
			while(shm_buffer[C2P].is_loaded == TRUE);
			strcpy(shm_buffer[C2P].buffer, result);
			shm_buffer[C2P].is_loaded = TRUE;
	}
	printf("Terminating Child.\n");
}

int Evaluate(char *expression)
{
	int op1 = 0, op2 = 0;
	char op = 0;
	int result = 0;

	int ret = sscanf(expression, "%d %c %d", &op1, &op, &op2);
	if(ret == 3){
		switch(op){
		case '+':
			result = op1 + op2;
			break;

		case '-':
			result = op1 - op2;
			break;

		case '*':
			result = op1 * op2;
			break;

		case '/':
			if(op2 != 0)
				result = op1 / op2;
			break;
		}
	}

	return result;
}

