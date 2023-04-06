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
#define C64 true

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


// Status codes

#define STATUS_OK				0		// No error, "ok"
#define STATUS_COMPILED			1		// All words compiled successfully
#define STATUS_STACK_UNDERFLOW	2		// Stack underflow error
#define STATUS_DIV_BY_ZERO		3		// Division by 0 error



// -----------------------------------------------------------------------------
// GLOBAL VARIABLES
// -----------------------------------------------------------------------------

int16_t ds[STACK_SIZE], dsp,	// Data stack and its pointer
	rs[STACK_SIZE], rsp;		// Return stack and its pointer
uint16_t goWhere;				// Used to make EXECUTE work
uint8_t status = STATUS_OK;		// System status
char input[INPUT_SIZE];			// User input buffer
char* ip;						// Interpreter pointer


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
	
	// Set the background color to black and the text color to white
	#if VIC20
	asm("LDA #8");
	asm("STA 36879");
	asm("LDA #1");
	asm("STA 646");
	#elif C64
	asm("LDA #0");
	asm("STA 32580");
	asm("STA 32581");
	asm("LDA #1");
	asm("STA 646");
	#endif
	printf("%cpa'lante 0.1\n\n", 147);
	while(true) {
		// Get user input
		memset(input, 0, INPUT_SIZE);
		fgets(input, INPUT_SIZE, stdin);
		
		// Push all the words the user typed onto the return stack
		for (i=INPUT_SIZE - 1; i>-1; i--) {
			
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
		
		// Run it, one word at a time
		status = STATUS_OK;
		while(rsp > 0) {
			// Pop the return stack to the input pointer
			rsp--;
			ip = (char*)rs[rsp];

			// Handle Forth words
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
			if (StringStartsWith(ip, "lshift ")) {
				lshift();
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
			if (StringStartsWith(ip, "rshift ")) {
				rshift();
				continue;
			}
			if (StringStartsWith(ip, "swap ")) {
				swap();
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
