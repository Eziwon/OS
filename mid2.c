/*

	This program reads an integer expression from the user and computes the result using a child process.
	The parent and child processes communicate with each other through shared memory.
	Implement this program following the instructions.
	Append "[parent]" and "[child]" to all messages in Parent() and Child() respectively.

	Example)
`		shmid = 10
		[parent] Input an integer binary expressions(type "quit" to finish.): 5 + 3
		[child] received expression "5 + 3"
		[child] sending result "8"
		[parent] 5 + 3 = 8
		[parent] Input an integer binary expressions(type "quit" to finish.): 7 *2
		[child] received expression "7 *2"
		[child] sending result "14"
		[parent] 7 *2 = 14
		[parent] Input an integer binary expressions(type "quit" to finish.): 9/2
		[child] received expression "9/2"
		[child] sending result "4"
		[parent] 9/2 = 4
		[parent] Input an integer binary expressions(type "quit" to finish.): quit
		[child] received expression "quit"
		[parent] Terminating Parent.
		[child] Terminating Child.


	You can use the following Linux commands to list or remove shared memory blocks
		ipcs				// show infromation on IPC facilities
		ipcrm -m <shm-id>	// delete a shared memory block


	After the program terminates, your program should not leave a shared memory block
	Example)
		$ ipcs
		...
		------ Shared Memory Segments --------
		key        shmid      owner      perms      bytes      nattch     status
		0x00000000 2          lightdm    600        524288     2          dest
		0x00000000 3          lightdm    600        33554432   2          dest

		// no shared memory block created by the user

*/

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

/*
	Transferring data through SharedBuffer

	sender:
		wait until is_loaded becomes FALSE
		store data in buffer
		set is_loaded to TRUE
		
	receiver:
		wait until is_loaded becomes TRUE
		retrieve data from buffer
		set is_loaded to FALSE

*/

void* Parent(void *arg);
void* Child(void *arg);
int Evaluate(char *expression);

int main(int argc, char *argv[])
{
    // TO DO: allocate an array composed of two SharedBuffer instances in a shared memory block
	//		the size of shared memory block should be sizeof(SharedBuffer) * 2.
	// 		one is for data transfer from the parent to the child.
	// 		the other is for data transfer from the child to the parent.
	//		call shmget() and shmat()
	// 		on failure, display an error message, deallocate the shared memory block if necessary, and quit.
	// 		otherwise, display the id of the shared memory block for debug.


    /*교수님 코드
    int shmid = shmget(IPC_PRIVATE, sizeof(SharedBuffer) * 2, S_IRUSR | S_IWUSR);
    if (seg_id == -1) {
        printf("shmget() error!\n");
        exit(-1);
    }
        
    SharedBuffer * shm_buffer = (SharedBuffer *)shmat(shmid, NULL, 0);
    if (shm_buffer == NULL){
        printf("shmget() error!\n");
        return 0;
    }
    */
    
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

    // initialize shm_buffer[0] and shm_buffer[1] (set the is_loaded fields to FALSE and the buffer fields to an empty string.)
	shm_buffer[P2C].is_loaded = FALSE;
	shm_buffer[P2C].buffer[0] = 0;
	shm_buffer[C2P].is_loaded = FALSE;
	shm_buffer[C2P].buffer[0] = 0;
    

    // TO DO: create a child process
	// 		on failure, display an error message, detatch and deallocate the shared memory block, and terminate.
	//		In the child process,
	//			call the Child() function passing the address to the SharedBuffer array.
	//			detatch the shared memory block
	//			call exit(0) for safety
	// 		In the parent process,
	//			call the Parent() function passing the address to the SharedBuffer array.
	//			detatch and destroy the shared memory block

    /*
     교수님 코드
    pid_t child = fork();
    switch(child){
        case -1:           // error
            printf("fork() error!\n");
            shmdt(shm_buffer);
            shmctl(seg_id, IPC_RMID, NULL);
            break;
        
        case 0:             // child
            Child(shm_buffer)
            shmdt(shm_buffer);
            exit(0);
            break;
            
        default:            // parent
            Parent(shm_buffer);
            shmdt(shm_buffer);
            shmctl(seg_id, IPC_RMID, NULL);
            break;
    }
    */
    
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
// TO DO: implement this function
// 		read integer expressions from the user
// 		send the expression to the child process through the shareed memory buffer
//		receive the result from the child through the shared memory buffer
//		display the expression and result
{
    // Algorithm)
	//	repeat until the user types "quit"
	// 		read an integer expression as a string (e.g., "5+3", "7 * 5", etc.) - use fgets()
	// 		if the input is not an empty string,
	// 			send the expression to the child through shm_buffer[P2C]
	// 				wait until the is_loaded field is FALSE
	//				copy the expression into the buffer field
	//				set the is_loaded field to TRUE
	// 			if the input string is "quit", break the loop
	// 			receive the result of the expression from the child through shm_buffer[C2P]
	//				wait until the is_loaded field is TRUE
	//				copy the result from the buffer field
	//				set the is_loaded field to FALSE
	// 			display the expression and the result from the child.
	//	display a message indicating that the parent is terminating.
    
	SharedBuffer *shm_buffer = (SharedBuffer *)arg;

	char expression[MAX_LEN] = "";
	char result[MAX_LEN] = "";

/*
교수님 코드
    while(1) {
        printf("[parent] Input an integer binary expressions(type \"quit\" to finish.): ");
        fgets(expression, MAX_LEN, stdin);
        expression[strlen(expression)-1] = 0;
        if (expression[0] != 0){
            while(shm_buffer[P2C].is_loaded == TRUE);
            strcpy(shm_buffer[P2C].buffer, expression);
            shm_buffer[P2C].is_loaded = TRUE;

            if (strcmp(expression, "quit") == 0)
                break;
            
            while(shm_buffer[C2P].is_loaded == FALSE);
            strcpy(result, shm_buffer[C2P].buffer);
            shm_buffer[C2P].is_loaded = FALSE;
            printf("[parent] %s = %s\n", expression, result);
        }
    }
*/

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

/*
교수님 코드
    while(1) {
        // recieve an expression from the parent
        while(shm_buffer[P2C].is_loaded == FALSE);
        strcpy(expression, shm_buffer[P2C].buffer);
        shm_buffer[P2C].is_loaded = FALSE;
        
        // if the expression is "quit", break the loop
        if (strcmp(expression, "quit") == 0)
            break;
        
        // evaluate the expression
        int v = Evaluate(expression);
        sprintf(result, "%d", v);

        // send the result to the parent
        while(shm_buffer[C2P].is_loaded == TRUE);
        strcpy(shm_buffer[C2P].buffer, result);
        shm_buffer[C2P].is_loaded = TRUE;
    }
*/

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

