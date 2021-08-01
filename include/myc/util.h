#pragma once
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#ifdef __cplusplus
    #define EXTERNC extern "C"
    #define EXTERNC_BEG extern "C" {
    #define EXTERNC_END
#else
    #define EXTERNC
    #define EXTERNC_BEG
    #define EXTERNC_END
#endif


EXTERNC_BEG

typedef struct _Notype* _Notype;  /*NOLINT(bugprone-reserved-identifier)*/



static inline void* myc_malloc(size_t size) {
    void* ptr = malloc(size);
    if(ptr == NULL)
        abort();
    return ptr;
}
static inline void* myc_calloc(size_t num, size_t size) {
    void* ptr = calloc(num, size);
    if(ptr == NULL)
        abort();
    return ptr;
}
static inline void* myc_realloc(void* ptr, size_t size) {
    ptr = realloc(ptr, size);
    if(ptr == NULL)
        abort();
    return ptr;
}
static inline void* myc_aligned_alloc(size_t alignment, size_t size) {
    void* ptr = aligned_alloc(alignment, size);
    if(ptr == NULL)
        abort();
    return ptr;
}

static inline char* myc_strdup(const char* s) {
    char* ptr = strdup(s);
    if(ptr == NULL)
        abort();
    return ptr;
}
static inline void* myc_memdup(const void* mem, size_t size) {
    void* ptr = myc_malloc(size);
    memcpy(ptr, mem, size);
    return ptr;
}



size_t myc_bsearchin(const void* key, const void* ptr, size_t count, size_t size, int (*comp)(const void*, const void*));
static inline void* myc_bsearchit(const void* key, const void* ptr, size_t count, size_t size, int (*comp)(const void*, const void*)) {
    return (void*)((const char*)ptr + (myc_bsearchin(key, ptr, count, size, comp) * size));
}
size_t myc_upboundin(const void* key, const void* ptr, size_t count, size_t size, int (*comp)(const void*, const void*));
static inline void* myc_upboundit(const void* key, const void* ptr, size_t count, size_t size, int (*comp)(const void*, const void*)) {
    return (void*)((const char*)ptr + (myc_upboundin(key, ptr, count, size, comp) * size));
}
size_t myc_loboundin(const void* key, const void* ptr, size_t count, size_t size, int (*comp)(const void*, const void*));
static inline void* myc_loboundit(const void* key, const void* ptr, size_t count, size_t size, int (*comp)(const void*, const void*)) {
    return (void*)((const char*)ptr + (myc_loboundin(key, ptr, count, size, comp) * size));
}



#define MYC_INTERNAL_DEFINE_ERRORS(DEFINE) \
    DEFINE(MYC_ENONE, "no error") \
    DEFINE(MYC_ESYS , "system error")

typedef enum {
    #define MYC_INTERNAL_DEFINE_ERROR(no, msg) no,
    MYC_INTERNAL_DEFINE_ERRORS(MYC_INTERNAL_DEFINE_ERROR)
    #undef MYC_INTERNAL_DEFINE_ERROR
} MycErr;


extern const char* const myc_errmsg[];

EXTERNC_END
