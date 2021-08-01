#pragma once
#include "array.h"
#include "string.h"
#include "util.h"
#include <stdbool.h>


EXTERNC_BEG

typedef uint32_t myc_size_t;
typedef int32_t myc_ssize_t;

/*#define MYC_DEFINE_TRIVIAL_CMP_FUNC(T) \
    static inline int cmp_##T(const void* a, const void* b) {   \
        const T x = *((const T*)a), y = *((const T*)b);         \
        return x < y ? -1 : (x > y ? 1 : 0);                    \
    }
//*/


typedef struct MycSourcePosition {
    myc_size_t line, col;
} MycSourcePosition;



typedef struct MycSourceFile {
    char* name;
    char* buffer;
    myc_size_t bufcap;
    myc_size_t bufbeg;
    myc_size_t bufend;
    int fd;

    MycSourcePosition pos;
} MycSourceFile;

void MycSourceFile_init(MycSourceFile* src);

bool MycSourceFile_open(MycSourceFile* src, const char* name);
void MycSourceFile_close(MycSourceFile* src);

int MycSourceFile_peek(MycSourceFile* src);
int MycSourceFile_getc(MycSourceFile* src);

int MycSourceFile_skip(MycSourceFile* src, myc_size_t count);
int MycSourceFile_reqbuf(MycSourceFile* src, myc_size_t count);

// const char* MycSourceFile_buffer(MycSourceFile* src);
// myc_size_t MycSourceFile_bufsize(MycSourceFile* src);

EXTERNC_END
