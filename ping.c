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

volatile sig_atomic_t exitting = false;
volatile pid_t pid;

void handle_sigint (int signum) {
	exitting = true;
}

void handle_sigterm (int signum) {
	exitting = true;
}

void handle_sigrtmin(int signum, siginfo_t* info, void* extra) {
	if (pid == info->si_pid) {
		const int value = info->si_value.sival_int;
		printf("ping %d\n", value);
		fflush(stdout);
		sigqueue(info->si_pid, SIGRTMIN, (union sigval) {.sival_int = value + 1});
	}
}

int main (int argc, char *argv[]) {
	pid = atoi(argv[1]);
	printf("pid: %d\n", getpid());
	fflush(stdout);
	printf("ping %d\n", 0);
	fflush(stdout);
	sigqueue(pid, SIGRTMIN, (union sigval) {.sival_int = 1});
	
	struct sigaction action_int;
	memset(&action_int, 0, sizeof(action_int));
	action_int.sa_handler = handle_sigint;
	action_int.sa_flags = SA_RESTART;
	sigaction(SIGINT, &action_int, NULL);
	
	struct sigaction action_rtmin;
        memset(&action_rtmin, 0, sizeof(action_rtmin));
        action_rtmin.sa_sigaction = handle_sigrtmin;
        action_rtmin.sa_flags = SA_RESTART | SA_SIGINFO;
        sigaction(SIGRTMIN, &action_rtmin, NULL);

	struct sigaction action_term;
	memset(&action_term, 0, sizeof(action_term));
	action_term.sa_handler = handle_sigterm;
	action_term.sa_flags = SA_RESTART;
	sigaction(SIGTERM, &action_term, NULL);
	
	while(!exitting);
	kill(pid, SIGTERM);	
	return 0;	
}
