#include "myc/SourceFile.h"
#include "myc/array.h"
#include "myc/util.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>


void MycSourceFile_init(MycSourceFile* src) {
    (*src) = (MycSourceFile){NULL, NULL, 0, 0, 0, 0, {0, 0}};
}


bool MycSourceFile_open(MycSourceFile* src, const char* name) {
    assert(src->buffer == NULL);

    src->fd = open(name, O_RDONLY|O_CLOEXEC);
    if(src->fd == -1)
        return false;

    src->bufcap = 16*1024;
    src->buffer = (char*)myc_malloc(src->bufcap);
    src->bufend = 0;
    src->bufbeg = 0;

    src->name = myc_strdup(name);

    src->pos.line = 0;
    src->pos.col = 0;
    return true;
}


void MycSourceFile_close(MycSourceFile* src) {
    if(src->buffer) {
        src->buffer = NULL;
        src->bufcap = 0;
        if(src->fd != -1) {
            close(src->fd);
            src->fd = -1;
        }
        if(src->name != NULL) {
            free(src->name);
            src->name = NULL;
        }
    }
}


int MycSourceFile_peek(MycSourceFile* src) {
    assert(src->bufbeg <= src->bufend);
    if(src->bufbeg == src->bufend) {
        errno = 0;
        do {
            const ssize_t r = read(src->fd, src->buffer, src->bufcap);
            if(r <= 0)
                return -1;
            src->bufend = (myc_size_t)r;
            src->bufbeg = 0;
        } while(src->bufbeg == src->bufend);
    }

    char ch = src->buffer[src->bufbeg];
    if(ch == '\r') {
        if(src->bufbeg == src->bufend) {
            errno = 0;
            const ssize_t r = read(src->fd, src->buffer, src->bufcap);
            if(r < 0)
                return -1;
            src->bufend = (myc_size_t)r;
            src->bufbeg = 0;
        }
        if(src->bufbeg < src->bufend && src->buffer[src->bufbeg] == '\n') {
            src->bufbeg++;
            ch = '\n';
        }
    }

    return ch;
}

int MycSourceFile_getc(MycSourceFile* src) {
    const int ch = MycSourceFile_peek(src);
    if(ch == -1)
        return -1;

    src->bufbeg++;
    if(ch == '\n') {
        src->pos.line++;
        src->pos.col = 0;
    }
    else {
        src->pos.col++;
    }

    return ch;
}


int MycSourceFile_skip(MycSourceFile* src, myc_size_t count) {
    while(count > 0) {
        int ch = MycSourceFile_getc(src);
        if(ch < 0) {
            if(errno == 0)
                ch = 0;
            return ch;
        }
        count--;
    }
    return 1;
}

int MycSourceFile_reqbuf(MycSourceFile* src, myc_size_t count) {
    assert(count <= src->bufcap);

    const myc_size_t target = src->bufbeg + count;
    if(target > src->bufend) {
        if(target > src->bufcap) {
            memmove(src->buffer, src->buffer + src->bufbeg, src->bufend - src->bufbeg);
            src->bufend -= src->bufbeg;
            src->bufbeg = 0;
        }

        errno = 0;
        do {
            const ssize_t r = read(src->fd, src->buffer + src->bufend, src->bufcap - src->bufend);
            if(r <= 0)
                return r;
            src->bufend += (myc_size_t)r;
        } while(target > src->bufend);
    }

    return 1;
}

// const char* MycSourceFile_buffer(MycSourceFile* src) {
//     assert(src->bufbeg < src->bufend);
//     return src->buffer + src->bufbeg;
// }
// myc_size_t MycSourceFile_bufsize(MycSourceFile* src) {
//     assert(src->bufbeg <= src->bufend);
//     return src->bufend - src->bufbeg;
// }
