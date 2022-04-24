#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

int server_fd, client_fd, socket_fd;

void shutdown_server() {
	printf("\nShutting down...\n");
	fflush(stdout);
	if (client_fd != -1) {
		shutdown(client_fd, SHUT_RDWR);
		close(client_fd);
	}

	if (server_fd != -1) {
		shutdown(server_fd, SHUT_RDWR);
		close(server_fd);
	}
	if (socket_fd != -1) {
		shutdown(socket_fd, SHUT_RDWR);
		close(socket_fd);
	}
	exit(0);
}

void handle_sigint (int signum) {
	shutdown_server();
}

void handle_sigterm (int signum) {
	shutdown_server();
}

void attach_signal() {
	signal(SIGPIPE, SIG_IGN);

	sigaction(SIGINT, &(struct sigaction) {
			.sa_handler = handle_sigint,
			.sa_flags = SA_RESTART
			}, NULL);

	sigaction(SIGTERM, &(struct sigaction) {
			.sa_handler = handle_sigterm,
			.sa_flags = SA_RESTART
			}, NULL);
}

void wheel(int client_fd) {
	while (true) {
		char first_receive[4096];
		memset(first_receive, '\0', sizeof(first_receive));

		if(read(client_fd, first_receive, sizeof(first_receive)) <= 0) {
			break;
		}
		printf("received %lu bytes from client\n", strlen(first_receive));
        	fflush(stdout);

       		char first_send[4096];
		memset(first_send, '\0', sizeof(first_send));
		strcat(first_send, first_receive);
		if(write(socket_fd, first_send, sizeof(first_send)) <= 0) {
                        break;
                }
		
		char second_receive[4096];
		memset(second_receive, '\0', sizeof(second_receive));
		if(read(socket_fd, second_receive, sizeof(second_receive)) <= 0) {
			break;
		}

		printf("received %lu bytes from server\n", strlen(second_receive));
		fflush(stdout);
 		
		char second_send[4096];
		memset(second_send, '\0', sizeof(second_send));
                strcat(second_send, second_receive);
                if(write(client_fd, second_send, sizeof(second_send)) <= 0) {
                        break;
                }
	}
}

int main (int agrc, char *argv[]) {
	attach_signal();
	in_port_t client_port = htons(strtol(argv[1], NULL, 10));
	in_port_t server_port = htons(strtol(argv[2], NULL, 10));
	printf("pid: %d\n", getpid());
	fflush(stdout);
	printf("Press ^C to exit\n\n");
	fflush(stdout);

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	///Connect to server
	if (socket_fd == -1) {
		shutdown_server();
	}

	struct sockaddr_in server_address = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = inet_addr("127.0.0.1"),
		.sin_port = server_port
	};

	if (-1 == connect(socket_fd, (const struct sockaddr*)(&server_address), sizeof(server_address))) {
		shutdown_server();
	} else {
		printf("Connected to the server\n");
		fflush(stdout);
	}

	///Connect to client
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		shutdown_server();
	}

	int setsockopt_value = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &setsockopt_value, sizeof(setsockopt_value));
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &setsockopt_value, sizeof(setsockopt_value));

	struct sockaddr_in client_address = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = INADDR_ANY,
		.sin_port = client_port
	};

	if (-1 == bind(server_fd, (const struct sockaddr*)(&client_address), sizeof(server_address))) {
		shutdown_server();
	}

	listen(server_fd, 1);
	struct sockaddr_in new_client_address;
	socklen_t client_address_length;
	client_fd = accept(server_fd, (struct sockaddr*)(&new_client_address), &client_address_length);

	if (client_fd == -1) {
		shutdown_server();
	}

	printf("Client connected\n");
	wheel(client_fd);
	shutdown_server();
}
