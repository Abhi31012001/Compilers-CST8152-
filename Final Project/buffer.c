/*
* File Name: buffer.c
* Version: 1.1
* Compiler: MS Visual Studio 2019
* Author: Abhi , s/n: 040978822
* Course: CST8152 - Compiler, Lab Section: 013
* Date: October 4th 2020
* Professor: Paulo Sousa, Haider Miraj
* Purpose: Programming and Using Dynamic Structures (buffers) with C
*
* Function list: bufferCreate, bufferAddChar, bufferClear, bufferFree, bufferGetAddCPosition, bufferGetCPosition
*                bufferGetCapacity, bufferGetChar, buffergetFlags, bufferGetIncrement, bufferGetOpMode, bufferGetString,
*				 bufferIsEmpty, bufferIsFull, bufferLoad, bufferPrint, bufferRetract, bufferRewind, bufferSetEnd, bufferSetMarkPosition
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "buffer.h"

/*
* Purpose: This function creates a new buffer in memory (on the program heap)
* Author: Hoan Linh Cao
* History/Versions: 1.1 September 17th, 2020
* Called functions: calloc(), malloc(), free()

* Parameters:	- short initCapacity: the current total size (measured in bytes) of the memory allocated for the buffer
				- char incFactor: a buffer increment factor. It is used in the calculations of a new buffer capacity when the buffer needs to grow
				- char opMode:  an operational mode indicator of buffer

* Algorithm:	1. Allocate memory for a new buffer
				2. Set the suitable capacity and mode for each increment factor
				3. Allocate memory for one character buffer
				4. Setup flags to default

* Return value: - ptr_Buffer: pointer to new buffer created
				- NULL: if there's any error happened
*/
pBuffer bufferCreate(short initCapacity, char incFactor, char opMode) {
	pBuffer ptr_Buffer = calloc(1, sizeof(Buffer));

	/* Check the validation of buffer*/
	if (!ptr_Buffer)
		return NULL;

	/* Validate the range of the argument initCapacity*/
	if (initCapacity < 0 || initCapacity > MAXIMUM_ALLOWED_VALUE)
		return NULL;

	/*Check if initCapacity equals to 0 (special case)*/
	if (initCapacity == 0) {
		initCapacity = DEFAULT_INIT_CAPACITY;
		if (opMode == 'a' || opMode == 'm') {
			incFactor = DEFAULT_INC_FACTOR;
		}
		else if (opMode == 'f') {
			incFactor = 0;
		}
	}

	char mode = FIXED_SIZE_MODE;
	unsigned char increment = (unsigned char)incFactor;
	/* Set operation mode and indicator mode for buffer*/
	switch (opMode)
	{
	case 'a': // Addicative mode
		if (MIN_ADD_INC < incFactor < MAX_ADD_INC)
			mode = ADD_INC_MODE;
		break;
	case 'm': // Multiplicative mode
		if (MIN_MULTI_INC < incFactor < MAX_MULTI_INC)
			mode = MULTI_INC_MODE;
		break;
	case 'f': // Fixed size mode
		increment = FIXED_SIZE;
		break;
	default: // return null if the opMode is not any of them
		return NULL;
		break;
	}

	/* Allocate memory for character array buffer*/
	ptr_Buffer->string = (char*)malloc(sizeof(char) * initCapacity);
	/* If allocate fails, free memory and return null*/
	if (!ptr_Buffer->string) {
		free(ptr_Buffer);
		return NULL;
	}

	ptr_Buffer->capacity = initCapacity;		// Initialize buffer with initCapacity
	ptr_Buffer->opMode = mode;					// Set buffer operation mode
	ptr_Buffer->increment = (char)increment;	// Set buffer increment factor
	ptr_Buffer->flags = DEFAULT_FLAGS;			// Set buffer flag

	/* Successful Create a buffer*/
	return ptr_Buffer;
}


/*
* Purpose: This function adds character symbol to character array of the given pointed buffer.
			It checks the buffer is full or not and resizable depends on operation mode. If the
			resize occured, set r_flag bit to 1 indicates that the location of the buffer character
			array in memory has been changed due to memory reallocation
* Author: Hoan Linh Cao
* History/Versions: 1.1 September 17th, 2020
* Called functions: calloc(), malloc(), free()

* Parameters:	- pBuffer const pBE: pointer to buffer
				- char symbol: the character need to be added

* Algorithm:	1. Check the buffer validation
				2. Checks the buffer is full or not and resize depends on operation mode
				3. Add the character

* Return value:	- pBE: pointer to new buffer created
				- NULL: if there's any error happened
*/
pBuffer bufferAddChar(pBuffer const pBE, char symbol) {
	short availableSpace, newCapacity = 0;
	/* Check the validation of buffer*/
	if (!pBE)
		return NULL;

	/* Resets the flags field r_flag bit to 0*/
	pBE->flags &= RESET_R_FLAG;

	/*Check If the character buffer is already full*/
	if (pBE->addCPosition == pBE->capacity) {
		switch (pBE->opMode)
		{
		case FIXED_SIZE_MODE:	// Fixed size mode
			return NULL;

		case ADD_INC_MODE:		// Addicative mode
			/*A new capacity is created by adding increment(converted to bytes) to capacity*/
			newCapacity = pBE->capacity + (unsigned char)pBE->increment * sizeof(char);
			if (0 < newCapacity < MAXIMUM_ALLOWED_VALUE)	// Check if the new capacity is in the valid range.
				break;
			else if (newCapacity > MAXIMUM_ALLOWED_VALUE)	// If new capacity exceeds maximum positive allowed value, it is assigned to the maximum value
				newCapacity = MAXIMUM_ALLOWED_VALUE;
			else											// Any else condition will be aborted and return null.
				return NULL;
			break;

		case MULTI_INC_MODE:	// Multicative mode
			if (pBE->capacity == MAXIMUM_ALLOWED_VALUE)		// If the capacity of buffer already reach the maximum, abort and return null.
				return NULL;
			/* Calculate new capacity*/
			availableSpace = MAXIMUM_ALLOWED_VALUE - pBE->capacity;
			float newIncrement = (availableSpace * (unsigned char)pBE->increment) / 100;
			newCapacity = pBE->capacity + newIncrement;

			if (newCapacity == pBE->capacity)				// If new capacity exceeds maximum positive allowed value, it is assigned to the maximum value
				newCapacity = MAXIMUM_ALLOWED_VALUE;
			break;

		default:
			return NULL;
			break;
		}

		/* Reallocate memory for charater buffer with new capacity*/
		char* buffer = realloc(pBE->string, newCapacity);
		/* If reallocate fails, return null*/
		if (!buffer)
			return NULL;

		pBE->capacity = newCapacity;	// Assign new capacity to buffer
		pBE->string = buffer;
		pBE->flags |= SET_R_FLAG;		// Set r_flag field of buffer

	}

	// Store the symbol into the character buffer and increment addCPosition by 1
	pBE->string[pBE->addCPosition] = symbol;
	pBE->addCPosition++;

	return pBE;
}

/*
* Purpose: The function loads (reads) an open input file specified by fi into a buffer specified by pBE
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: fgetc(), ungetc(), feof(), bufferAddChar()

* Parameters:	- Buffer* const pBE
				- FILE* const fi

* Algorithm:	1. Check the buffer and file validation
				2. Loads input file to buffer
				3. Add the character

* Return value:	- fCount: number of characters in file load to buffer
				- RT_FAIL_1
				- LOAD_FAIL
*/
int bufferLoad(FILE* const fi, Buffer* const pBE) {
	/* Check the validation of buffer and file (arguments)*/
	if (!pBE || !fi)
		return RT_FAIL_1;

	char symbol;
	int fCount = 0;

	/* Loops through the file until it reachs the end of file*/
	while (!feof(fi)) {
		symbol = (char)fgetc(fi);	// Read one charater
		if (feof(fi)) break;		// If eof reached, end the loop
		if (!bufferAddChar(pBE, symbol)) {	// If the buffer is unable to add more character
			ungetc(symbol, fi);
			return LOAD_FAIL;
		}
		fCount++;
	}
	return fCount;

}

/*
* Purpose: The function is used to read and get the character from buffer
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: None

* Parameters:	- Buffer* const pBE

* Algorithm:	1. Check the buffer validation
				2. Check if getCPosition equals to addCPosition (reach the end of buffer) and set flag fields eob bit to 1 or else 0

* Return value:	- Return character at getCPosition
				- RT_FAIL_2
				- 0
*/
char bufferGetChar(Buffer* const pBE) {
	/* Check the validation of buffer*/
	if (!pBE) return RT_FAIL_2;

	if (pBE->getCPosition == pBE->addCPosition) { // If getCPosition equals to addCPosition
		pBE->flags |= SET_EOB;		// Set flag fields eob bit to 1
		return 0;					// return 0 
	}
	pBE->flags &= RESET_EOB;		// Set flag fields eob bit to 0

	/* Return character at getCPosition and increase getCPosition by 1*/
	return pBE->string[pBE->getCPosition++];
}


/*
* Purpose: The function returns the current capacity of the character buffer
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: None

* Parameters:	- Buffer* const pBE

* Algorithm:	1. Check the buffer validation

* Return value:	- Returns the current capacity of the character buffer
				- RT_FAIL_1
*/
short bufferGetCapacity(Buffer* const pBE) {
	/* Check the validation of buffer*/
	if (!pBE) return RT_FAIL_1;
	return pBE->capacity;
}

/*
* Purpose: The function returns the value of opMode
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: None

* Parameters:	- Buffer* const pBE

* Algorithm:	1. Check the buffer validation

* Return value:	- Returns the opMode of buffer
				- RT_FAIL_1
*/
int bufferGetOpMode(Buffer* const pBE) {
	/* Check the validation of buffer*/
	if (!pBE) return RT_FAIL_1;
	return pBE->opMode;
}

/*
* Purpose: The function returns the current addCPosition
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: None

* Parameters:	- Buffer* const pBE

* Algorithm:	1. Check the buffer validation

* Return value:	- Returns the current addCPosition of buffer
				- RT_FAIL_1
*/
short bufferGetAddCPosition(Buffer* const pBE) {
	/* Check the validation of buffer*/
	if (!pBE) return RT_FAIL_1;
	return pBE->addCPosition;
}

/*
* Purpose: The function returns the non-negative value of increment
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: None

* Parameters:	- Buffer* const pBE

* Algorithm:	1. Check the buffer validation

* Return value:	- Returns the non-negative value of increment
				- 0x100
*/
size_t bufferGetIncrement(Buffer* const pBE) {
	/* Check the validation of buffer*/
	if (!pBE) return 0x100;
	return (size_t)(unsigned char)pBE->increment;
}

/*
* Purpose: The function returns the flag field from buffer
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: None

* Parameters:	- Buffer* const pBE

* Algorithm:	1. Check the buffer validation

* Return value:	- Returns the flag field from buffer
				- RT_FAIL_1
*/
unsigned short bufferGetFlags(pBuffer const pBE) {
	/* Check the validation of buffer*/
	if (!pBE) return RT_FAIL_1;
	return (unsigned short)pBE->flags;
}

/*
* Purpose: The function returns a pointer to the location of the character buffer indicated by charPosition
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: None

* Parameters:	- Buffer* const pBE
				- short charPosition

* Algorithm:	1. Check the buffer validation
				2. Check if the value of charPosition is in the valid range

* Return value:	- Returns a pointer to the location of the character buffer indicated by charPosition
				- NULL
*/
char* bufferGetString(Buffer* const pBE, short charPosition) {
	/* Check the validation of buffer*/
	if (!pBE) return NULL;
	if (charPosition < 0 || charPosition > pBE->addCPosition) // Check if the value of charPosition is in the valid range
		return NULL; // abort and return null if it is not valid
	return pBE->string + charPosition;
}

/*
* Purpose: This function is intended to print the content of buffer (in string field)
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: printf(), bufferGetChar()

* Parameters:	- Buffer* const pBE
				- char newLine

* Algorithm:	1. Check the buffer validation
				2. Loop through the buffer to print string content

* Return value:	- Returns a number of character in buffer
				- RT_FAIL_1
*/
int bufferPrint(Buffer* const pBE, char newLine) {
	/* Check the validation of buffer*/
	if (!pBE) return RT_FAIL_1;

	char symbol;
	int count = 0;
	/* Loops through the buffer string*/
	while (1) {
		symbol = bufferGetChar(pBE); // Get 1 symbol character at a time
		if ((pBE->flags & CHECK_EOB)) break; // If it reaches the end of buffer, break the loop
		printf("%c", symbol);	// Print 1 symbol at a time
		count++; // Increase 1 for each character read
	}
	if (newLine != 0)
		printf("\n");
	return count;
}

/*
* Purpose: The function set the getCPosition and markCPosition to 0, so that the buffer can be reread again.
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: None

* Parameters:	- Buffer* const pBE

* Algorithm:	1. Check the buffer validation
				2. Set the getCPosition and markCPosition to 0

* Return value:	- Returns 0
				- RT_FAIL_1
*/
int bufferRewind(Buffer* const pBE) {
	/* Check the validation of buffer*/
	if (!pBE) return RT_FAIL_1;
	pBE->getCPosition = 0;
	pBE->markCPosition = 0;
	return 0;
}

/*
* Purpose: The function de-allocates (frees) the memory occupied by the character buffer and the Buffer structure (buffer descriptor)
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: free()

* Parameters:	- Buffer* const pBE

* Algorithm:	1. Check the buffer validation
				2. De-allocates (frees) the memory occupied by the character buffer and the Buffer structure

* Return value:	- Return void

*/
void bufferFree(Buffer* const pBE) {
	/* Check the validation of buffer*/
	if (!pBE) return;	// Frees the memory occupied by the character buffer
	free(pBE->string);	// Frees the memory occupied by the buffer structure
	free(pBE);
}

/*
* Purpose: The function retains the memory space currently allocated to the buffer, but re-initializes all appropriate data members of the given Buffer structure
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: None
* Parameters:	- Buffer* const pBE

* Algorithm:	1. Check the buffer validation
				2. Reset all appropriate data members

* Return value:	- RT_FAIL_1
				- 0

*/
int bufferClear(Buffer* const pBE) {
	/* Check the validation of buffer*/
	if (!pBE) return RT_FAIL_1;

	/* Reset all appropriate data members*/
	pBE->addCPosition = 0;
	pBE->getCPosition = 0;
	pBE->markCPosition = 0;
	pBE->flags &= RESET_EOB;
	pBE->flags &= RESET_R_FLAG;

	return 0;
}

/*
* Purpose: The function checks if the charater buffer is full or not
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: None

* Parameters:	- Buffer* const pBE

* Algorithm:	1. Check the buffer validation
				2. Checks if the buffer is full

* Return value:	- RT_FAIL_1
				- 0 (false)
				- 1 (true)

*/
int bufferIsFull(Buffer* const pBE) {
	/* Check the validation of buffer*/
	if (!pBE) return RT_FAIL_1;

	/* Checks if the buffer is full by comparing addCPosition with capacity*/
	if (pBE->addCPosition * sizeof(char) >= (size_t)pBE->capacity)
		return 1;	//  Buffer is full
	else
		return 0;	// Buffer is not full
}

/*
* Purpose: The function checks if the charater buffer is empty by the value of addCPosition
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: None

* Parameters:	- Buffer* const pBE

* Algorithm:	1. Check the buffer validation
				2. Checks if the buffer is empty

* Return value:	- RT_FAIL_1
				- 0 (false)
				- 1 (true)

*/
int bufferIsEmpty(Buffer* const pBE) {
	/* Check the validation of buffer*/
	if (!pBE) return RT_FAIL_1;

	/* Checks if the buffer is empty addCPosition value*/
	if (pBE->addCPosition == 0)
		return 1;	// Buffer is empty
	else
		return 0;	// Buffer is not empty
}

/*
* Purpose: The function shrinks (or in some cases may expand) the buffer to a new capacity for all operational modes of the buffer
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: realloc()

* Parameters:	- Buffer* const pBE
				- char symbol

* Algorithm:	1. Check the buffer validation
				2. Create new capacity by the current limit plus a space for one more character
				3. Reallocate memory for charater buffer with new capacity
				4. Update some members of buffer
				5. Add symbol to the end of buffer and increase addCPosition by 1

* Return value:	- RT_FAIL_1
				- 0 (false)
				- 1 (true)

*/
Buffer* bufferSetEnd(Buffer* const pBE, char symbol) {
	/* Check the validation of buffer*/
	if (!pBE) return NULL;

	/* Calculate new capacity by the current limit plus a space for one more character*/
	int newCapacity = (pBE->addCPosition + 1) * sizeof(char);
	if (newCapacity <= 0) return NULL; // Validate the new character

	/* Reallocate memory for charater buffer with new capacity*/
	char* buffer = realloc(pBE->string, newCapacity);
	if (!buffer) return NULL;

	/* Update some members of buffer*/
	pBE->string = buffer;
	pBE->capacity = newCapacity;

	if (pBE->string != buffer) {
		pBE->string = buffer;
		pBE->flags |= SET_R_FLAG;
	}

	/* Add symbol to the end of buffer and increase addCPosition by 1*/
	pBE->string[pBE->addCPosition++] = symbol;
	return pBE;
}

/*
* Purpose: The function decrements getCPosition by 1
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: None

* Parameters:	- Buffer* const pBE

* Algorithm:	1. Check the buffer and getCPosition validation
				2. Decrease getCPosition by 1

* Return value:	- RT_FAIL_1
				- getCPosition

*/
short bufferRetract(Buffer* const pBE) {
	/* Check the validation of buffer and getCPosition*/
	if (!pBE || pBE->getCPosition == 0) return RT_FAIL_1;

	/* Return the value of getCPosition after decrement it by 1*/
	return --pBE->getCPosition;
}

/*
* Purpose: The function returns getCPosition of buffer
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: None

* Parameters:	- Buffer* const pBE

* Algorithm:	1. Check the buffer validation
				2. Return getCPosition value

* Return value:	- RT_FAIL_1
				- getCPosition

*/
short bufferGetCPosition(Buffer* const pBE) {
	/* Check the validation of buffer*/
	if (!pBE) return RT_FAIL_1;
	return pBE->getCPosition;
}

/*
* Purpose: The function sets markCPosition to mark
* Author: Hoan Linh Cao
* History/Versions: 1.1 October 3rd, 2020
* Called functions: None

* Parameters:	- Buffer* const pBE
				- short mark

* Algorithm:	1. Check the buffer validation
				2. Check the mark (in argument) validation
				3. Set markCPosition of buffer to mark

* Return value:	- RT_FAIL_1
				- markCPosition

*/
short bufferSetMarkPosition(pBuffer const pBE, short mark) {
	/* Check the validation of buffer*/
	if (!pBE) return RT_FAIL_1;

	/* Check the mark (in argument) validation*/
	if (mark < 0 || mark > pBE->addCPosition) return RT_FAIL_1;
	pBE->markCPosition = mark;	// Set markCPosition of buffer to mark
	return pBE->markCPosition;
}

short bufferReset(Buffer* const pBE) {
	if (!pBE) return RT_FAIL_1;

	pBE->getCPosition = pBE->markCPosition;
	return pBE->getCPosition;
}