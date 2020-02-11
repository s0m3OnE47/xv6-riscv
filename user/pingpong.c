#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
	int pid, parent_fd[2], child_fd[2];
	char *ping = "X", *pong = "Y", *msg = "Z";

	if (argc > 1) {
		fprintf(2, "pingpong: takes no operand\n");
		exit();
	}

	if (pipe(parent_fd) == -1) {
		fprintf(2,"pipe(): Pipe Failed" );
		exit();
	}

	if (pipe(child_fd) == -1)	{
		fprintf(2,"pipe(): Pipe Failed" );
		exit();
	}

	pid = fork();
	if (pid < 0) {
		fprintf(2,"fork(): Fork failed\n");
		exit();
	}
	if (pid == 0) {
		close(parent_fd[1]);	// Close unused write end
		read(parent_fd[0], ping, strlen(msg));	// Child process reads from parent_fd
		printf("%d: received ping\n", getpid());
		close(parent_fd[0]);	// Close unused read end
		close(child_fd[0]);		// Close unused read end
		write(child_fd[1], msg, strlen(msg));	// Child process writes to child_fd
		close(child_fd[1]);		// Close unused write end
	}
	else {
		close(parent_fd[0]);	// Close unused read end
		write(parent_fd[1], msg, strlen(msg));	// Parent process writes to parent_fd
		close(parent_fd[1]);	// Close unused write end
		wait();			// Prevents child becoming zombie
		close(child_fd[1]);		// Close unused write end
		read(child_fd[0], pong, strlen(msg));	// Parent process reads from child-fd
		printf("%d: received pong\n", getpid());
		close(child_fd[0]);		// Close unused read end
		close(child_fd[0]);
	}
	exit();
}
