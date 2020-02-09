#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
	static char buf[DIRSIZ + 1];
	char *p;

	// Find first character after last slash.
	for (p = path + strlen(path); p >= path && *p != '/'; p--)
		;
	p++;

	// Return blank-padded name.
	if (strlen(p) >= DIRSIZ)
		return p;
	memmove(buf, p, strlen(p));
	//memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
	return buf;
}

void
find(char *path, char *toFind)
{
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if ((fd = open(path, 0)) < 0) {					// Open the file with RDONLY permission
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}

	if (fstat(fd, &st) < 0) {					// Returns information about a file.
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return;
	}

	switch (st.type) {
	case T_FILE:							// If file
		if ((strcmp(fmtname(buf), toFind)) == 0) {		// comparing toFind with nuf name
			printf("%s\n", buf);
		}
		break;

	case T_DIR:
		if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {	// If path length + Max filename size > 512 
			printf("find: path too long\n");			// print error 
			break;
		} 

		strcpy(buf, path);					// copies path to buf
		p = buf + strlen(buf);					// p navigates to last letter of the path
		*p++ = '/';						// and appends '/' at the end
		while (read(fd, &de, sizeof(de)) == sizeof(de)) {	// read name by name 
			if (de.inum == 0 || (strcmp(de.name, ".") == 0) || strcmp(de.name, "..") == 0)	// Ignore if inode number = 0 or if it encounters "." or ".." 
				continue;				// Ignoring
			memmove(p, de.name, DIRSIZ);			// Moving name to pointer p
			p[DIRSIZ] = 0;					// Setting 14th char as NULL
			if (stat(buf, &st) < 0) {			// Stat for file info
				printf("find: cannot stat %s\n", buf);
				continue;
			}
			if (st.type == 1) {				// If directory
				if ( (strcmp(de.name, toFind)) == 0 ) {	// If directory is found
					printf("%s\n", buf);		// print path
				}

				else {					// else recursion with new path
					find(buf, toFind);
				}
			}
			else if( (st.type == 2) && (strcmp(de.name,toFind) == 0)){
				printf("%s\n",buf );			// If file type, and name matches, then print the path
			}
		}
		break;
	}
	return;
}

int main(int argc, char *argv[]) {

	if (argc < 2 || argc > 3) { 			// No operands passed
		fprintf(2,"find: invalid arguments\n");
		exit();
	}

	if (argc == 2) {				// If one operand is passed, then it should be
		find(".", argv[1]);			// the file to be searched in root directory
	}

	if (argc == 3) {				// First argument is path to search,
		find(argv[1], argv[2]);			// second argument is the file to be searched
	}
	exit();
}
