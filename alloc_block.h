#ifndef ALLOCATION_STRUCT
#define ALLOCATION_STRUCT

struct alloc_block
{
    int bsize;
    char *bword;
    bool operator==(const alloc_block &ab)
    {
        return bsize == ab.bsize &&
               bword == ab.bword;
    }
};

#endif