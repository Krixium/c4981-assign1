/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: 		translate.c - An applicaiton that emulates a terminal with raw input from the keyboard.
--
-- PROGRAM: 			termEmu
--
-- FUNCTIONS:			void parseString(char * dest, const char * src)
--                      void translate(const int * inputPipe, const int * outputPipe)
--
-- DATE: 				January 21, 2018
--
-- DESIGNER: 			Benny Wang
--
-- PROGRAMMER: 			Benny Wang
--
-- NOTES:
-- This file contains all the functions that are involved in translating the user output.
----------------------------------------------------------------------------------------------------------------------*/
#include "translate.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: 			parseString
--
-- DATE: 				January 21, 2018
--
-- DESIGNER: 			Benny Wang
--
-- PROGRAMMER: 			Benny Wang
--
-- INTERFACE: 			void parseString (char * dest, const char * src)
--                          char * dest: A pointer to destination of the parsed string.
--                          const char * src: A pointer to the source of the input string.
--
-- RETURNS: 			void.
--
-- NOTES:
-- This function parses the user's input.
--
-- If 'K' is found, everything before it is discarded.
-- If 'X' is found, the previous character is discarded.
-- If 'a' is found, it is replaced with 'z'.
-- Otherwise, the character is copied over.
--
-- At the end, \r\n is appened with the null character to format the string for displaying to the terminal.
----------------------------------------------------------------------------------------------------------------------*/
void parseString(char * dest, const char * src)
{
	int i;
	int j;

	for (i = 0, j = 0; i < strlen(src) ; i++)
    {
        if (src[i] == 'K')
		{
			j = 0;
		}
        else if (src[i] == 'X')
		{
			j--;
		}
		else if (src[i] == 'a')
		{
			dest[j] = 'z';
			j++;
		}
		else
		{
			dest[j] = src[i];
			j++;
		}
	}

	dest[j] = '\r';
	dest[j + 1] = '\n';
	dest[j + 2] = '\0';
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: 			translate
--
-- DATE: 				January 21, 2018
--
-- DESIGNER: 			Benny Wang
--
-- PROGRAMMER: 			Benny Wang
--
-- INTERFACE: 			void translate (const int * inputPipe, const int * outputPipe)
--							const int * inputPipe: A reference to the file descriptor for the pipe to input.
--							const int * outputPipe: A reference to the file descriptor for the pipe to output.
--
-- RETURNS: 			void.
--
-- NOTES:
-- This function is the main function of one of the child processes. While reading from the input pipe gives no errors,
-- this funciton will pass the input to the parser function and then pass the parsed string to output to be displayed
-- to the user.
----------------------------------------------------------------------------------------------------------------------*/
void translate(const int * inputPipe, const int * outputPipe)
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