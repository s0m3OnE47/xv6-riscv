/*

parent      child     parent    child
         ___________         ____________     ...
   2 -->| prime 2   |  3 -->| prime 3    |    ...
   3 -->|           |  5 -->|            |    ...
   4 .  | discard 4 |  7 -->| discard 9  |    ...
   5 .  | discard 6 |  9 -->| discard 15 |    ...
     .  | .         |    .  | .          |    ...
  35 -->| .         | 35 -->| .          |    ...
        | .         |       | .          |    ...
         -----------         ------------     ...
 */

#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
	int MAX_LENGTH = 35;
    int numbers[MAX_LENGTH], counter = 0, i;
    int parent_fd[2];
    int pid,n;
    int prime, firstPrimeOfNewProcess = 0;

	if(argc == 2) {
		MAX_LENGTH = atoi(argv[1]);
	}

	if(argc > 2) {
		printf("prime: takes no more than one operand\n");
		exit();
	}

    for (i = 2; i <= MAX_LENGTH; i++) {
        numbers[counter++] = i;
    }

    while (counter > 0) {		// Exits if EOF is reached
        if (pipe(parent_fd) == -1)  {
            printf("pipe(): Pipe Failed\n" );
            exit();
        }

        pid = fork();
        if (pid < 0) {
            printf("fork(): Fork failed\n");
            exit();
        }

        if (pid == 0) {			// Child discards composite numbers in each loop's run
            counter = -1;		// To test if its the first number in the run
            close(parent_fd[1]);	// Closing unused end
            n=read(parent_fd[0], &prime, sizeof(prime)); 	// n stores 0 if it encounters EOF
	    while (n != 0) {    	// While EOF is reached
                if (counter == -1) {    // Is this the first number of the process?
                    firstPrimeOfNewProcess = prime; 		// If yes, Then store
                    counter++;		// Set counter to 0
                } 
                else {                  // This is not first number in the process, So discard composite numbers
                    if (prime % firstPrimeOfNewProcess != 0)    // Discarding
                        numbers[counter++] = prime;             // Store probable prime numbers
                }
                n=read(parent_fd[0], &prime, sizeof(prime));	
            }
            close(parent_fd[0]);
            printf("prime %d\n", firstPrimeOfNewProcess);
        }
        else {				// Parent writes number for child to read
            close(parent_fd[0]);	// Closing of unused end
            for (i = 0; i < counter; i++) {
                write(parent_fd[1], &numbers[i], sizeof(numbers[0]));
            }
            close(parent_fd[1]);	// Closing of unused end
            wait();			// Reap child resources after child terminates
            break;
        }
    }
    exit();
}
