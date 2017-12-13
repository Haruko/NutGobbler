#include "Shared.h"

void SplitString(char* str, char* elements[], char* delims, int max_size)
{
	int i = 0;
	elements[i++] = strtok(str, delims);
	while(i < max_size && (elements[i++] = strtok(NULL, delims)) != NULL);
}

/*void GetRawInput(char* userin, int bufsize, char* prompt, FILE* file, int* error)
{
	printf("%s",prompt);
	int c;
	int count = 0;
	while(count < bufsize - 1 && (c = getc(file)) != '\n')
	{
		if(c == EOF)
		{
			*error = 1;
			printf("\n");
			break;
		}
		userin[count] = c;
		count++;
	}
	userin[count]='\0';
}*/

void SeedRNG()
{
	time_t future = time(NULL) + 1; // One second from now
	int seed = 0;
	while(time(NULL) < future) // Continue looping for an amount of time and use the resulting value of seed as a seed
		++seed;
	
	srand(seed);
}

char *ReadLine(FILE *in)
{
	char *line = NULL;
	int allocSize = 0;

	int numRead = 0;

	char c;
	while((c = getc(in)) != '\n')
	{
		if(numRead == allocSize) // Need to allocate more space
		{
			line = (char *)realloc(line, allocSize + sizeof(char) * BASE_INPUT_LENGTH);
			allocSize += BASE_INPUT_LENGTH;
		}
		line[numRead++] = c;
		if(c == EOF)
			break;
	}

	// Now add the termination character
	if(numRead == allocSize) // Need to allocate more space
	{
		line = (char *)realloc(line, allocSize + sizeof(char));
		++allocSize;
	}
	line[numRead] = '\0';

	if(line[0] == '\0') // Meaning there wasn't actually anything on the line
	{
		free(line);
		return NULL;
	}

	return line;
}

void PadString(char inString[], int inLength, char outString[], int outLength)
{
	if(inLength > outLength)
		return;
	
	char outBuffer[outLength];
	memset(outBuffer, ' ', outLength * sizeof(char));
	memset(outString, '\0', outLength * sizeof(char));
	
	// Cutoff for concatenation
	outBuffer[outLength - inLength] = '\0';
	
	snprintf(outString, outLength, "%s%s", outBuffer, inString);
}
