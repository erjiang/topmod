/**

 * $Id$
 * Copyright (C) 2001 NaN Technologies B.V.
 * Guarded memory (de)allocation
 *
 *
 * @mainpage MEM - c-style guarded memory allocation
 *
 * @section about About the MEM module
 *
 * MEM provides guarded malloc/calloc calls. All memory is enclosed by
 * pads, to detect out-of-bound writes. All blocks are placed in a
 * linked list, so they remain reachable at all times. There is no
 * back-up in case the linked-list related data is lost.
 *
 * @section issues Known issues with MEM
 *
 * There are currently no known issues with MEM. Note that there is a
 * second intern/ module with MEM_ prefix, for use in c++.
 * 
 * @section dependencies Dependencies
 *
 * - stdlib
 *
 * - stdio
 *
 * */

#ifndef MEM_MALLOCN_H
#define MEM_MALLOCN_H

/* Needed for FILE* */
#include "stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

	/** Returns the lenght of the allocated memory segment pointed at
	 * by vmemh. If the pointer was not previously allocated by this
	 * module, the result is undefined.*/
	int MEM_allocN_len(void *vmemh);

	/**
	 * Release memory previously allocatred by this module. 
	 */
	short MEM_freeN(void *vmemh);

	/**
	 * Duplicates a block of memory, and returns a pointer to the
	 * newly allocated block.  */
	void *MEM_dupallocN(void *vmemh);

	/**
	 * Allocate a block of memory of size len, with tag name str. The
	 * memory is cleared. The name must be static, because only a
	 * pointer to it is stored ! */
	void *MEM_callocN(unsigned int len, char * str);
	
	/** Allocate a block of memory of size len, with tag name str. The
	 * name must be a static, because only a pointer to it is stored !
	 * */
	void *MEM_mallocN(unsigned int len, char * str);

	/** Print a list of the names and sizes of all allocated memory
	 * blocks. */ 
	void MEM_printmemlist(void);

	/** Set the callback function for error output. */
	void MEM_set_error_callback(void (*func)(char *));

	/**
	 * Are the start/end block markers still correct ?
	 *
	 * @retval 0 for correct memory, 1 for corrupted memory. */
	int MEM_check_memory_integrity(void);

#ifdef __cplusplus
}
#endif

#endif

