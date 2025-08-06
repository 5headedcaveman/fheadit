#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

enum fhedit_input_types {
	FHEDIT_ITYPE_CONTROLS =		0b0000000000000000000000000000000000000000000000000000000000000000,
	FHEDIT_ITYPE_FILE_NAME =	0b0000000000000000000000000000000000000000000000000000000000000001,
	FHEDIT_ITYPE_MESSAGE =		0b0000000000000000000000000000000000000000000000000000000000000010,
	FHEDIT_ITYPE_MAX_ENUM =		0b1111111111111111111111111111111111111111111111111111111111111111 
};

int main(int argc, char *argv[]) {
	struct termios mode = {0};

	tcgetattr(STDIN_FILENO, &mode);

	mode.c_lflag &= ~(ECHO | ICANON);

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &mode);

	char c;
	uint64_t itype = 0;


	uint64_t input_buffer_size = 10; //In bytes
	char *input_buffer = malloc(input_buffer_size);
	
	uint32_t input_length = 0;

	char *file_name = NULL;
	int file_num = -1;

	while(read(STDIN_FILENO, &c, 1) == 1) {
		if(itype == FHEDIT_ITYPE_FILE_NAME && c == '\n') {
			mode.c_lflag &= ~(ECHO);
			tcsetattr(STDIN_FILENO, TCSANOW, &mode);

			file_name = malloc(input_length + 1);
			
			strncpy(file_name, input_buffer, input_length);
			file_name[input_length] = '\0';

			file_num = open(file_name, O_RDWR | O_CREAT, 0644);

			if(file_num == -1) {
				write(STDERR_FILENO, "Open Failed\n", 12);
				break;
			}

			free(input_buffer);
			input_buffer_size = 10;
			input_buffer = malloc(input_buffer_size);
			input_length = 0;
			
			itype = FHEDIT_ITYPE_CONTROLS;
		} else if(itype == FHEDIT_ITYPE_MESSAGE && c == '\n') {
			mode.c_lflag &= ~(ECHO);
			tcsetattr(STDIN_FILENO, TCSANOW, &mode);

			char *message = malloc(input_length + 1);
			
			strncpy(message, input_buffer, input_length);
			message[input_length] = '\n';
			
			lseek(file_num, 0, SEEK_END);
			write(file_num, message, input_length + 1);
			
			free(input_buffer);
			input_buffer_size = 10;
			input_buffer = malloc(input_buffer_size);
			input_length = 0;
			
			itype = FHEDIT_ITYPE_CONTROLS;
		} else if(itype == FHEDIT_ITYPE_CONTROLS) {
			if(c == 'q') {
				break;
			} else if(c == 'f') {
				itype = FHEDIT_ITYPE_FILE_NAME;
				write(STDOUT_FILENO, "File Name: ", 11);
				
				mode.c_lflag |= ECHO;
				tcsetattr(STDIN_FILENO, TCSANOW, &mode);
			} else if(c == 'm' && file_num != -1) {
				itype = FHEDIT_ITYPE_MESSAGE;
				write(STDOUT_FILENO, "Message: ", 9);

				mode.c_lflag |= ECHO;
				tcsetattr(STDIN_FILENO, TCSANOW, &mode);
			} else if(c == 'r' && file_num != -1) {
				int file_length = lseek(file_num, 0, SEEK_END);
				if(file_length == -1)
					break;

				lseek(file_num, 0, SEEK_SET);
				char *buff = malloc(file_length);

				read(file_num, buff, file_length);

				write(STDOUT_FILENO, buff, file_length);
				write(STDOUT_FILENO, "\n", 1);
			} else if(c == 'c' && file_num != -1) {
				int result = close(file_num);
				if(result == -1)
					break;

				free(file_name);
				file_name = NULL;
				file_num = -1;
			} else {
				if(write(STDOUT_FILENO, &c, 1) == -1)
					break;
			}
		} else {
			if(input_length < input_buffer_size) {
				input_buffer[input_length] = c;
				++input_length;
			} else {
				char *old_input_buffer = input_buffer;

				input_buffer_size += 10;
				input_buffer = malloc(input_buffer_size);

				strncpy(input_buffer, (const char *)old_input_buffer, input_buffer_size - 10);

				free(old_input_buffer);

				input_buffer[input_length] = c;
				++input_length;
			}
		}
	}

	mode.c_lflag |= ECHO | ICANON;

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &mode);
	return 0;
}
