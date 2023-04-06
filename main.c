// Name:			Pa'lante
// Description:		An 8-bit Forth interpreter
// Author			The Geek on Skates
// Website:			http://www.geekonskates.com
// License:			To be decided
// =============================================================================

// -----------------------------------------------------------------------------
// DEPENDENCIES
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <peekpoke.h>



// -----------------------------------------------------------------------------
// MACROS
// -----------------------------------------------------------------------------

/**
 * Checks is a string starts with a substring
 * @param[in] The string to be tested
 * @param[in] The string we're looking for
 * @returns True if string a starts with string b,
 * or false if not
 */
#define StringStartsWith(a, b) (strstr(a, b) == a) 



// -----------------------------------------------------------------------------
// GLOBAL VARIABLES
// -----------------------------------------------------------------------------

uint16_t ds[256], dsp,
	rs[256], rsp;
char input[80];
char* ip;



// -----------------------------------------------------------------------------
// FORTH WORDS
// -----------------------------------------------------------------------------

void printStack() {
	// .S
	static uint8_t i;
	printf("<%d> ", dsp);
	for (i=0; i<dsp; i++)
		printf("%d ", ds[i]);
}

void dup() {
	// DUP
	ds[dsp] = ds[dsp - 1];
	dsp++;
}

void swap() {
	// SWAP
	static uint8_t n;
	n = ds[dsp - 2];
	ds[dsp - 2] = ds[dsp - 1];
	ds[dsp - 1] = n;
}

void over() {
	// OVER
	ds[dsp] = ds[dsp - 2];
	dsp++;
}

void emit() {
	// EMIT
	dsp--;
	printf("%lc", ds[dsp]);
}

void drop() {
	// DROP
	dsp--;
}

void pop() {
	// .
	dsp--;
	printf("%d ", ds[dsp]);
}



// -----------------------------------------------------------------------------
// TO BE SORTED
// -----------------------------------------------------------------------------

/**
 * Checks if a string contains a signed whole number
 * (with or withou leading sign)
 * @param The string to be tested
 * @returns True if it is, false if it isn't
 */
bool IsNumber(char* word) {
	static uint8_t i, length;
	if (strcmp(word, "-") == 0 || strcmp(word, "+") == 0)
		return false;
	length = strlen(word);
	i = 0;
	if (word[0] == '-' || word[0] == '+') i++;
	for (; i<length; i++)
		if (word[i] < '0' || word[i] > '9')
			return word[i] == ' ';
	return true;
}

void main() {
	static int8_t i;
	while(true) {
		// Get user input
		memset(input, 0, 80);
		fgets(input, 80, stdin);
		
		// Push all the words the user typed onto the return stack
		for (i=79; i>-1; i--) {
			
			// Move past any unset chars
			while (i>0 && input[i] == '\0') i--;
			
			// Convert \n to a space for easier parsing
			if (input[i] == '\n') input[i] = ' ';
			
			// Move past any trailing spaces
			while (i>=0 && input[i] == ' ') i--;
			
			// Move to the space before the start of the word (if there is one)
			while (i>=0 && input[i] != ' ') i--;
			
			// If there is one, move up 1 char to go to the start of the word
			if (i == -1 || input[i] == ' ') i++;
			
			// And push it to the return stack
			if (i >= 0 && input[i] != ' ') {
				rs[rsp] = input + i;
				rsp++;
			}
		}
		
		// And run it
		while(rsp > 0) {
			// Pop the return stack to the input pointer
			rsp--;
			ip = (char*)rs[rsp];

			// Handle Forth words
			if (StringStartsWith(ip, ".s ")) {
				printStack();
				continue;
			}
			if (StringStartsWith(ip, ". ")) {
				pop();
				continue;
			}
			if (StringStartsWith(ip, "dup ")) {
				dup();
				continue;
			}
			if (StringStartsWith(ip, "drop ")) {
				drop();
				continue;
			}
			if (StringStartsWith(ip, "emit ")) {
				emit();
				continue;
			}
			if (StringStartsWith(ip, "over ")) {
				over();
				continue;
			}
			if (StringStartsWith(ip, "swap ")) {
				swap();
				continue;
			}
			if (IsNumber(ip)) {
				ds[dsp] = atoi(ip);
				dsp++;
				continue;
			}
		}
	}
}
