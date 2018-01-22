/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: 		io.c - An applicaiton that emulates a terminal with raw input from the keyboard.
--
-- PROGRAM: 			termEmu
--
-- FUNCTIONS:			void input(const int * trainslatePipe, const int * outputPipe)
--
-- DATE: 				January 21, 2018
--
-- DESIGNER: 			Benny Wang
--
-- PROGRAMMER: 			Benny Wang
--
-- NOTES:
-- This file contains all the functios that interact with the user.
----------------------------------------------------------------------------------------------------------------------*/
#include "io.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: 			input
--
-- DATE: 				January 21, 2018
--
-- DESIGNER: 			Benny Wang
--
-- PROGRAMMER: 			Benny Wang
--
-- INTERFACE: 			void input (const int * translatePipe, const int * outputPipe)
--                          const int * translatePipe: A reference to the file descriptor for the pipe to translate.
--                          const int * outputPipe: A reference to the file descriptor for the pipe to output.
--
-- RETURNS: 			void.
--
-- NOTES:
-- This function is the main input function. It will get characters from the user and search for control characters. If
-- no control characters are found, it then saves the character to a buffer and sends the same character to output so
-- that it can be echoed to the user.
--
-- Control characters include:
--      ^K  : Abnormal termination
--      T   : Normal termination
--      E   : Send buffer to translate and \r\n to output
----------------------------------------------------------------------------------------------------------------------*/
void input(const int * translatePipe, const int * outputPipe)
{
	const char r = '\r';
	const char n = '\n';

	char c;
	char buffer[BUFFER_SIZE];
	int i = 0;

	while ((c = getchar()))
	{
		/* ^K is ASCII 11 */
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
			write(outputPipe[1], &r, 1);
			write(outputPipe[1], &n, 1);
			write(translatePipe[1], buffer, strlen(buffer));
			memset(buffer, 0, i);
			i = 0;
		}
		else
		{
			write(outputPipe[1], &c, 1);
			buffer[i] = c;
			i++;
		}
	}
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: 			output
--
-- DATE: 				January 21, 2018
--
-- DESIGNER: 			Benny Wang
--
-- PROGRAMMER: 			Benny Wang
--
-- INTERFACE: 			void output (const int * inputPipe, const int * translatePipe)
--                          const int * inputPipe: A reference to the file descriptor for the pipe from input.
--                          const int * translatePipe: A reference to the file descriptor for the pipe from translate.
--
-- RETURNS: 			void.
--
-- NOTES:
-- This function handles the displaying text to the user.
--
-- This function will constantly check the two pipes for characters. If a character is found from the input pipe, it
-- simply displays the character. If characters are found from translate, it displays the string and clears its buffer.
----------------------------------------------------------------------------------------------------------------------*/
void output(const int * inputPipe, const int  * translatePipe)
{
	char iBuffer;
	char tBuffer[BUFFER_SIZE];
	int iRead;
	int tRead;

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
			fprintf(stdout, "%s", tBuffer);
			memset(tBuffer, 0, BUFFER_SIZE);
		}
		fflush(stdout);
	}
}