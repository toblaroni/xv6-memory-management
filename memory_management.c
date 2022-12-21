#include "memory_management.h"

#define FREE_CHUNK 1
#define ALLOCATED_CHUNK 0

chunk *head = NULL;

// Iterating over a list - https://www.learn-c.org/en/Linked_lists
// Returns the last chunk in the list of chunks
chunk *getLastChunk(chunk *h){
	chunk *currentChunk = h;

	while(currentChunk->nextChunk != NULL){
		currentChunk = currentChunk->nextChunk;
	}

	return currentChunk;
}

// s = size to allocate
// Returns pointer to allocated chunk
chunk *firstFit(int s){
	chunk *currentChunk = head;

	while(currentChunk != NULL){
		if(currentChunk->isAllocated == FREE_CHUNK){
			// Remainder should leave enough space for a header + 1
			int remainder = currentChunk->size - s;

			// If it's just the same size just change the flag
			if(currentChunk->size == s){
				currentChunk->isAllocated = ALLOCATED_CHUNK;
				return currentChunk;
			} else if (remainder >= sizeof(chunk) + 1){ 
				chunk *nChunkPtr = currentChunk->nextChunk; // Temporary variable to hold chunk ptr

				currentChunk->isAllocated = ALLOCATED_CHUNK; // Mark as allocated
				currentChunk->size = s;

				// This chunk now points to a free segment
				currentChunk->nextChunk = (void *) (chunk*)currentChunk + sizeof(chunk) + currentChunk->size;
				currentChunk->nextChunk->isAllocated = FREE_CHUNK;
				currentChunk->nextChunk->size = remainder - sizeof(chunk);
				currentChunk->nextChunk->nextChunk = nChunkPtr; // New free chunk now points to next contiguious chunk
				return currentChunk;
			}
		}
		// Move to the next chunk...
		currentChunk = currentChunk->nextChunk;
	}

	return NULL;
}

// Function that merges contiguous free chunks
void coalesce(){
	// ITERATE THROUGH THE LINKED LIST
	chunk *cChunk = head; // Iterator chunk pointer

	// If there's no next chunk exit loop
	while (cChunk != NULL){
		if(cChunk->nextChunk == NULL) return;
		// IF CURRENT NODE IS FREE && NEXTNODE IS FREE
		if(cChunk->isAllocated == FREE_CHUNK && cChunk->nextChunk->isAllocated == FREE_CHUNK){
			// FLAG STAYS THE SAME
			// SIZE = CURRENT->SIZE + NEXTNODE->SIZE + SIZEOF(CHUNK)
			// CURRENT->NEXTNODE = NEXTNODE->NEXTNODE
			// NCHUNK = cChunk
			chunk *nChunk = cChunk->nextChunk;
			chunk *tmpPtr = nChunk->nextChunk; // Ptr that will become the coalesced blocks new nextChunk ptr

			cChunk->size = nChunk->size + sizeof(chunk) + cChunk->size;
			cChunk->nextChunk = tmpPtr;

			nChunk = cChunk; // Both pointers at the same address

			coalesce(); // Function recursively coalesces free chunks
		}

		cChunk = cChunk->nextChunk;
	}
	return;	
}

void *_malloc(int size){
	if(size <= 0)
		return NULL;

	// If head is NULL, there's no heap.
	if(head == NULL){
		head = (chunk *) sbrk((unsigned long)size + sizeof(chunk)); // head becomes the start of the heap
		head->isAllocated = ALLOCATED_CHUNK;
		head->size = size;
		head->nextChunk = NULL;
		return (void *) head + sizeof(chunk);
	} else {
		// Coalesce contiguous free chunks
		coalesce();
		// if There's an appropriate chunk
		// Allocate the memory there.
		chunk *apprChunk = firstFit(size);
		if(apprChunk != NULL)
			return (void *) apprChunk + sizeof(chunk); // Return pointer to begining of data.

		// if there's no suitable chunk -> Add memory to the end of the heap
		chunk *lChunk = getLastChunk(head);

		lChunk->nextChunk = (chunk *) sbrk((unsigned long) size + sizeof(chunk));
		lChunk->nextChunk->isAllocated = ALLOCATED_CHUNK;
		lChunk->nextChunk->size = size;
		lChunk->nextChunk->nextChunk = NULL;

		return (void *) lChunk->nextChunk + sizeof(chunk); // Return pointer to beginning of the memory space
	}
}

void _free(void *ptr){

	// Access the header of the memory we are pointing to
	chunk *header = (chunk *) ptr - 1;

	chunk *cChunk = head;

	// Loop through the List and try and find a node with the same address as the head of the ptr parameter
	while(cChunk != NULL){
		if(header == cChunk){
			cChunk->isAllocated = FREE_CHUNK;
			return;
		}
		cChunk = cChunk->nextChunk;
	} 
	return;
}
