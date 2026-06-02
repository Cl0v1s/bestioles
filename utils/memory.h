#pragma once 

// this is for the lolz

#define POOL_SIZE 1024
#define PTR_SIZE 20

static unsigned char pool[POOL_SIZE];
static unsigned int poolIndex = 0;

typedef struct {    
    void* data;
    unsigned int size;
} Ptr;

static Ptr ptrs[PTR_SIZE];
static unsigned int ptrsIndex = 0;


inline Ptr* pool_alloc(unsigned int size) {
    if(size == 0 || poolIndex + size > POOL_SIZE || ptrsIndex + 1 > PTR_SIZE) return 0;
    Ptr* p = &ptrs[ptrsIndex];

    p->data = &pool[poolIndex];
    p->size = size;

    ptrsIndex += 1;
    poolIndex += size;

    return p;
}

inline unsigned char pool_copy(Ptr* dest, void* source, unsigned int len) {
    if(dest->data == 0 || source == 0) return 1;
    for(unsigned int i = 0; i < len; i++) {
        ((unsigned char*)dest->data)[i] = ((unsigned char*)source)[i];
    }
    return 0;
}

inline unsigned char pool_free(Ptr* ptr) {
    unsigned int i = ptr - ptrs;
    // we mark this one for deletion
    ptr->data = 0;
    if(i != ptrsIndex - 1) {
        return 1;
    }
    // we try to compact if others were fred
    while(i >= 0 && ptrs[i].data == 0) {
        poolIndex -= ptrs[i].size;
        ptrsIndex -= 1;
        i -= 1;
    }
    return 0;
}

inline void pool_reset() {
    poolIndex = 0;
    ptrsIndex = 0;
}