/*
    xargs echo hello,
    i am robot

    Flow:
    xargs echo hello,
argv [0]  [1]   [2]
args      [0]   [1]

      i  am  robot (NULL)
args [2] [3]  [4]   [5]

    i am robot
    |||||||||||...
    0|23|56789|...    } argument variable
  NULL..NULL..NULL    } newArgumentPosition variable
     |  |     |       } newArgumentCount increamented by this amount

 */

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[])
{
	char *args[MAXARG];			// MAXARG = 32
	char argument[512];
	int n;
	int tempPointer = 0, newArgumentPosition = 0, newArgumentCount;
	char ch = 0;

	if (argc < 2) {
		fprintf(2, "xargs: invalid arguments\n");
		exit();
	}

	for (int i = 0; i < argc - 1; i++) {
		args[i] = argv[i + 1];					// Shifting arguments by one location
	}

	for (n = 1; n > 0;) {
		tempPointer = 0;
		newArgumentPosition = 0;
		newArgumentCount = argc - 1;
		ch = 0;

		while (1) {
			if ((n = read(0, &ch, 1)) == 0) {		// Read from standard input
				exit();
			}

			if (ch == ' ' || ch == '\n') {
				argument[tempPointer] = 0;			// NULL in ASCII is 0; NULL is stored in space or newline location. Used in next line
				args[newArgumentCount] = &argument[newArgumentPosition];	// assignment operator will copy the position of new argument

				tempPointer++;
				newArgumentCount++;
				newArgumentPosition = tempPointer;	// next argument's address is updated

				if (ch == '\n') {					// Exec when newline. i.e break
					break;
				}
			} else {
				argument[tempPointer] = ch;			// Store each character in argument variable
				tempPointer++;						// Till it encounters space or newline
			}
		}

		args[newArgumentCount] = 0;					// End the command with NULL character

		if (fork() == 0) {
			exec(args[0], args);		// exec will leave this code and go to args[0]'s code for execution'
			fprintf(2, "xargs: exec failed\n");   // If stays here, print error
		} else {						// Parent will wait for child to finish
			wait();
		}
	}
	exit();
}
