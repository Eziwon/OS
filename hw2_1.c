// end의 값을 계속해서 늘려나가는 것이 아니라, 공백을 만났을 때의 인덱스로 바로 지정해주는 방식으로 바꿨습니다.
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_TEXT 256
#define MAX_LEN 128
#define SIZE 512

typedef struct {
		int start, end;
		char substr[MAX_LEN];
} Substring;

Substring words[SIZE];
int no_word = 0;

int main() {
		char *prompt = "Input a text: ";
		int prompt_len = strlen(prompt);

		char text[MAX_TEXT] = "";
		printf("%s", prompt);
		fgets(text, MAX_TEXT, stdin);
		int len = strlen(text) - 1;
		text[len] = 0;
		int num = 0;

		for (int i=0; i<prompt_len; i++)
				printf(" ");
		for (int i=0; i<len; i++){
			if(i==0) {
					// previous code
						/*
						printf("[");
						words[no_word].start = i;
						words[no_word].end = i;
						words[no_word].substr[num++] = text[i];
						*/
					// updated code
						printf("[");
						words[no_word].start = i;
						words[no_word].substr[num++] = text[i];
			}
			else if (isspace(text[i])!=0){
				if(isspace(text[i-1])!=0) printf(" ");
				else {
					// previous code
						/*
						printf("]");
						words[no_word].end++;
						no_word++;
						num = 0;
						*/
					// updated code
						printf("]");
						words[no_word].end = i;
						no_word++;
						num = 0;
				}
			}
			else {
					if (isspace(text[i-1])!=0) {
						// previous code
							/*
							printf("[");
							words[no_word].start = i;
							words[no_word].end = i;
							words[no_word].substr[num++] = text[i];
							*/
						// updated code
							printf("[");
							words[no_word].start = i;
							words[no_word].substr[num++] = text[i];
					}
					else {
						// previous code
							/*
							printf(" ");
							words[no_word].end++;
							words[no_word].substr[num++] = text[i];
							*/
						// updated code
							printf(" ");
							words[no_word].substr[num++] = text[i];
					}
			}
		}
		// previous code
		/*
		printf("]");
		words[no_word].end++;
		no_word++;
		*/
		// updated code
		printf("]");
		words[no_word].end = len;
		no_word++;

		putchar('\n');
		putchar('\n');
		for(int i=0; i<no_word; i++)
				printf("words[%d] = (%d, %d, %s)\n", i, words[i].start, words[i].end, words[i].substr);
		
		printf("Bye!\n");

		return 0;
}
