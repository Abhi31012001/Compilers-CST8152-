/*
* IMPORTANT NOTE (2DO):
* RENAME this file as "buffer.h"
* REPLACE the file header with your file header (see CST8152_ASSAMG.pdf for details).
* File Name: buffer.h
* Version: 2.01
* Author: Paulo Sousa / Svillen Ranev
* Date: 1 Sep 2020
* Preprocessor directives, type declarations and prototypes necessary for buffer implementation
* as required for CST8152-Assignment #1.
*/

/*
* IMPORTANT NOTE (2DO):
* The file is not completed.
* You must add your function declarations (prototypes).
* You must also add your constant definitions and macros,if any.
*/

#ifndef BUFFER_H_
#define BUFFER_H_

/*#pragma warning(1:4001) *//*to enforce C89 type comments  - to make //comments an warning */

/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */

/* standard header files */
#include <stdio.h>  /* standard input/output */
#include <malloc.h> /* for dynamic memory allocation*/
#include <limits.h> /* implementation-defined data type ranges and limits */

/* constant definitions */
#define RT_FAIL_1 (-1)         /* operation failure return value 1 */
#define RT_FAIL_2 (-2)         /* operation failure return value 2 */
#define LOAD_FAIL (-2)         /* load fail return value */

#define DEFAULT_INIT_CAPACITY 200   /* default initial buffer capacity */
#define DEFAULT_INC_FACTOR 15       /* default increment factor */
#define MAXIMUM_ALLOWED_VALUE SHRT_MAX-1


/* You should add your own constant definitions here */
#define FIXED_SIZE_MODE		0
#define ADD_INC_MODE		1
#define MULTI_INC_MODE		-1

#define MIN_ADD_INC			1
#define MAX_ADD_INC			255
#define FIXED_SIZE			0
#define MIN_MULTI_INC		1
#define MAX_MULTI_INC		100
/* Add your bit-masks constant definitions here */
#define DEFAULT_FLAGS	0xFFFC	// 1111 1111 1111 1100
#define SET_EOB			0x0001	// 0000 0000 0000 0001
#define RESET_EOB		0xFFFE	// 1111 1111 1111 1110
#define CHECK_EOB		0x0001	// 0000 0000 0000 0001
#define SET_R_FLAG		0x0002	// 0000 0000 0000 0010
#define RESET_R_FLAG	0xFFFD	// 1111 1111 1111 1101
#define CHECK_R_FLAG	0x0002	// 0000 0000 0000 0010

/* user data type declarations */
/* user data type declarations */
typedef struct BufferEntity {
	char* string;   /* pointer to the beginning of character array (character buffer) */
	short capacity;    /* current dynamic memory size (in bytes) allocated to character buffer */
	short addCPosition;  /* the offset (in chars) to the add-character location */
	short getCPosition;  /* the offset (in chars) to the get-character location */
	short markCPosition; /* the offset (in chars) to the mark location */
	char  increment; /* character array increment factor */
	char  opMode;       /* operational mode indicator*/
	unsigned short flags;     /* contains character array reallocation flag and end-of-buffer flag */
} Buffer, * pBuffer;

/*
IMPORTANT NOTE (2DO):
Place your function declarations here.
Do not include the function header comments here.
Place them in the buffer.c file
*/
pBuffer bufferCreate(short, char, char);
pBuffer bufferAddChar(pBuffer const, char);
int bufferLoad(FILE* const, Buffer* const);
char bufferGetChar(Buffer* const);
short bufferGetCapacity(Buffer* const);
int bufferGetOpMode(Buffer* const);
short bufferGetAddCPosition(Buffer* const);
size_t bufferGetIncrement(Buffer* const);
unsigned short bufferGetFlags(pBuffer const);
char* bufferGetString(Buffer* const, short);
int bufferPrint(Buffer* const, char);
int bufferRewind(Buffer* const);
void bufferFree(Buffer* const);
int bufferClear(Buffer* const);
int bufferIsFull(Buffer* const);
int bufferIsEmpty(Buffer* const);
Buffer* bufferSetEnd(Buffer* const, char);
short bufferRetract(Buffer* const);
short bufferGetCPosition(Buffer* const);
short bufferSetMarkPosition(pBuffer const, short);
short bufferReset(Buffer* const);

#endif
