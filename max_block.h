#ifndef MAX_BLOCK_STRUCT
#define MAX_BLOCK_STRUCT

#include "alloc_block.h"

struct max_block
{
    int length;
    alloc_block *block;
};

#endif