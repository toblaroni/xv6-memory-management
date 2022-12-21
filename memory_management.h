#include "kernel/types.h"
#include "user/user.h"
#include <stddef.h>

typedef struct chunkList {
	int isAllocated; // Flag that says whether the chunk is free or allocated
	int size; // Size of current chunk
	struct chunkList *nextChunk; // Pointer to next chunk

} chunk;

void *_malloc(int size);
void _free(void *);

