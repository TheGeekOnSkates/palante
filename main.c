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

// Build targets

#define VIC20 false
#define C64 false

/**
 * Checks is a string starts with a substring
 * @param[in] The string to be tested
 * @param[in] The string we're looking for
 * @returns True if string a starts with string b,
 * or false if not
 */
#define StringStartsWith(a, b) (strstr(a, b) == a) 

// Misc. constants

#define INPUT_SIZE 80
#define STACK_SIZE 256
#define DICT_SIZE 4096


// Status codes

#define STATUS_OK				0		// No error, "ok"
#define STATUS_COMPILED			1		// All words compiled successfully
#define STATUS_STACK_UNDERFLOW	2		// Stack underflow error
#define STATUS_DIV_BY_ZERO		3		// Division by 0 error
#define STATUS_UNKNOWN_WORD		4		// Unknown word error



// -----------------------------------------------------------------------------
// GLOBAL VARIABLES
// -----------------------------------------------------------------------------

int16_t ds[STACK_SIZE], dsp,	// Data stack and its pointer
	rs[STACK_SIZE], rsp;		// Return stack and its pointer
uint16_t goWhere;				// Used to make EXECUTE work
uint8_t status = STATUS_OK;		// System status
char input[INPUT_SIZE];			// User input buffer
char* ip;						// Interpreter pointer
char dictionary[DICT_SIZE];		// The list of user-defined words
bool compiling = false,			// Whether or not we're compiling
	isName = false;				// Set to true if the next word is the name of a
								// word in a : definition



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
	if (!dsp) { status = STATUS_STACK_UNDERFLOW; return; }
	ds[dsp] = ds[dsp - 1];
	dsp++;
}

void swap() {
	// SWAP
	static uint8_t n;
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	n = ds[dsp - 2];
	ds[dsp - 2] = ds[dsp - 1];
	ds[dsp - 1] = n;
}

void over() {
	// OVER
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	ds[dsp] = ds[dsp - 2];
	dsp++;
}

void emit() {
	// EMIT
	if (!dsp) { status = STATUS_STACK_UNDERFLOW; return; }
	dsp--;
	printf("%lc", ds[dsp]);
}

void drop() {
	// DROP
	if (!dsp) { status = STATUS_STACK_UNDERFLOW; return; }
	dsp--;
}

void pop() {
	// .
	if (!dsp) { status = STATUS_STACK_UNDERFLOW; return; }
	dsp--;
	printf("%d ", ds[dsp]);
}

void fetch() {
	// @
	if (!dsp) { status = STATUS_STACK_UNDERFLOW; return; }
	ds[dsp - 1] = PEEKW(ds[dsp]);
}

void cfetch() {
	// C@
	if (!dsp) { status = STATUS_STACK_UNDERFLOW; return; }
	ds[dsp - 1] = PEEK(ds[dsp - 1]);
}

void store() {
	// @
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	POKEW(ds[dsp - 1], ds[dsp - 2]);
	dsp -= 2;
}

void cstore() {
	// C!
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	POKE(ds[dsp - 1], ds[dsp - 2]);
	dsp -= 2;
}

void execute() {
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	dsp--;
	goWhere = ds[dsp - 1];
	asm("JSR _goWhere");
}

void add() {
	// +
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	dsp--;
	ds[dsp - 1] += ds[dsp];
}

void subtract() {
	// - 
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	dsp--;
	ds[dsp - 1] -= ds[dsp];
}

void multiply() {
	// *
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	dsp--;
	ds[dsp - 1] *= ds[dsp];
}

void divide() {
	// /
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	dsp--;
	if (!ds[dsp]) { status = STATUS_DIV_BY_ZERO; return; }
	ds[dsp - 1] /= ds[dsp];
}

void mod() {
	// /
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	dsp--;
	if (!ds[dsp]) { status = STATUS_DIV_BY_ZERO; return; }
	ds[dsp - 1] %= ds[dsp];
}

void lshift() {
	// LSHIFT
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	ds[dsp - 2] = ds[dsp - 2] << ds[dsp - 1];
	dsp--;
}

void rshift() {
	// RSHIFT
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	ds[dsp - 2] = ds[dsp - 2] >> ds[dsp - 1];
	dsp--;
}

void and() {
	// AND
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	ds[dsp - 2] = ds[dsp - 2] & ds[dsp - 1];
	dsp--;
}

void or() {
	// OR
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	ds[dsp - 2] = ds[dsp - 2] | ds[dsp - 1];
	dsp--;
}

void xor() {
	// XOR
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	ds[dsp - 2] = ds[dsp - 2] ^ ds[dsp - 1];
	dsp--;
}

void bye() {
	exit(0);
}

void equals() {
	// =
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	ds[dsp - 2] = ds[dsp - 2] == ds[dsp - 1] ? -1 : 0;
	dsp--;
}

void notEqual() {
	// =
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	ds[dsp - 2] = ds[dsp - 2] != ds[dsp - 1] ? -1 : 0;
	dsp--;
}

void gt() {
	// >
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	ds[dsp - 2] = ds[dsp - 2] > ds[dsp - 1] ? -1 : 0;
	dsp--;
}

void gte() {
	// >=
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	ds[dsp - 2] = ds[dsp - 2] >= ds[dsp - 1] ? -1 : 0;
	dsp--;
}

void lt() {
	// <
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	ds[dsp - 2] = ds[dsp - 2] < ds[dsp - 1] ? -1 : 0;
	dsp--;
}

void lte() {
	// <=
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	ds[dsp - 2] = ds[dsp - 2] <= ds[dsp - 1] ? -1 : 0;
	dsp--;
}

void depth() {
	// DEPTH
	ds[dsp] = dsp;
	dsp++;
}

void invert() {
	// INVERT
	if (!dsp) { status = STATUS_STACK_UNDERFLOW; return; }
	ds[dsp - 1] *= -1;
	ds[dsp - 1]--;
}

void negate() {
	// NEGATE
	if (!dsp) { status = STATUS_STACK_UNDERFLOW; return; }
	ds[dsp - 1] *= -1;
}

void pick() {
	static uint8_t n;
	if (!dsp) { status = STATUS_STACK_UNDERFLOW; return; }
	n = ds[dsp - 1];
	ds[dsp - 1] = ds[dsp - n - 2];
}

void roll() {
	static int16_t j;
	static uint8_t n;
	n = ds[dsp - 1];
	ds[dsp - 1] = ds[dsp - n - 2];
	ds[dsp - 1] = ds[dsp - n - 2];
	for (j = n + 1; j >= 0; j--)
		ds[dsp - j - 1] = ds[dsp - j];
	dsp--;
}

void type() {
	static char* string;
	static int16_t j;
	if (dsp < 2) { status = STATUS_STACK_UNDERFLOW; return; }
	string = (char*)ds[dsp - 2];
	for (j=0; j<ds[dsp - 1] * 2; j++) {
		printf("%c", string[j]);
	}
	dsp -= 2;
}

void accept() {
	static int8_t i;
	
	// Get user input
	memset(input, 0, INPUT_SIZE);
	fgets(input, INPUT_SIZE, stdin);
	
	// Convert \n or Shift-space to a space for easier parsing
	for (i=0; i<INPUT_SIZE; i++)
		if (input[i] == '\n' || input[i] == 160) input[i] = ' ';
	
	// Push all the words the user typed onto the return stack
	for (i=INPUT_SIZE - 1; i>-1; i--) {
		
		// Move past any unset chars
		while (i>0 && input[i] == '\0') i--;
		
		// Move past any trailing spaces
		while (i>=0 && input[i] == ' ') i--;
		
		// Move to the space before the start of the word (if there is one)
		while (i>=0 && input[i] != ' ') i--;
		
		// If there is one, move up 1 char to go to the start of the word
		if (i == -1 || input[i] == ' ') i++;
		
		// And push it to the return stack
		if (i >= 0 && input[i] != ' ') {
			rs[rsp] = (uint16_t)(input + i);
			rsp++;
		}
	}
	
}



// -----------------------------------------------------------------------------
// TO BE SORTED
// -----------------------------------------------------------------------------

void clearCompiledWord() {
	static char start[83];
	static char* definition, * end;
	
	// Build the start of the definition ("\t " + word)
	memset(start, 0, 81);
	start[0] = '\t';
	strcat(start, ip);
	
	// NULL the end of the test string
	definition = strchr(start, ' ');
	definition[0] = '\0';
	definition = NULL;
	
	// If that string is not in our dictionary, do nothing
	definition = strstr(dictionary, start);
	if (definition == NULL) return;
	
	// Otherwise, find the end of that definition
	end = strstr(definition + 1, "\t");
	if (end == NULL) return;	// Should never happen, but don't crash if it does
	
	// Copy over it and delete everything after it
	// 
	// **** LEFT OFF HERE ****
	// 
	// Here's where we once again get to Hacky McMathski's territory. :D
	// I'm too tired to algorithmicize and depuzzle and play brain Tetris. :D
	// But this is what needs to happen..... somehowz..... :)
	strcpy(definition, end);
	
	// And let the user know what's up
	start[0] = ' ';
	printf("Redefined%s\n", start);
}

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

/**
 * Checks if the current word is in the dictionary, and puts its definition on
 * the return stack if it is
 * @returns True if the word is in the dictionary, false if it isn't
 */
bool InDictionary() {
	// Build a string: \t + current word + space
	static char wordName[82], * temp;
	static uint16_t i;
	memset(wordName, 0, 82);
	wordName[0] = '\t';
	temp = ip; i = 0;
	while(temp[0] != ' ' && temp[0] != '\n') {
		wordName[i + 1] = temp[0];
		i++; temp++;
	}
	i++;
	wordName[i] = ' ';
	
	// If this combo is not found in the dictionary, do nothing
	temp = strstr(dictionary, wordName);
	if (temp == NULL) return false;
	
	// If it gets here, it IS in the dictionary, so push every word in its
	// definition onto the return stack.  To get there, I need to do like I did
	// in the main loop.  First, move to the end of the definition:
	temp++;
	while(temp[0] != '\t') temp++;	// End of the definition
	temp--;		// so it's not at the closing \t anymore
	
	// Now again, like I did there, work backward till it reaches the word in ip
	while(temp[0] != '\t') {
		while(temp[0] == ' ') temp--;
		while(temp[0] != ' ' && temp[0] != '\t') temp--;
		if (temp[0] == '\t') break;
		temp++;
		rs[rsp] = (uint16_t)temp;
		rsp++;
		temp--;
	}
	
	return true;
}

void main() {
	static uint16_t length;
	
	// For now, I'm gonna put some test data in my dictionary.
	// Once I have my code finding and running compiled words, then I'll add
	// the : and ; words so users can add/edit (see my previous attempt for how)
	strcpy(dictionary, "\trot 2 roll \t-rot rot rot \t2dup over over \t2drop drop drop \t");
	
	// Set the background color to black and the text color to white
	#if VIC20
	asm("LDA #8");
	asm("STA 36879");
	asm("LDA #1");
	asm("STA 646");
	#elif C64
	asm("LDA #0");
	asm("STA 53280");
	asm("STA 53281");
	asm("LDA #1");
	asm("STA 646");
	#endif
	printf("%cpa'lante 0.2\n\n", 147);
	while(true) {
		// Parse user input
		accept();
		
		// Run it, one word at a time
		if (!compiling) status = STATUS_OK;
		while(rsp > 0) {
			// If the status is not OK or COMPILED, exit the loop
			if (status > STATUS_COMPILED) {
				dsp = 0;
				rsp = 0;
				break;
			}
			
			// Pop the return stack to the input pointer
			rsp--;
			ip = (char*)rs[rsp];
			
			// If we're compiling, keep compiling unless it's a semicolon
			if (compiling) {
				if (isName) clearCompiledWord();
				isName = false;
				if (StringStartsWith(ip, "; ")) {
					strcat(dictionary, "\t");
					compiling = false;
					status = STATUS_OK;
				}
				else {
					length = strlen(dictionary);
					while(ip[0] != ' ') {
						dictionary[length] = ip[0];
						length++;
						ip++;
					}
					dictionary[length] = ' ';
				}
				continue;
			}
			
			// This is not a standard Forth word, but a tutorial I followed once
			// (the one I learned this compiler strategy from) had it and I like
			// it, so it works for now (lol).  Maybe I'll replace it with "SEE"
			// later... or maybe I'll just leave it :)
			if (StringStartsWith(ip, "compiler ")) {
				printf("%s", dictionary);
				continue;
			}

			// Handle Forth words
			if (StringStartsWith(ip, ": ")) {
				compiling = isName = true;
				status = STATUS_COMPILED;
				continue;
			}
			if (StringStartsWith(ip, "= ")) {
				equals();
				continue;
			}
			if (StringStartsWith(ip, "<> ")) {
				notEqual();
				continue;
			}
			if (StringStartsWith(ip, "<= ")) {
				lte();
				continue;
			}
			if (StringStartsWith(ip, "< ")) {
				lt();
				continue;
			}
			if (StringStartsWith(ip, ">= ")) {
				gte();
				continue;
			}
			if (StringStartsWith(ip, "> ")) {
				gt();
				continue;
			}
			if (StringStartsWith(ip, "! ")) {
				store();
				continue;
			}
			if (StringStartsWith(ip, "@ ")) {
				fetch();
				continue;
			}
			if (StringStartsWith(ip, ". ")) {
				pop();
				continue;
			}
			if (StringStartsWith(ip, ".s ")) {
				printStack();
				continue;
			}
			if (StringStartsWith(ip, "+ ")) {
				add();
				continue;
			}
			if (StringStartsWith(ip, "- ")) {
				subtract();
				continue;
			}
			if (StringStartsWith(ip, "* ")) {
				multiply();
				continue;
			}
			if (StringStartsWith(ip, "/ ")) {
				divide();
				continue;
			}
			if (StringStartsWith(ip, "mod ")) {
				mod();
				continue;
			}
			if (StringStartsWith(ip, "accept ")) {
				accept();
				continue;
			}
			if (StringStartsWith(ip, "and ")) {
				and();
				continue;
			}
			if (StringStartsWith(ip, "bye ")) {
				bye();
			}
			if (StringStartsWith(ip, "c! ")) {
				cstore();
				continue;
			}
			if (StringStartsWith(ip, "c@ ")) {
				cfetch();
				continue;
			}
			if (StringStartsWith(ip, "depth ")) {
				depth();
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
			if (StringStartsWith(ip, "execute ")) {
				execute();
				continue;
			}
			if (StringStartsWith(ip, "invert ")) {
				invert();
				continue;
			}
			if (StringStartsWith(ip, "lshift ")) {
				lshift();
				continue;
			}
			if (StringStartsWith(ip, "negate ")) {
				negate();
				continue;
			}
			if (StringStartsWith(ip, "or ")) {
				or();
				continue;
			}
			if (StringStartsWith(ip, "over ")) {
				over();
				continue;
			}
			if (StringStartsWith(ip, "pick ")) {
				pick();
				continue;
			}
			if (StringStartsWith(ip, "roll ")) {
				roll();
				continue;
			}
			if (StringStartsWith(ip, "rshift ")) {
				rshift();
				continue;
			}
			if (StringStartsWith(ip, "swap ")) {
				swap();
				continue;
			}
			if (StringStartsWith(ip, "type ")) {
				type();
				continue;
			}
			if (StringStartsWith(ip, "xor ")) {
				xor();
				continue;
			}
			
			// If it's not a Forth word, is it a number?  If so, push it onto the stack.
			if (IsNumber(ip)) {
				ds[dsp] = atoi(ip);
				dsp++;
				continue;
			}
			
			// If it's not a number, is it in the dictionary?
			if (InDictionary()) continue;
			
			// If at any point there's an error, stop running
			if (status > STATUS_COMPILED) {
				dsp = 0;
				rsp = 0;
				break;
			}
		}
		
		// And print the system status
		switch(status) {
			case STATUS_COMPILED: printf("compiled"); break;
			case STATUS_STACK_UNDERFLOW: printf("stack underflow"); break;
			case STATUS_DIV_BY_ZERO: printf("division by zero"); break;
			default: printf("  ok");
		}
		printf("\n");
	}
}
