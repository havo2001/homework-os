#define _GNU_SOURCE
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

///function to get the number
int extract_number(char* buffer, int count) {
        int i = count;
        int char_count = 0;
        int remark;
        while (char_count < 3) {
                if (buffer[i] == ':') {
			char_count++;
			if (char_count == 2)
				remark = i;
		}
                i--;
        }
        char digit[remark - i];
        int k = 0;
        for (int j = i + 2; j < remark; ++j) {
                digit[k] = buffer[j];
                k++;
        }
        int number = atoi(digit);
        return number;
}

///count
void count(int* lines, bool counted) {
	if(!counted) 
		(*lines) += 1;
}

///parse the log
void parse(char* buffer, size_t size) {
	bool run = true;
        int line_of_warning = 0;
        int line_of_error = 0;
        int pointer = 0;
	bool warning[100000] = {false};
	bool error[100000] = {false};

        while (run) {
		if (((strstr(buffer + pointer + 1, "warning:") && strstr(buffer + pointer + 1, "error:")) 
		&& (strlen(strstr(buffer + pointer + 1, "warning:")) > strlen(strstr(buffer + pointer + 1, "error:"))))
		|| (strstr(buffer + pointer + 1, "warning:") && !strstr(buffer + pointer + 1, "error:"))) {
			char* first = strstr(buffer + pointer + 1, "warning:");
                        pointer = size - strlen(first);
                        int number = extract_number(buffer, pointer);
                        count(&line_of_warning, warning[number]);
                        warning[number] = true;
		} else if (((strstr(buffer + pointer + 1, "warning:") && strstr(buffer + pointer + 1, "error:"))
                && (strlen(strstr(buffer + pointer + 1, "warning:")) < strlen(strstr(buffer + pointer + 1, "error:"))))
                || (!strstr(buffer + pointer + 1, "warning:") && strstr(buffer + pointer + 1, "error:"))) {
			char* second = strstr(buffer + pointer + 1, "error:");
                        pointer = size - strlen(second);
                        int number = extract_number(buffer, pointer);
                        count(&line_of_error, error[number]);
                        error[number] = true;
		} else {
			break;
		}
	}
	printf("Unique lines with warnings: %d\n", line_of_warning);
        printf("Unique lines with errors: %d\n", line_of_error);
}

int main(int argc, char *argv[]) {
        int fd[2];
        pipe(fd);
        pid_t pid = fork();
        if (pid == 0) {
		pid_t pid2 = fork();
		if (pid2 == 0) {
                        ///child process
                        close(fd[0]);
                        dup2(fd[1], 2);
                        execlp("gcc","gcc", argv[1],"-o","executable", NULL);
                } else {
                        waitpid(pid2, NULL, 0);
			struct stat stat_instance;
			if (lstat("executable", &stat_instance) != -1) { 
                        	execlp("unlink", "unlink", "executable", NULL);
			}
                }
        }
        else {
                ///Parent process
                waitpid(pid, NULL, 0);
                close(fd[1]);
	        const size_t buffer_size = 100000;
                char buffer[buffer_size];
                size_t size = read(fd[0], buffer,  buffer_size);
		parse(buffer, size);
	}
   return 0;
}
