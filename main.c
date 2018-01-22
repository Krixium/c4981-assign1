
/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: 		main.c - An applicaiton that emulates a terminal with raw input from the keyboard.
--
-- PROGRAM: 			termEmu
--
-- FUNCTIONS:			int main(int argc, char * argv[])
--
-- DATE: 				January 21, 2018
--
-- DESIGNER: 			Benny Wang
--
-- PROGRAMMER: 			Benny Wang
--
-- NOTES:
-- This program puts the terminal into raw mode and emulates the keyboard input functionality. This is done with three
-- processes, one for input, one for output, and one for parsing user intput.
--
-- Some functions that are different from a normal terminal:
--		E 	- Enter
--		T 	- Normal termination of the program
--		^K	- Abnormal termination of the program
--
-- All user input is also passed to a function that parses it. When an X deletes the previous character, K is line-clear
-- and all instances of 'a' is converted to 'z'.
----------------------------------------------------------------------------------------------------------------------*/
#include "main.h"
#include "io.h"

pid_t pid[PID_COUNT];

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: 			main
--
-- DATE: 				January 21, 2018
--
-- DESIGNER: 			Benny Wang
--
-- PROGRAMMER: 			Benny Wang
--
-- INTERFACE: 			int main (int argc, char * argv[])
--							int argc: The amount of commandline arguements passed to the program. Not used.
--							char * argv[]: List of command line arguements passed to the program. Not used.
--
-- RETURNS: 			An exit code.
--
-- NOTES:
-- The main entry function of the program. This function is where all pipes and processes are created. The reading end
-- of the pipes that connect input with output, and translate with output, are set to no wait. The two processes this
-- function creates are created as children to the main proccess(fan style). If the process is the parent process, it
-- also manages the closing of the pipes and sending exit signals to the two children processes when the user wants to
-- end execution of the program.
----------------------------------------------------------------------------------------------------------------------*/
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

	/* Set no delay for the reading ends of pipes that go to output */
	if (fcntl(pipes[1][0], F_SETFL, O_NDELAY) < 0)
	{
		fprintf(stderr, "Error setting flag on pipe\n");
		return EXIT_FAILURE;
	}
	if (fcntl(pipes[2][0], F_SETFL, O_NDELAY) < 0)
	{
		fprintf(stderr, "Error setting flag on pipe\n");
		return EXIT_FAILURE;
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
	else if (i == 1)
	{
		/* Second child */
		translate(pipes[0], pipes[2]);
	}
	else if (i == 2)
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
