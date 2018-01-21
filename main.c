#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define PID_COUNT 2
#define BUFFER_SIZE 5120 

pid_t pid[PID_COUNT];


void input(const int translatePipe, const int outputPipe)
{
	char c;
	char buffer[BUFFER_SIZE];
	int i = 0;

	memset(buffer, 0, BUFFER_SIZE);

	while ((c = getchar()))
	{
		/* ^K send ASCII 11 */
		if (c == (char)11)
		{
			kill(0, SIGQUIT);
		}
		else if (c == 'T')
		{
			break;
		}
		else if (c == 'E')
		{
			write(translatePipe[1], buffer, strlen(buffer));
			fsync(translatePipe[1]);
			memset(buffer, 0, i);
			i = 0;
		}
		else
		{
			write(outputPipe[1], &c, 1);
			fsync(outputPipe[1]);
			buffer[i] = c;
			i++;
		}
	}
}


void output(const int inputPipe, const int translatePipe)
{
	char iBuffer;
	char tBuffer[BUFFER_SIZE];
	int iRead;
	int tRead;
	
	fcntl(inputPipe[0], F_SETFL, O_NDELAY);
	fcntl(translatePipe[0], F_SETFL, O_NDELAY);

	for (;;)
	{
		iRead = read(inputPipe[0], &iBuffer, 1);
		tRead = read(translatePipe[0], tBuffer, BUFFER_SIZE);

		if (iRead > 0)
		{
			fprintf(stdout, "%c", iBuffer);
			iBuffer = '\0';
		}
		if (tRead > 0)
		{
			fprintf(stdout, "\r\n%s\r\n", tBuffer);
			memset(tBuffer, 0, BUFFER_SIZE);
		}
		fflush(stdout);
	}
}


void parseString(char * dest, const char * src)
{
	int i;
	int j;

	for (i = 0, j = 0; i < strlen(src) ; i++)
	{
		if (src[i] == 'a')
		{
			dest[j] = 'z';
			j++;
		}
		else if (src[i] == 'K')
		{
			j = 0;
		}
		else if (src[i] == 'X')
		{
			j--;
		}
		else
		{
			dest[j] = src[i];
			j++;
		}
	}

	dest[j] = '\0';
}


void translate(const int inputPipe, const int outputPipe)
{
	int i;
	int j;
	char buffer[BUFFER_SIZE];
	char outputString[BUFFER_SIZE];

	while (read(inputPipe[0], buffer, BUFFER_SIZE))
	{
		parseString(outputString, buffer);	
		memset(buffer, 0, BUFFER_SIZE);
		write(outputPipe[1], outputString, strlen(outputString));
	}
}


int main(int argc, char * argv[])
{
	/* 
	 Pipe 0: input to translate
	 Pipe 1: input to output
	 Pipe 2: translate to output
	 */
	int pipes[3][2]; 
	int i;
	
	/* Open pipes */
	for (i = 0; i < 3; i++)
	{
		if (pipe(pipes[i]) == -1)
		{
			fprintf(stderr, "Opening pipe failed\r\n");
			return EXIT_FAILURE;
		}
	}

	/* Disable terminal functions */
	system("stty raw igncr -echo");

	/* Create processes */
	for (i = 0; i < PID_COUNT; i++)
	{
		pid[i] = fork();
		/* If this is the child process then don't fork */
		if (pid[i] == 0)
		{
			break;
		}	
	}

	if (i == 0)
	{
		/* First child */
		output(pipes[1], pipes[2]);
	}
	else if (childpid == 0 && getpid() - getppid() == 1)
	{
		/* Second child */
		translate(pipes[0], pipes[2]);
	}
	else if (childpid == 0 && getpid() - getppid() == 2)
	{
		/* Parent */
		input(pipes[0], pipes[1]);

		/* End child proccesses */
		kill(pid[0], SIGTERM);
		kill(pid[1], SIGTERM);

		/* Close pipes */
		for (i = 0; i < 3; i++)
		{
			close(pipes[i][0]);
			close(pipes[i][1]);
		}
	}
	else
	{
		fprintf(stderr, "Error during forking\n", i);
	}

	system("stty -raw -igncr echo");

	return EXIT_SUCCESS;
}
