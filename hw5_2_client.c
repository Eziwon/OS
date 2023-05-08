#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 256

int main(int argc, char *argv[])
{
	if(argc < 2){
		printf("Usage: %s <server-ip> <server-port> <client-port>\n", argv[0]);
		return -1;
	}

	char *server_ip = argv[1];
	int server_port = atoi(argv[2]);

	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == -1){
		perror("failed to create a socket\n");
		exit(1);
	}

	printf("Connecting to %s:%d\n", server_ip, server_port);

	struct sockaddr_in svr_addr = { 0 };
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_port = htons(server_port);
	svr_addr.sin_addr.s_addr = inet_addr(server_ip);

	if (connect(clientSocket, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) == -1){
		perror("connect() error!\n");
		exit(1);
	}

	printf("Done.\n");

	char input[64] = "";
	
	while(1){
		char result[BUFFER_SIZE] = "";
		printf("Input a number: ");
		fgets(input, 64, stdin);
		input[strlen(input)-1] = '\0';
		if (write(clientSocket, input, sizeof(input)) == -1)
			perror("write() error!\n");
		if (read(clientSocket, result, BUFFER_SIZE) == -1)
			perror("read() error!\n");
		printf("%s ==> %s\n", input, result);
		if (result[0] == 0) break;
	}

	printf("Closing socket\n");

	close(clientSocket);

	printf("Done.\n");

	printf("Bye!\n");

	return 0;
}
