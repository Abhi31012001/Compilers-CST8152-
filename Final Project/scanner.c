/***************************************************
* Filename: scanner.h
* Purpose: Functions implementing a Lexical Analyzer (Scanner) as required for CST8152, Assignment #2, scanner_init() must be called before using the scanner.
* Version: 1.21
* Date: 24 Sep 2020
* Created by: Svillen Ranev - Updated by: Paulo Sousa
* Course: CST8152 - Compiler
* Author:
****************************************************
*/

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
 * to suppress the warnings about using "unsafe" functions like fopen()
 * and standard sting library functions defined in string.h.
 * The define does not have any effect in Borland compiler projects.
 */
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

 /*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
   It is defined in platy_st.c */
extern pBuffer stringLiteralTable;		/* String literal table */
int line;								/* current line number of the source code */
extern int errorNumber;					/* defined in platy_st.c - run-time error number */

static char debugMode = 0;				/* optional for debugging */

/* Local(file) global objects - variables */
static pBuffer lexemeBuffer;			/* pointer to temporary lexeme buffer */
static pBuffer sourceBuffer;			/* pointer to input source buffer */
/* No other global variable declarations/definitiond are allowed */

/* scanner.c static(local) function  prototypes */
static int nextTokenClass(char c);		/* character class function */
static int getNextState(int, char);		/* state machine function */
static int isKeyword(char* kw_lexeme);	/* keywords lookup functuion */

/* TODO_02: Follow the standard and adjust all function headers */

/*************************************************************
 * Intitializes scanner
 ************************************************************/
int initScanner(pBuffer psc_buf) {
	if (bufferIsEmpty(psc_buf)) return EXIT_FAILURE;/*1*/
	/* in case the buffer has been read previously  */
	bufferRewind(psc_buf);
	bufferClear(stringLiteralTable);
	line = 1;
	sourceBuffer = psc_buf;
	return EXIT_SUCCESS;/*0*/
/*   scerrnum = 0;  *//*no need - global ANSI C */
}


/*************************************************************
 * Process Token
 ************************************************************/
Token processToken(void) {
	Token currentToken = { 0 }; /* token to return after pattern recognition. Set all structure members to 0 */
	unsigned char c;	/* input symbol */
	int state = 0;		/* initial state of the FSM */
	short lexStart;		/* start offset of a lexeme in the input char buffer (array) */
	short lexEnd;		/* end offset of a lexeme in the input char buffer (array)*/

	int lexLength;		/* token length */
	int i;				/* counter */
	int isError;		/* error identification */
	unsigned char newc;	/* new char */

	while (1) { /* endless loop broken by token returns it will generate a warning */
		c = bufferGetChar(sourceBuffer);

		/* Part 1: Implementation of token driven scanner */
		/* every token is possessed by its own dedicated code */
		/* TODO_03: Token driven scanner implementation using switch */

		switch (c) {
			/*SEOF tokens - '\0' or one of 255,0xFF,EOF*/
		case CHARSEOF255:
			currentToken.code = SEOF_T;
			currentToken.attribute.seof = SEOF_EOF;
			return currentToken;
			break;
		case CHARSEOF0:
			currentToken.code = SEOF_T;
			currentToken.attribute.seof = SEOF_0;
			return currentToken;
			break;

			/* Some symbol tokens*/
		case '(':
			currentToken.code = LPR_T;
			return currentToken;
			break;
		case ')':
			currentToken.code = RPR_T;
			return currentToken;
			break;
		case '{':
			currentToken.code = LBR_T;
			return currentToken;
			break;
		case '}':
			currentToken.code = RBR_T;
			return currentToken;
			break;
		case ' ':
		case '\v':
		case '\t':
		case '\r':
		case '\f':
			continue;

			/* Count number of lines*/
		case '\n':
			line++;
			break;
			/* Arithmetic tokens*/
		case '+':
			currentToken.code = ART_OP_T;
			currentToken.attribute.arr_op = ADD;
			return currentToken;
			break;
		case '-':
			currentToken.code = ART_OP_T;
			currentToken.attribute.arr_op = SUB;
			return currentToken;
			break;
		case '*':
			currentToken.code = ART_OP_T;
			currentToken.attribute.arr_op = MUL;
			return currentToken;
			break;
		case '/':
			currentToken.code = ART_OP_T;
			currentToken.attribute.arr_op = DIV;
			return currentToken;
			break;
		case '=':
			c = bufferGetChar(sourceBuffer);
			if (c == '=') {
				currentToken.code = REL_OP_T;
				currentToken.attribute.rel_op = EQ;
				return currentToken;
			}
			else
			{
				bufferRetract(sourceBuffer);
				currentToken.code = ASS_OP_T;
				return currentToken;
			}
		case '$':
			c = bufferGetChar(sourceBuffer);
			if (c == '$') {
				currentToken.code = SCC_OP_T;
				return currentToken;
			}
			bufferRetract(sourceBuffer);
			currentToken.code = ERR_T;
			currentToken.attribute.err_lex[0] = '$';
			currentToken.attribute.err_lex[1] = '\0';
			return currentToken;
			break;
		case ';':
			currentToken.code = EOS_T;
			return currentToken;
			break;
		case ',':
			currentToken.code = COM_T;
			return currentToken;
			break;
		case '_':
			bufferSetMarkPosition(sourceBuffer, bufferGetCPosition(sourceBuffer));
			c = bufferGetChar(sourceBuffer);
			if (c == 'A' && bufferGetChar(sourceBuffer) == 'N' && bufferGetChar(sourceBuffer) == 'D' && bufferGetChar(sourceBuffer) == '_') {
				currentToken.code = LOG_OP_T;
				currentToken.attribute.log_op = AND;
				return currentToken;
			}
			else if (c == 'O' && bufferGetChar(sourceBuffer) == 'R' && bufferGetChar(sourceBuffer) == '_')
			{
				currentToken.code = LOG_OP_T;
				currentToken.attribute.log_op = OR;
				return currentToken;
			}
			bufferReset(sourceBuffer);
			currentToken.code = ERR_T;
			strcpy(currentToken.attribute.err_lex, "_");
			return currentToken;
			break;
		case '!':
			c = bufferGetChar(sourceBuffer);
			if (c == '!') {
				while (c != CHARSEOF255 && c != '\n') {
					c = bufferGetChar(sourceBuffer);
				}
				line++;
			}
			else
			{
				bufferRetract(sourceBuffer);
				bufferRetract(sourceBuffer);
				c = bufferGetChar(sourceBuffer);
				currentToken.code = ERR_T;
				currentToken.attribute.err_lex[0] = c;
				c = bufferGetChar(sourceBuffer);
				currentToken.attribute.err_lex[1] = c;
				currentToken.attribute.err_lex[2] = '\0';
				while (c != '\n') {
					c = bufferGetChar(sourceBuffer);
				}
				return currentToken;
			}
			break;
		case '>':
			currentToken.code = REL_OP_T;
			currentToken.attribute.rel_op = GT;
			return currentToken;
			break;
		case '<':
			c = bufferGetChar(sourceBuffer);
			if (c == '>') {
				currentToken.code = REL_OP_T;
				currentToken.attribute.rel_op = NE;
				return currentToken;
			}
			else
			{
				bufferRetract(sourceBuffer);
				currentToken.code = REL_OP_T;
				currentToken.attribute.rel_op = LT;
				return currentToken;
			}
			break;

		case '"':
			lexStart = bufferGetCPosition(sourceBuffer);
			bufferSetMarkPosition(sourceBuffer, lexStart);
			c = bufferGetChar(sourceBuffer);
			while (c != '"') {
				if (c == '\n' || c == CHARSEOF255) {
					++line;
				}
				if (c == CHARSEOF0)
				{
					i = 0;
					lexEnd = bufferGetCPosition(sourceBuffer);
					currentToken.code = ERR_T;
					bufferSetMarkPosition(sourceBuffer, lexStart - 1);
					bufferReset(sourceBuffer);
					while (lexStart <= lexEnd) {
						c = bufferGetChar(sourceBuffer);
						if (i < ERR_LEN)
							currentToken.attribute.err_lex[i] = c;
						++i;
						++lexStart;
					}
					if (i > (ERR_LEN - 3)) {
						currentToken.attribute.err_lex[ERR_LEN - 3] = '.';
						currentToken.attribute.err_lex[ERR_LEN - 2] = '.';
						currentToken.attribute.err_lex[ERR_LEN - 1] = '.';
						currentToken.attribute.err_lex[ERR_LEN] = CHARSEOF0;
					}
					bufferRetract(sourceBuffer);
					return currentToken;
				}
				c = bufferGetChar(sourceBuffer);
			}
			currentToken.code = STR_T;
			(currentToken.attribute).str_offset = bufferGetAddCPosition(stringLiteralTable);
			lexEnd = (bufferGetCPosition(sourceBuffer) - 1);
			bufferReset(sourceBuffer);
			while (lexStart != lexEnd) {
				c = bufferGetChar(sourceBuffer);
				bufferAddChar(stringLiteralTable, c);
				++lexStart;
			}
			bufferAddChar(stringLiteralTable, '\0');
			c = bufferGetChar(sourceBuffer);
			return currentToken;
			break;
			/* Part 2: Implementation of Finite State Machine (DFA)
					   or Transition Table driven Scanner
					   Note: Part 2 must follow Part 1 to catch the illegal symbols
			*/
			/* TODO_04: Transition driven scanner implementation inside default */

		default: // general case
			/* ... */
			if (isalnum(c)) {
				state = 0;
				lexStart = bufferGetCPosition(sourceBuffer) - 1;
				bufferSetMarkPosition(sourceBuffer, bufferGetCPosition(sourceBuffer) - 1);
				bufferRetract(sourceBuffer);

				while (stateType[state] == NOAS) {
					c = bufferGetChar(sourceBuffer);
					state = getNextState(state, c);
				}

				if (stateType[state] == ASWR) {
					bufferRetract(sourceBuffer);
				}
				lexEnd = bufferGetCPosition(sourceBuffer);
				lexLength = lexEnd - lexStart;
				lexemeBuffer = bufferCreate(lexLength + 1, 1, 'f');
				if (!lexemeBuffer) {
					++errorNumber;
					currentToken.code = ERR_T;
					strcpy(currentToken.attribute.err_lex, "RUN TIME ERROR");
					return currentToken;
				}
				bufferReset(sourceBuffer);
				while (lexStart != lexEnd) {
					c = bufferGetChar(sourceBuffer);
					if (!isspace(c))
						bufferAddChar(lexemeBuffer, c);
					++lexStart;
				}
				bufferAddChar(lexemeBuffer, CHARSEOF0);
				currentToken = finalStateTable[state](bufferGetString(lexemeBuffer, 0));
				bufferFree(lexemeBuffer);
				return currentToken;
			}
			else
			{
				currentToken.code = ERR_T;
				currentToken.attribute.err_lex[0] = c;
				currentToken.attribute.err_lex[1] = CHARSEOF0;
				return currentToken;
			}
			break;
		} // switch

	} //while

} //processToken


/* DO NOT MODIFY THE CODE OF THIS FUNCTION
YOU CAN REMOVE THE COMMENTS ONLY */
/*************************************************************
 * Get Next State
 ************************************************************/

int getNextState(int state, char c) {
	int col;
	int next;
	if (debugMode)
		fprintf(stderr, "[03] getNextState\n");
	col = nextTokenClass(c);
	next = transitionTable[state][col];
#ifdef DEBUG
	printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif
	/*
	The assert(int test) macro can be used to add run-time diagnostic to programs
	and to "defend" from producing unexpected results.
	assert() is a macro that expands to an if statement;
	if test evaluates to false (zero) , assert aborts the program
	(by calling abort()) and sends the following message on stderr:

	Assertion failed: test, file filename, line linenum

	The filename and linenum listed in the message are the source file name
	and line number where the assert macro appears.
	If you place the #define NDEBUG directive ("no debugging")
	in the source code before the #include <assert.h> directive,
	the effect is to comment out the assert statement.
	*/
	assert(next != IS);

	/*
	The other way to include diagnostics in a program is to use
	conditional preprocessing as shown bellow. It allows the programmer
	to send more details describing the run-time problem.
	Once the program is tested thoroughly #define DEBUG is commented out
	or #undef DEBUF is used - see the top of the file.
	*/
#ifdef DEBUG
	if (next == IS) {
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
		exit(1);
	}
#endif
	return next;
}

/*************************************************************
 * Get Next Token Class
 ************************************************************/
int nextTokenClass(char c) {
	int val = -1;
	if (debugMode)
		fprintf(stderr, "[04] NextTokenClass\n");

	/*
	THIS FUNCTION RETURNS THE COLUMN NUMBER IN THE TRANSITION
	TABLE st_table FOR THE INPUT CHARACTER c.
	SOME COLUMNS MAY REPRESENT A CHARACTER CLASS .
	FOR EXAMPLE IF COLUMN 2 REPRESENTS [A-Za-z]
	THE FUNCTION RETURNS 2 EVERY TIME c IS ONE
	OF THE LETTERS A,B,...,Z,a,b...z.
	PAY ATTENTION THAT THE FIRST COLOMN IN THE TT IS 0 (has index 0)
	*/

	/* TODO_05: the logic to return the next column in TT */
	/*				[A-z](0),	0(1),	[1-9](2),	.(3),	$(4),	"(5),	SEOF(6),	other(7) */
	switch (c) {
	case '0':
		val = 1;
		break;
	case '.':
		val = 3;
		break;
	case '$':
		val = 4;
		break;
	case '"':
		val = 5;
		break;
	case CHARSEOF255 || CHARSEOF0:
		val = 6;
		break;
	default:
		if (isalpha(c))
			val = 0;
		else if (isdigit(c) && c != '0')
			val = 2;
		else
			val = 7;
		break;
	} //switch
	return val;
}

/*************************************************************
 * Acceptance State Function AVID
 ************************************************************/
 /* TODO_06: Implement the method to recognize AVID */
 /*
HERE YOU WRITE THE DEFINITIONS FOR YOUR ACCEPTING FUNCTIONS.
************************************************************
ACCEPTING FUNCTION FOR THE arithmentic variable identifier AND keywords (VID - AVID/KW)
REPLACE XX WITH THE CORRESPONDING ACCEPTING STATE NUMBER
*/
Token aStateFuncAVID(char lexeme[]) {
#ifdef DEBUG
	printf("lexeme: |%s|\u", lexeme);
#endif
	Token currentToken = { 0 };
	int numKey = isKeyword(lexeme);

	if (numKey != -1) {
		currentToken.code = KW_T;
		currentToken.attribute.kwt_idx = numKey;
		return currentToken;
	}
	else {
		currentToken.code = AVID_T;
	}
	if (strlen(lexeme) > VID_LEN) {
		for (int j = 0; j < VID_LEN; j++) {
			currentToken.attribute.vid_lex[j] = lexeme[j];
		}
		currentToken.attribute.vid_lex[VID_LEN] = CHARSEOF0;
	}
	else
	{
		strcpy(currentToken.attribute.vid_lex, lexeme);
	}
	return currentToken;
}

/*************************************************************
 * Acceptance State Function SVID
 ************************************************************/
 /* TODO_07: Implement the method to recognize SVID */
 /*
 ACCEPTING FUNCTION FOR THE string variable identifier (VID - SVID)
 REPLACE XX WITH THE CORRESPONDING ACCEPTING STATE NUMBER
 */
Token aStateFuncSVID(char lexeme[]) {
#ifdef DEBUG
	printf("lexeme: |%s|\u", lexeme);
#endif
	Token currentToken = { 0 };
	currentToken.code = SVID_T;
	if (strlen(lexeme) > VID_LEN) {
		strncpy(currentToken.attribute.vid_lex, lexeme, VID_LEN - 1);
		for (int j = 0; j < VID_LEN; j++) {
			currentToken.attribute.vid_lex[j] = lexeme[j];
		}
		currentToken.attribute.vid_lex[VID_LEN - 1] = '$';
		currentToken.attribute.vid_lex[VID_LEN] = CHARSEOF0;
	}
	else {
		strcpy(currentToken.attribute.vid_lex, lexeme);
	}
	return currentToken;
}

/*************************************************************
 * Acceptance State Function IL
 ************************************************************/
 /* TODO_08: Implement the method to recognize IL */
 /*
 ACCEPTING FUNCTION FOR THE integer literal(IL) - decimal constant (DIL)
 */
Token aStateFuncIL(char lexeme[]) {
#ifdef  DEBUG
	printf("lexeme: |%s| \n", lexeme);
#endif
	Token currentToken = { 0 };
	long decValue = atol(lexeme);

	if (strlen(lexeme) > ERR_LEN || decValue < SHRT_MIN || decValue > SHRT_MAX) {
		strncpy(currentToken.attribute.err_lex, lexeme, ERR_LEN - 3);
		currentToken.attribute.err_lex[ERR_LEN - 3] = '.';
		currentToken.attribute.err_lex[ERR_LEN - 2] = '.';
		currentToken.attribute.err_lex[ERR_LEN - 1] = '.';

		currentToken.attribute.err_lex[ERR_LEN] = CHARSEOF0;
	}
	else {
		currentToken.code = INL_T;
		currentToken.attribute.int_value = decValue;
	}
	return currentToken;
}

/*************************************************************
 * Acceptance State Function FPL
 ************************************************************/
 /* TODO_09: Implement the method to recognize FPL */
 /*
 ACCEPTING FUNCTION FOR THE floating-point literal (FPL)
 */
Token aStateFuncFPL(char lexeme[]) {
#ifdef DEBUG
	printf("lexeme: |%s| \n", lexeme);
#endif
	Token currentToken = { 0 };
	double floatValue = atof(lexeme);
	if (floatValue > FLT_MAX || (floatValue < FLT_MIN && floatValue != 0.0)) {
		currentToken.code = ERR_T;

		if (strlen(lexeme) > ERR_LEN) {
			strncpy(currentToken.attribute.err_lex, lexeme, ERR_LEN - 3);
			currentToken.attribute.err_lex[ERR_LEN - 3] = '.';
			currentToken.attribute.err_lex[ERR_LEN - 2] = '.';
			currentToken.attribute.err_lex[ERR_LEN - 1] = '.';

			currentToken.attribute.err_lex[ERR_LEN] = CHARSEOF0;
		}
		else
			strcpy(currentToken.attribute.err_lex, lexeme);
		return currentToken;
	}

	currentToken.code = FPL_T;
	currentToken.attribute.flt_value = (float)floatValue;
	return currentToken;
}

/*************************************************************
 * Acceptance State Function SL
 ************************************************************/
 /* TODO_10: Implement the method to recognize SL */
 /*
 ACCEPTING FUNCTION FOR THE string literal (SL)
 */
Token aStateFuncSL(char lexeme[]) {
#ifdef DEBUG
	printf("lexeme: |%s|\u", lexeme);
#endif
	Token currentToken = { 0 };
	/* ... */
	int length = (int)strlen(lexeme);

	currentToken.attribute.str_offset = bufferGetAddCPosition(stringLiteralTable);

	for (int i = 0; i < length; i++) {
		if (lexeme[i] != '"') {
			bufferAddChar(stringLiteralTable, lexeme[i]);
		}

		if (lexeme[i] == '\n') {
			line++;
		}
	}

	/*add \0 at the end ake the string c_ type string*/
	bufferAddChar(stringLiteralTable, CHARSEOF0);
	/*set tring token code*/
	currentToken.code = STR_T;
	return currentToken;
}

/*************************************************************
 * Acceptance State Function Error
 ************************************************************/
 /* TODO_11: Implement the method to deal with Error Token */
 /*
 ACCEPTING FUNCTION FOR THE ERROR TOKEN
 */
Token aStateFuncErr(char lexeme[]) {
#ifdef DEBUG
	printf("lexeme: |%s|\u", lexeme);
#endif // DEBUG
	Token currentToken = { 0 };
	/* ... */
	int length = (int)strlen(lexeme);/*lexeme string length*/

	currentToken.code = ERR_T; /*Set error code*/
	/*If error lexeme is longer than ERR_LEN:*/
	if (length > ERR_LEN) {
		/*only ERR_LEN - 3 is stored in err_lex*/
		strncpy(currentToken.attribute.err_lex, lexeme, ERR_LEN - 3);
		/*then 3 dots are added to the end of err_lex*/
		currentToken.attribute.err_lex[ERR_LEN - 3] = '.';
		currentToken.attribute.err_lex[ERR_LEN - 2] = '.';
		currentToken.attribute.err_lex[ERR_LEN - 1] = '.';

		/*Add null char at the end to make c type string*/
		currentToken.attribute.err_lex[ERR_LEN] = CHARSEOF0;
	}/*If error lexeme is not longer than ERR_LEN*/
	else {
		/*store whole lexeme*/
		strncpy(currentToken.attribute.err_lex, lexeme, length);
		/*Add null char the end to make c type string*/
		currentToken.attribute.err_lex[length] = CHARSEOF0;
	}

	/*if the ERROR lexeme contains line terminators, line counter must be incremented*/
	for (int i = 0; i < length; i++) {
		if (lexeme[i] == '\n') {
			line++;
		}
	}

	/*Return appropriate token code*/
	return currentToken;
}

/*************************************************************
 * Check if it is Keyword
 ************************************************************/
 /* TODO_11: Checks if a string is a keyword, returning the position in the list */
int isKeyword(char* kw_lexeme) {
	/* ... */
	int i;

	for (i = 0; i < KWT_SIZE; i++) {
		if (strcmp(kw_lexeme, keywordTable[i]) == 0)
			return i;
	}

	return -1;
}

/*
HERE YOU WRITE YOUR ADDITIONAL FUNCTIONS (IF ANY).
FOR EXAMPLE
*/
Token aStateFuncErrWithRetract(char lexeme[]) {
	Token currentToken = { 0 }; /*token to return*/
	currentToken = finalStateTable[ES](lexeme);
	return currentToken;
}