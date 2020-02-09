#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
	if(argc <= 1) {
		printf("sleep: missing operand\n");
		exit();
	}
	if(argc > 2) {
		printf("sleep: takes one operand\n");
		exit();
	}

	printf("uptime = %d",uptime());		// Print uptime
	sleep(atoi(argv[1]));			// Converts char to integer
	printf("\nuptime = %d\n",uptime());	// Print uptime

	exit();
}
