#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>

#define try bool __HadError=false;
#define catch(x) ExitJmp:if(__HadError)
#define throw(x) {__HadError=true;goto ExitJmp;}

#define BUFFER_SIZE 8192

int next_block_size(int count, int buffer_size) {
	return (count >= buffer_size)? buffer_size: count % buffer_size;
}

int main (int argc, char* argv[]) {
	try {

		if (argc == 1) {
			printf("usage: phrases [-l] file");
			return 0;
		}

		int file_fd = open(argv[argc - 1], O_RDONLY);
		if (file_fd == -1) {
			printf("error: cannot open %s\n", argv[argc - 1]);
			exit(EXIT_FAILURE);
		}

		struct stat info;
		int ret = lstat(argv[argc - 1], &info);

		if (ret == -1) {
			printf("error: cannot stat %s\n", argv[argc - 1]);
			exit(EXIT_FAILURE);
		}

		int count = info.st_size;
		char buffer[BUFFER_SIZE];
		int numLines = 1;
		char delimit[] = ".!?";
		while (count != 0) {
			int bytesin = read(file_fd, buffer, next_block_size(count, BUFFER_SIZE));
			char bufferAux[BUFFER_SIZE];
			strcpy(bufferAux, buffer);
			int countAux = 0;
			count -= bytesin;			
			char * ptr = strtok(buffer, delimit);

			while (ptr != NULL) {
				countAux += strlen(ptr);
				if (ptr[0] == ' ') 
    					memmove(ptr, ptr+1, strlen(ptr));
				char * aux = strchr(ptr, '\n');
				if (aux) *aux = ' ';
				if (argc == 3) {
					printf("[%i] %s%c\n", numLines, ptr, bufferAux[countAux]);
				}
				numLines++;
				ptr = strtok(NULL, delimit);
				countAux += 1;
			};
		}

		if (argc == 2)		
			printf("%i\n", numLines - 1);
		close(file_fd);
		return 0;	

	} catch() {
		perror("exec error");
		exit(EXIT_FAILURE);
	}

}