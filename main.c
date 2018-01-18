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


void input(const int translatePipe, const int outputPipe)
{
	char buffer[BUFFER_SIZE];
	fprintf(stderr, "This is input with pipe id's %d and %d\n", translatePipe, outputPipe);
}


void output(const int inputPipe, const int translatePipe)
{
	char buffer[BUFFER_SIZE];
	fprintf(stderr, "This is output with pipe id's %d and %d\n", inputPipe, translatePipe);
}


void translate(const int inputPipe, const int outputPipe)
{
	char buffer[BUFFER_SIZE];
	fprintf(stderr, "This is translate with pipe id's %d and %d\n", inputPipe, outputPipe);
}


int main(int argc, char * argv[])
{
	/* Initialize variables */
	pid_t childpid;
	int i;

	/* 
	 Pipe 0: input to translate
	 Pipe 1: input to output
	 Pipe 2: translate to output
	 */
	int pipes[3]; 
	
	/* Create pipes */
	if (pipe(pipes) < 0)
	{
		fprintf(stderr, "Error creating pipes\n");
		return EXIT_BAD;
	}

	/* Create processes as a chain */
	for (i = 0; i < CHILD_PROCESSES_COUNT; i++)
	{
		if ((childpid = fork()) <= 0)
		{
			break;
		}	
	}

	/* Determine which processes it is */
	if (childpid != 0)
	{
		fprintf(stdout, "childpid: %ld			GETPID: %ld			GETPPID: %ld\n",
				(long)childpid, (long)getpid(), (long)getppid());
		input(pipes[0], pipes[1]);
	}
	else if (childpid == 0 && getpid() - getppid() == 1)
	{
		fprintf(stdout, "childpid: %ld			GETPID: %ld			GETPPID: %ld\n",
				(long)childpid, (long)getpid(), (long)getppid());
		output(pipes[1], pipes[2]);
	}
	else if (childpid == 0 && getpid() - getppid() == 2)
	{
		fprintf(stdout, "childpid: %ld			GETPID: %ld			GETPPID: %ld\n",
				(long)childpid, (long)getpid(), (long)getppid());
		translate(pipes[0], pipes[2]);
	}
	else
	{
		fprintf(stderr, "Error in creating process\n");
		return EXIT_BAD;
	}
	
	return EXIT_GOOD;
}
