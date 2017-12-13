// File to store globals, structures, and enumerations used throughout the program.
// Makes it so we don't need to link functions from IPC, for example, just to get MAX_MESSAGE_LENGTH

#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BASE_INPUT_LENGTH 5



/*
	Splits a string into an array of words
	Parameters:
		str - string to read from
		elements - array to store words in
		delims - delimiter to use for parsing
		max_size - maximum number of words
*/
void SplitString(char* str, char* elements[], char* delims, int max_size);

/*
	Seeds the random number generator
*/
void SeedRNG();

/*
	Parameters:
		in - file to read data from, opened elsewhere
	Return:
		line read from in
*/
char *ReadLine(FILE *in);

/*
	Pads a string with spaces in front of it to make if of a certain length
	Parameters:
		inString - string to pad
		inLength - length of string to pad including terminator
		outString - buffer for output string
		outLength - length of output string including terminator
*/
void PadString(char inString[], int inLength, char outString[], int outLength);

#endif
