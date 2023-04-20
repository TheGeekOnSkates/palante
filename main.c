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
#include <cbm.h>



// -----------------------------------------------------------------------------
// MACROS
// -----------------------------------------------------------------------------

// Execution tokens (XTs) are a kind of intermediate bytecode.  By "compiling"
// Forth source to this bytecode, we save on a bunch of runtime string-parsing

#define XT_NUMBER              1
#define XT_AND                 2
#define XT_AGAIN               3
#define XT_BEGIN               4
#define XT_BYE                 5
#define XT_CFETCH              6
#define XT_CSTORE              7
#define XT_DEPTH               8
#define XT_DIVIDE              9
#define XT_DOT                 10
#define XT_DOT_S               11
#define XT_DROP                12
#define XT_DUP                 13
#define XT_EMIT                14
#define XT_EQUAL               15
#define XT_EXECUTE             16
#define XT_FETCH               17
#define XT_GREATER_THAN        18
#define XT_LESS_THAN           19
#define XT_LSHIFT              20
#define XT_OR                  21
#define XT_OVER                22
#define XT_PICK                23
#define XT_PLUS                24
#define XT_ROLL                25
#define XT_RSHIFT              26
#define XT_STORE               27
#define XT_SWAP                28
#define XT_TIMES               29
#define XT_UNTIL               30
#define XT_XOR                 31


// Build targets

#define VIC20 false
#define C64 true





// -----------------------------------------------------------------------------
// TYPES
// -----------------------------------------------------------------------------

typedef uint16_t xt;


// -----------------------------------------------------------------------------
// GLOBAL VARIABLES
// -----------------------------------------------------------------------------

const char* DELIMITERS = " \xA0\n";		// Space, Shift-Space (160) and \n
char buffer[80];						// For storing users' code
xt input[40],							// input (above), compiled to XTs
	ip = 0,								// Interpreter pointer
	ds[128], dsp = 0,					// Data stack ("the stack") and pointer
	rs[128], rsp = 0,					// Return stack and its pointer
	dictionary[4096], dp = 0;			// Dictionary and its pointer
uint8_t temp8 = 0;						// For one-off jobs requiring 8-bit and
int16_t temp16 = 0;						// 16-bit numbers (obviously) :)


// -----------------------------------------------------------------------------
// TO BE SORTED
// -----------------------------------------------------------------------------

bool IsNumber(char* word) {
	static size_t i, length;
	i = 0;
	if (word == NULL) return false;
	length = strlen(word);
	if (word[i] == '-') {
		if (length == 1) return false;
		i++;
	}
	for (; i<length; i++)
		if (word[i] < '0' || word[i] > '9')
			return false;
	return true;
}

void main() {
	static char* word;
	static uint8_t error, i;
	
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
	printf("%cPa'lante 0.3\n\n", 147);
	
	// This is the "main loop" (like in games)
	while(true) {
		
		// Read and compile the user's input
		fgets(buffer, 80, stdin);
		word = strtok(buffer, DELIMITERS);
		memset(input, 0, 40);
		ip = 0;
		while(word != NULL) {
			if (strcmp(word, "#") == 0) break;
			if (strcmp(word, "(") == 0) {
				word = strtok(NULL, DELIMITERS);
				while(word != NULL) {
					if (word[strlen(word)-1] == ')') break;
					word = strtok(NULL, DELIMITERS);
				}
				word = strtok(NULL, DELIMITERS);
				if (word == NULL) break;
			}
			if (strcmp(word, "and") == 0) {
				input[ip] = XT_AND;
				ip++;
			}
			else if (strcmp(word, "again") == 0) {
				input[ip] = XT_AGAIN;
				ip++;
			}
			else if (strcmp(word, "begin") == 0) {
				input[ip] = XT_BEGIN;
				ip++;
			}
			else if (strcmp(word, "bye") == 0) {
				input[ip] = XT_BYE;
				ip++;
			}
			else if (strcmp(word, "c@") == 0) {
				input[ip] = XT_CFETCH;
				ip++;
			}
			else if (strcmp(word, "c!") == 0) {
				input[ip] = XT_CSTORE;
				ip++;
			}
			else if (strcmp(word, "depth") == 0) {
				input[ip] = XT_DEPTH;
				ip++;
			}
			else if (strcmp(word, "/") == 0) {
				input[ip] = XT_DIVIDE;
				ip++;
			}
			else if (strcmp(word, ".") == 0) {
				input[ip] = XT_DOT;
				ip++;
			}
			else if (strcmp(word, ".s") == 0) {
				input[ip] = XT_DOT_S;
				ip++;
			}
			else if (strcmp(word, "drop") == 0) {
				input[ip] = XT_DROP;
				ip++;
			}
			else if (strcmp(word, "dup") == 0) {
				input[ip] = XT_DUP;
				ip++;
			}
			else if (strcmp(word, "emit") == 0) {
				input[ip] = XT_EMIT;
				ip++;
			}
			else if (strcmp(word, "=") == 0) {
				input[ip] = XT_EQUAL;
				ip++;
			}
			else if (strcmp(word, "execute") == 0) {
				input[ip] = XT_EXECUTE;
				ip++;
			}
			else if (strcmp(word, "@") == 0) {
				input[ip] = XT_FETCH;
				ip++;
			}
			else if (strcmp(word, "<") == 0) {
				input[ip] = XT_LESS_THAN;
				ip++;
			}
			else if (strcmp(word, ">") == 0) {
				input[ip] = XT_GREATER_THAN;
				ip++;
			}
			else if (strcmp(word, "lshift") == 0) {
				input[ip] = XT_LSHIFT;
				ip++;
			}
			else if (strcmp(word, "or") == 0) {
				input[ip] = XT_OR;
				ip++;
			}
			else if (strcmp(word, "over") == 0) {
				input[ip] = XT_OVER;
				ip++;
			}
			else if (strcmp(word, "pick") == 0) {
				input[ip] = XT_PICK;
				ip++;
			}
			else if (strcmp(word, "+") == 0) {
				input[ip] = XT_PLUS;
				ip++;
			}
			else if (strcmp(word, "roll") == 0) {
				input[ip] = XT_ROLL;
				ip++;
			}
			else if (strcmp(word, "rshift") == 0) {
				input[ip] = XT_RSHIFT;
				ip++;
			}
			else if (strcmp(word, "!") == 0) {
				input[ip] = XT_STORE;
				ip++;
			}
			else if (strcmp(word, "swap") == 0) {
				input[ip] = XT_SWAP;
				ip++;
			}
			else if (strcmp(word, "*") == 0) {
				input[ip] = XT_TIMES;
				ip++;
			}
			else if (strcmp(word, "until") == 0) {
				input[ip] = XT_UNTIL;
				ip++;
			}
			else if (strcmp(word, "xor") == 0) {
				input[ip] = XT_XOR;
				ip++;
			}
			else if (IsNumber(word)) {
				input[ip] = XT_NUMBER;
				ip++;
				input[ip] = atol(word);
				ip++;
			}
			else {
				printf("Unknown word \"%s\"\n", word);
				input[0] = 65535L;
				break;
			}
			word = strtok(NULL, DELIMITERS);
		}
		
		// If there was a compile-time error, get input again
		if (input[0] == 65535L) continue;
		
		// Now run the code
		ip = 0; error = 0;
		while(!error) {
			switch(input[ip]) {
				case XT_AND:
					if (!dsp)
						error = 2;
					else {
						ds[dsp - 2] = ds[dsp - 2] & ds[dsp - 1];
						dsp--;
					}
					break;
				case XT_AGAIN:
					if (!rsp)
						error = 3;
					else {
						ip = rs[rsp] - 1;
						rsp--;
					}
					break;
				case XT_BEGIN:
					rs[rsp] = ip;
					rsp++;
					break;
				case XT_BYE:
					return;
				case XT_CFETCH:
					ds[dsp - 1] = PEEK(ds[dsp - 1]);
					break;
				case XT_CSTORE:
					POKE(ds[dsp - 1], ds[dsp - 2]);
					dsp -= 2;
					break;
				case XT_DEPTH:
					ds[dsp] = dsp;
					dsp++;
					break;
				case XT_DIVIDE:
					if (dsp < 2)
						error = 2;
					else {
						dsp--;
						if (!ds[dsp]) error = 4;
						else ds[dsp - 1] /= ds[dsp];
					}
					break;
				case XT_DOT:
					if (!dsp)
						error = 2;
					else {
						dsp--;
						printf("%d ", ds[dsp]);
					}
					break;
				case XT_DOT_S:
					printf("<%d> ", dsp);
					for (i=0; i<dsp; i++) printf("%d ", ds[i]);
					break;
				case XT_DROP:
					if (!dsp)
						error = 2;
					else dsp--;
					break;
				case XT_DUP:
					if (!dsp)
						error = 2;
					else {
						ds[dsp] = ds[dsp - 1];
						dsp++;
					}
					break;
				case XT_EMIT:
					if (!dsp)
						error = 2;
					else {
						dsp--;
						printf("%c", ds[dsp]);
					}
					break;
				case XT_EQUAL:
					if (dsp < 2)
						error = 2;
					else {
						ds[dsp - 2] = ds[dsp - 2] == ds[dsp - 1] ? -1 : 0;
						dsp--;
					}
					break;
				case XT_EXECUTE:
					if (dsp < 2)
						error = 2;
					else {
						dsp--;
						temp16 = ds[dsp - 1];
						asm("JSR _temp16");
					}
					break;
				case XT_GREATER_THAN:
					if (!dsp)
						error = 2;
					else {
						ds[dsp - 2] = ds[dsp - 2] > ds[dsp - 1] ? -1 : 0;
						dsp--;
					}
					break;
				case XT_LESS_THAN:
					if (!dsp)
						error = 2;
					else {
						ds[dsp - 2] = ds[dsp - 2] < ds[dsp - 1] ? -1 : 0;
						dsp--;
					}
					break;
				case XT_LSHIFT:
					if (!dsp)
						error = 2;
					else {
						ds[dsp - 2] = ds[dsp - 2] << ds[dsp - 1];
						dsp--;
					}
					break;
				case XT_NUMBER:
					ip++;
					ds[dsp] = input[ip];
					dsp++;
					break;
				case XT_FETCH:
					if (dsp)
						ds[dsp - 1] = PEEKW(ds[dsp - 1]);
					else error = 2;
					break;
				case XT_OR:
					if (!dsp)
						error = 2;
					else {
						ds[dsp - 2] = ds[dsp - 2] | ds[dsp - 1];
						dsp--;
					}
					break;
				case XT_OVER:
					if (dsp < 2)
						error = 2;
					else {
						ds[dsp] = ds[dsp - 2];
						dsp++;
					}
					break;
				case XT_PICK:
					if (!dsp)
						error = 2;
					else {
						temp8 = ds[dsp - 1];
						ds[dsp - 1] = ds[dsp - temp8 - 2];
					}
					break;
				case XT_PLUS:
					if (dsp < 2)
						error = 2;
					else {
						dsp--;
						ds[dsp - 1] += ds[dsp];
					}
					break;
				case XT_ROLL:
					if (!dsp)
						error = 2;
					else {
						temp8 = ds[dsp - 1];
						ds[dsp - 1] = ds[dsp - temp8 - 2];
						ds[dsp - 1] = ds[dsp - temp8 - 2];
						for (temp16 = temp8 + 1; temp16>= 0; temp16--)
							ds[dsp - temp16 - 1] = ds[dsp - temp16];
						dsp--;
					}
					break;
				case XT_RSHIFT:
					if (!dsp)
						error = 2;
					else {
						ds[dsp - 2] = ds[dsp - 2] >> ds[dsp - 1];
						dsp--;
					}
					break;
				case XT_STORE:
					if (dsp < 2)
						error = 2;
					else {
						POKEW(ds[dsp - 1], ds[dsp - 2]);
						dsp -= 2;
					}
					break;
				case XT_SWAP:
					if (dsp < 2)
						error = 2;
					else {
						temp8 = ds[dsp - 2];
						ds[dsp - 2] = ds[dsp - 1];
						ds[dsp - 1] = temp8;
					}
					break;
				case XT_TIMES:
					if (dsp < 2)
						error = 2;
					else {
						dsp--;
						ds[dsp - 1] *= ds[dsp];
					}
					break;
				case XT_UNTIL:
					if (!dsp)
						error = 2;
					else if (!rsp)
						error = 3;
					else {
						dsp--;
						rsp--;
						if (!ds[dsp]) {
							ip = rs[rsp] - 1;
						}
					}
					break;
				case XT_XOR:
					if (!dsp)
						error = 2;
					else {
						ds[dsp - 2] = ds[dsp - 2] ^ ds[dsp - 1];
						dsp--;
					}
					break;
				default:
					error = 1;
					break;
			}
			ip++;
		}
		switch(error) {
			case 1: printf("  ok"); break;
			case 3: printf("return ");	// Notice there's no "break"
			case 2: printf("stack underflow"); break;
		}
		printf("\n");
	}
}
