#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void Digit2Text(char *digit_string, char *alphabet_str);

int main(int argc, char *argv[])
{
	if(argc < 2){
		printf("Usage: %s <server-port>\n", argv[0]);
		return 0;
	}

	int ret = 0;
	int port = atoi(argv[1]);
	printf("port = %d\n", port);

	printf("creating socket...\n");

	if ((ret = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("failed to create a socket\n");
		exit(1);
	}

	printf("Done.\n");

	struct sockaddr_in svr_addr = { 0 };
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_port = htons(port);
	svr_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	printf("socket binding...\n");

	if (bind(ret, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) == -1) {
		perror("bind() error!\n");
		exit(1);
	}

	printf("Done.\n");

	printf("listening socket...\n");

	if (listen(ret, 5) == 1) {
		perror("listen() error!\n");
		exit(1);
	}

	printf("Done.\n");

	struct sockaddr_in client_addr = { 0 };
	memset(&client_addr, 0, sizeof(client_addr));
	socklen_t client_addr_len = 0;

	printf("Waiting for connect request...\n");

	client_addr_len = sizeof(client_addr);
	int clientSocket = accept(ret, (struct sockaddr *)&client_addr, &client_addr_len);
	if (clientSocket == -1) {
		perror("accept() error!\n");
		exit(1);
	}

	printf("Connected.\n");

	char input[64] = "";

	while(1){
		char output[512] = "";
		if (read(clientSocket, input, sizeof(input)) == -1)
			perror("read() error!\n");
		if (strcmp(input, "quit") == 0) break;
		printf("input = [%s]\n", input);
		Digit2Text(input, output);
		printf("output = [%s]\n", output);
		printf("Sending message...\n");
		if (write(clientSocket, output, strlen(output)) == -1)
			perror("write() error!\n");
		printf("Done.\n");
	}

	printf("Closing sockets\n");

	close(clientSocket);
	close(ret);

	printf("Done.\n");

	printf("Bye!\n");

	return 0;
}

void Digit2Text(char *digit_str, char *alphabet_str)
{
	static char digit_name[][16] = {
		"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"
	};

	if (digit_str[0] == 0){
		strcpy(alphabet_str, "");
		return;
	}
	int len = strlen(digit_str);
	for (int i=0; i<len; i++) {
		if (digit_str[i] >= '0' && digit_str[i] <= '9'){
			if (i != 0) 
				strcat(alphabet_str, " ");
			strcat(alphabet_str, digit_name[digit_str[i] - '0']);
		}
		else {
			strcpy(alphabet_str, "");
			return;
		}
	}
}
