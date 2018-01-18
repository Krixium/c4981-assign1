#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define EXIT_GOOD 0
#define EXIT_BAD 1
#define EXIT_TERMINATE 2
#define EXIT_KILL 3

#define CHILD_PROCESSES_COUNT 2
#define BUFFER_SIZE 5120

/* Read from 0, write to 1 */

void input(const int * translatePipe, const int * outputPipe)
{
	const char * msg = "Hello World";
	char buffer[BUFFER_SIZE];
	fprintf(stderr, "This is input with pipe id's %p and %p\n", (void*)translatePipe, (void*)outputPipe);
	write(translatePipe[1], msg, 12);
	write(outputPipe[1], msg, 12);
}


void output(const int * inputPipe, const int * translatePipe)
{
	char buffer[BUFFER_SIZE];
	fprintf(stderr, "This is output with pipe id's %p and %p\n", (void*)inputPipe, (void*)translatePipe);
	if (read(inputPipe[0], buffer, 14))
	{
		fprintf(stdout, "output: %s\n", buffer);
		fflush(stdout);
	}
	else
	{
		fprintf(stderr, "Error reading from pipe in output\n");
	}
}


void translate(const int * inputPipe, const int * outputPipe)
{
	char buffer[BUFFER_SIZE];
	fprintf(stderr, "This is translate with pipe id's %p and %p\n", (void*)inputPipe, (void*)outputPipe);
	read(inputPipe[0], buffer, 12);
	if (read(inputPipe[0], buffer, 12))
	{
		fprintf(stdout, "translate: %s", buffer);
		fflush(stdout);
	}
	else
	{
		fprintf(stderr, "Error reading from pipe in translate\n");
	}
}


int main(int argc, char * argv[])
{
	/* Initialize variables */
	pid_t pid[CHILD_PROCESSES_COUNT];
	int i;

	/* 
	 Pipe 0: input to translate
	 Pipe 1: input to output
	 Pipe 2: translate to output
	 */
	int pipes[3][2]; 
	
	for (i = 0; i < CHILD_PROCESSES_COUNT; i++)
	{
		pipe(pipes[i]);
		/* If this is the child process then don't fork */
		if ((pid[i] = fork()) == 0)
		{
			break;
		}
	}

	if (i == 0)
	{
		/* First child */
		input(pipes[0], pipes[1]);
	}
	else if (i == 1)
	{
		/* Second child */
		output(pipes[1], pipes[2]);
	}
	else if (i == 2)
	{
		/* parent */
		translate(pipes[0], pipes[2]);
	}
	else
	{
		fprintf(stderr, "Error during forking i: %d\n", i);
	}

	return EXIT_GOOD;
}
