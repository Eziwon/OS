// 2분반 22100574 이지원
/*

    This program reads an integer expression from the user and computes the result using a child process.
    The parent and child processes communicate with each other through message passing.
    Implement this program following the instructions.
    Append "[parent]" and "[child]" to all messages in Parent() and Child respectively.

    Example)
        p2c = 0, c2p = 1                // the ids of the message queues
        [parent] Input an integer binary expressions(type "quit" to finish.): 5 + 3     // this user typed "5 + 3"
        [parent] msgsnd() returned = 0
        [child] received 0-th message "5 + 3", ret = 136
        [child] sending 0-th message "8"
        [child] msgsnd() returned = 0
        [parent] msgrcv() returned = 136
        [parent] 5 + 3 = 8
        [parent] Input an integer binary expressions(type "quit" to finish.): 7 *2     // this user typed "7 *2"
        [parent] msgsnd() returned = 0
        [child] received 1-th message "7 *2", ret = 136
        [child] sending 1-th message "14"
        [child] msgsnd() returned = 0
        [parent] msgrcv() returned = 136
        [parent] 7 *2 = 14
        [parent] Input an integer binary expressions(type "quit" to finish.): 9/2         // this user typed "9/2"
        [parent] msgsnd() returned = 0
        [child] received 2-th message "9/2", ret = 136
        [child] sending 2-th message "4"
        [child] msgsnd() returned = 0
        [parent] msgrcv() returned = 136
        [parent] 9/2 = 4
        [parent] Input an integer binary expressions(type "quit" to finish.): quit         // this user typed "quit"
        [parent] msgsnd() returned = 0
        [child] received 3-th message "quit", ret = 136
        Terminating Parent.
        Terminating Child.


    You can use the following Linux commands to list or remove message queues
        ipcs                // show infromation on IPC facilities
        ipcrm -q <msgq-id>    // delete a message queue

    After the program terminates, there should be no remaining message queue.
    Example)
        $ ipcs
        ------ Message Queues --------
        key        msqid      owner      perms      used-bytes   messages

        // nothing

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define _DEBUG

#define MAX_LEN 128

void Parent(int p2c, int c2p);
void Child(int p2c, int c2p);
int Evaluate(char *expression);

#define MSG_P2C 1
#define MSG_C2P 2

typedef struct {
	long type;
	int index;
	char content[MAX_LEN];
} Message;

int main(int argc, char *argv[])
{
	// to do: create two message queues
    //         one is for messages from the parent to the child.
    //         the other is for messages from the child to the parent.
    //         on failure, display an error message and quit.
    //         otherwise, display the ids of the two message queues for debug.
	int id_p2c, id_c2p;
	if (((id_p2c = msgget(MSG_P2C, IPC_CREAT|0666))==-1) || ((id_c2p = msgget(MSG_C2P, IPC_CREAT|0666))==-1)) {
		perror("msgget() error!");
		exit(-1);
	}

	printf("p2c = %d, c2p = %d\n", id_p2c, id_c2p);

	// to do: create a child process
    //      on failure, display an error message, destroy the message queues, and terminate.
    //      In the parent process, call the Parent() function passing the two message queues.
    //      In the child process, call the Child() function passing the two message queues.
	pid_t child = fork();
	if (child < 0) {	// error checking
		perror("Failed to create a child process!");
		exit(-1);
	}
	else if (child > 0)	// parent process
		Parent(id_p2c, id_c2p);
	else 	// child process
		Child(id_p2c, id_c2p);

	return 0;
}

void Parent(int p2c, int c2p)
// TO DO: implement this function
//         read integer expressions from the user
//         send the expression to the child process by message passing
//        receive the result from the child by message passing
//        display the expression and result
{
	Message out_msg, in_msg;
	size_t msg_size = sizeof(Message) - sizeof(long);
	out_msg.type = MSG_P2C;
	out_msg.index = 0;
    
    // Algorithm)
    //    repeat until the user types "quit"
    //         read an integer expression as a string (e.g., "5+3", "7 * 5", etc.) - use fgets()
    //         if the input is not an empty string,
    //             send the expression to the child through the message queue p2c.
    //                 on failure, display an error message.
    //            for debug, display the return value of msgsnd().
    //             if the input string is "quit", break the loop.
    //             receive the result of the expression from the child through the message queue c2p.
    //                on failure, display an error message.
    //            for debug, display the return value of msgrcv().
    //             display the expression and the result from the child.
    //             increase the index of the outgoing message.
    //
    //    destroy the two message queues.
    //    display a message indicating that the parent is terminating.

	while(1) {
		printf("[parent] Input an integer binary expression(type \"quit\" to finish.): ");
		if (fgets(out_msg.content, MAX_LEN, stdin) != NULL) {
			out_msg.content[strlen(out_msg.content)-1] = '\0';
			int snd_r;
			if ((snd_r = msgsnd(p2c, &out_msg, msg_size, 0))==-1) perror("[parent] msgsnd() error!\n");
			printf("[parent] msgsnd() returned = %d\n", snd_r);

			if (strcmp(out_msg.content, "quit")==0) break;

			int rcv_r = msgrcv(c2p, &in_msg, msg_size, 0, 0);
			if (rcv_r == -1) perror("[parent] msgrcv() error!\n");
			printf("[parent] msgrcv() returned = %d\n", rcv_r);
			printf("[parent] %s = %s\n", out_msg.content, in_msg.content);
			out_msg.index++;
		}
	}
	if (msgctl(c2p, IPC_RMID, 0) == -1) perror("msgctl() error!\n");
	if (msgctl(p2c, IPC_RMID, 0) == -1) perror("msgctl() error!\n");
	printf("Terminating Parent.\n");
}

void Child(int p2c, int c2p)
// TO DO: implement this function
//         receive an integer expressions from the parent by message passing
//        compute the result of the expression
//         send the result to the parent process by message passing
{
	Message in_msg, out_msg;
	out_msg.type = MSG_C2P;
	out_msg.index = 0;
	size_t msg_size = sizeof(Message) - sizeof(long);
    
    // Algorithm)
    //     repeat until the incomming message is "quit"
    //         receieve an expression from the parent through the message queue p2c.
    //             on failure, display an error message.
    //         for debug, display the incoming message, its index, and the return value of msgrcv().
    //         if the message is "quit", break the loop
    //         compute the evaluation in the incoming message
    //         for debug, display the outcoming message and its index.
    //         send the result as a string to the parent through the message queue c2p.
    //             on failure, display an error message.
    //        for debug, display the return value of msgsnd().
    //         increase the index of outgoing message.
    //
    //    display a message indicating that the child is terminating.

	while(1){
		int rcv_r = msgrcv(p2c, &in_msg, msg_size, 0, 0);
		if (rcv_r == -1) perror("[child] msgrcv() error!\n");
		printf("[child] received %d-th message \"%s\", ret = %d\n", in_msg.index, in_msg.content, rcv_r);
		if (strcmp(in_msg.content, "quit")==0) break;
		sprintf(out_msg.content,"%d", Evaluate(in_msg.content)) ;
		
		printf("[child] sending %d-th message \"%s\"\n", out_msg.index, out_msg.content);
		int snd_r = msgsnd(c2p, &out_msg, msg_size, 0);
		if (snd_r == -1) perror("[child] msgsnd() error!\n");
		printf("[child] msgsnd() returned = %d\n", snd_r);
		out_msg.index++;
	}
	printf("Terminating Child.\n");
}

int Evaluate(char *expression)
// TO DO: return the result of the expression
//        separate the operator and operands from the expression by calling the sscanf() function. (if necessary, search the Internet for sscanf().)
//            (the expression is composed of a left operand, an operator (+, - , *, or /), and a right operand., e.g., "5/2", "10 * 3", "7- 9")
//            for the division operator (/), return zero if the denominator is zero.
//        if the operator and the two operands were retrieved, compute the expression and return the result.
//        otherwise, return zero.
{
	int a, b;
	char str;

	sscanf(expression, "%d %c %d", &a, &str, &b);
	switch (str){
		case '+' : 
			return  a+b;
		case '-' : 
			return a-b;
		case '*' :
			return a * b;
		case '/' :
			if (b == 0) return 0;
			return a / b;
		default:
			return 0;
	}
}
