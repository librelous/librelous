// Copyright (C) 1999-2000 Id Software, Inc.
//
//
// g_mem.c
//
// Golliwog: All rewritten to allow deallocation


#include "g_local.h"

#define POOLSIZE		(256 * 1024)
#define	FREEMEMCOOKIE	((int)0xDEADBE3F)	// Any unlikely to be used value
#define	ROUNDBITS		31					// Round to 32 bytes

struct freememnode
{
	// Size of ROUNDBITS
	int cookie, size;				// Size includes node (obviously)
	struct freememnode *prev, *next;
};

static char		memoryPool[POOLSIZE];
static struct freememnode *freehead;
static int		freemem;

void *G_Alloc( int size )
{
	// Find a free block and allocate.
	// Does two passes, attempts to fill same-sized free slot first.

	struct freememnode *fmn, *prev, *next, *smallest;
	int allocsize, smallestsize;
	char *endptr;
	int *ptr;

	allocsize = ( size + sizeof(int) + ROUNDBITS ) & ~ROUNDBITS;		// Round to 32-byte boundary
	ptr = NULL;

	smallest = NULL;
	smallestsize = POOLSIZE + 1;		// Guaranteed not to miss any slots :)
	for( fmn = freehead; fmn; fmn = fmn->next )
	{
		if( fmn->cookie != FREEMEMCOOKIE )
			G_Error( "G_Alloc: Memory corruption detected!\n" );

		if( fmn->size >= allocsize )
		{
			// We've got a block
			if( fmn->size == allocsize )
			{
				// Same size, just remove

				prev = fmn->prev;
				next = fmn->next;
				if( prev )
					prev->next = next;			// Point previous node to next
				if( next )
					next->prev = prev;			// Point next node to previous
				if( fmn == freehead )
					freehead = next;			// Set head pointer to next
				ptr = (int *) fmn;
				break;							// Stop the loop, this is fine
			}
			else
      {
				// Keep track of the smallest free slot
				if( fmn->size < smallestsize )
				{
					smallest = fmn;
					smallestsize = fmn->size;
				}
			}
		}
	}
  
	if( !ptr && smallest )
	{
		// We found a slot big enough
		smallest->size -= allocsize;
		endptr = (char *) smallest + smallest->size;
		ptr = (int *) endptr;
	}

	if( ptr )
	{
		freemem -= allocsize;
		if( g_debugAlloc.integer )
			G_Printf( "G_Alloc of %i bytes (%i left)\n", allocsize, freemem );
		memset( ptr, 0, allocsize );
		*ptr++ = allocsize;				// Store a copy of size for deallocation
		return( (void *) ptr );
	}

	G_Error( "G_Alloc: failed on allocation of %i bytes\n", size );
	return( NULL );
}

void G_Free( void *ptr )
{
	// Release allocated memory, add it to the free list.

	struct freememnode *fmn;
	char *freeend;
	int *freeptr;

	freeptr = ptr;
	freeptr--;

	freemem += *freeptr;
	if( g_debugAlloc.integer )
		G_Printf( "G_Free of %i bytes (%i left)\n", *freeptr, freemem );

	for( fmn = freehead; fmn; fmn = fmn->next )
	{
		freeend = ((char *) fmn) + fmn->size;
		if( freeend == (char *) freeptr )
		{
			// Released block can be merged to an existing node

			fmn->size += *freeptr;		// Add size of node.
			return;
		}
	}
	// No merging, add to head of list

	fmn = (struct freememnode *) freeptr;
	fmn->size = *freeptr;				// Set this first to avoid corrupting *freeptr
	fmn->cookie = FREEMEMCOOKIE;
	fmn->prev = NULL;
	fmn->next = freehead;
	freehead->prev = fmn;
	freehead = fmn;
}

void G_InitMemory( void )
{
	// Set up the initial node

	freehead = (struct freememnode *) memoryPool;
	freehead->cookie = FREEMEMCOOKIE;
	freehead->size = POOLSIZE;
	freehead->next = NULL;
	freehead->prev = NULL;
	freemem = sizeof(memoryPool);
}

void G_DefragmentMemory( void )
{
	// If there's a frenzy of deallocation and we want to
	// allocate something big, this is useful. Otherwise...
	// not much use.

	struct freememnode *startfmn, *endfmn, *fmn;

	for( startfmn = freehead; startfmn; )
	{
		endfmn = (struct freememnode *)(((char *) startfmn) + startfmn->size);
		for( fmn = freehead; fmn; )
		{
			if( fmn->cookie != FREEMEMCOOKIE )
				G_Error( "G_DefragmentMemory: Memory corruption detected!\n" );

			if( fmn == endfmn )
			{
				// We can add fmn onto startfmn.

				if( fmn->prev )
					fmn->prev->next = fmn->next;
				if( fmn->next )
				{
					if( !(fmn->next->prev = fmn->prev) )
						freehead = fmn->next;	// We're removing the head node
				}
				startfmn->size += fmn->size;
				memset( fmn, 0, sizeof(struct freememnode) );	// A redundant call, really.

				startfmn = freehead;
				endfmn = fmn = NULL;				// Break out of current loop
			}
			else
				fmn = fmn->next;
		}

		if( endfmn )
			startfmn = startfmn->next;		// endfmn acts as a 'restart' flag here
	}
}

void Svcmd_GameMem_f( void )
{
	// Give a breakdown of memory

	struct freememnode *fmn;

	G_Printf( "Game memory status: %i out of %i bytes allocated\n", POOLSIZE - freemem, POOLSIZE );

	for( fmn = freehead; fmn; fmn = fmn->next )
		G_Printf( "  %dd: %d bytes free.\n", fmn, fmn->size );
	G_Printf( "Status complete.\n" );
}

/* Golliwog: All rewritten
static int		allocPoint;

void *G_Alloc( int size ) {
	char	*p;

	if ( g_debugAlloc.integer ) {
		G_Printf( "G_Alloc of %i bytes (%i left)\n", size, POOLSIZE - allocPoint - ( ( size + 31 ) & ~31 ) );
	}

	if ( allocPoint + size > POOLSIZE ) {
		G_Error( "G_Alloc: failed on allocation of %i bytes\n", size );
		return NULL;
	}

	p = &memoryPool[allocPoint];

	allocPoint += ( size + 31 ) & ~31;

	return p;
}

void G_InitMemory( void ) {
	allocPoint = 0;
}

void Svcmd_GameMem_f( void ) {
	G_Printf( "Game memory status: %i out of %i bytes allocated\n", allocPoint, POOLSIZE );
}
// Golliwog.
*/
