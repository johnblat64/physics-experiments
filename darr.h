#ifndef darr_h
#define darr_h

#include <stdlib.h>

typedef struct DarrHeader {
    int len;
    int cap;
} DarrHeader;

void *
initDarr( void *arr, size_t elmSize, size_t initCap ) {
    void *ptr = malloc(sizeof(DarrHeader) + (elmSize*initCap));
    DarrHeader header = {0};
    header.len = 0;
    header.cap = initCap;
    DarrHeader *headerPtr = (DarrHeader *) ptr;
    *(headerPtr) = header;

    arr = (char *) ptr+sizeof(DarrHeader);
    return arr;
}

size_t 
lenDarr(void *arr) {
    void *ptr = (char*) arr-sizeof(DarrHeader);
    DarrHeader *header = (DarrHeader *)ptr;
    size_t len = header->len;
    return len;
}

size_t
capDarr(void *arr) {
    void *ptr = (char*) arr-sizeof(DarrHeader);
    DarrHeader *header = (DarrHeader *)ptr;
    return header->cap;
}

void *
growDarr( void *arr, size_t elmSize, size_t newCount ) {
    void *ptr = (char *) arr - sizeof(DarrHeader);
    void *newptr = realloc(ptr, sizeof(DarrHeader) + (elmSize*newCount));
    DarrHeader header = {0};
    header.len = lenDarr(arr);
    header.cap = newCount;

    DarrHeader *headerPtr = (DarrHeader *) newptr;
    *(headerPtr) = header;

    arr = (char *) newptr+sizeof(DarrHeader);
    return arr;
}

// void addDarr(void *arr, void *valPtr, size_t sizeVal) {
//     if(lenDarr(arr) > capDarr(arr)) {
//         arr = growDarr(arr, sizeVal, lenDarr(arr) * 2);
//     }
//     arr[lenDarr(arr)] = *valPtr;
// }

#define addDarr(arr, val, T) \
{ \
    if(lenDarr(arr) > capDarr(arr)) { \
        (arr) = growDarr(arr,sizeof(T), lenDarr(arr) *2); \
    } \
    (arr)[lenDarr(arr)] = val; \
    void *ptr = (char *)arr-sizeof(DarrHeader); \
    DarrHeader *header = (DarrHeader *)ptr; \
    header->len++; \
}

#endif