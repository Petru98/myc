#include "myc/string.h"


/*
 * SmallString
 */
void String_init(String* self) {
    assert(self != NULL);
    self->capacity = 0;
    self->s.size = 0;
    self->s.buf[0] = 0;
}

void String_free(String* self) {
    assert(self != NULL);
    if(self->capacity > 0) {
        free(self->b.ptr);
        self->capacity = 0;
    }
    self->s.size = 0;
    self->s.buf[0] = 0;
}


char* String_data(String* self) {
    assert(self != NULL);
    return self->capacity == 0 ? self->s.buf : self->b.ptr;
}
size_t String_size(String* self) {
    assert(self != NULL);
    return self->capacity == 0 ? self->s.size : self->b.size;
}


void String_reserve(String* self, size_t size) {
    assert(self != NULL);
    if(self->capacity == 0) {
        if(size >= sizeof(self->s.buf)) {
            char* const ptr = myc_malloc(size + 1);
            memcpy(ptr, self->s.buf, self->s.size + 1);
            self->capacity = size;
            self->b.size = self->s.size;
            self->b.ptr = ptr;
        }
    }
    else {
        if(size > self->capacity) {
            self->capacity = size;
            self->b.ptr = myc_realloc(self->b.ptr, self->capacity + 1);
        }
    }
}

void String_resize(String* self, size_t size) {
    assert(self != NULL);
    if(self->capacity == 0) {
        if(size != self->s.size) {
            if(size >= sizeof(self->s.buf)) {
                char* const ptr = myc_malloc(size + 1);
                memcpy(ptr, self->s.buf, self->s.size);
                self->capacity = size;
                self->b.size = size;
                self->b.ptr = ptr;
                self->b.size = size;
                self->b.ptr[self->b.size] = 0;
            }
            else {
                self->s.size = size;
                self->s.buf[self->s.size] = 0;
            }
        }
    }
    else {
        if(size != self->b.size) {
            if(size >= self->capacity) {
                self->capacity = size;
                self->b.ptr = myc_realloc(self->b.ptr, self->capacity + 1);
            }
            self->b.size = size;
            self->b.ptr[self->b.size] = 0;
        }
    }
}

void String_fit(String* self) {
    assert(self != NULL);
    if(self->capacity > 0) {
        if(self->b.size < self->capacity) {
            if(self->b.size >= sizeof(self->s.buf)) {
                self->capacity = self->b.size;
                self->b.ptr = myc_realloc(self->b.ptr, self->capacity + 1);
            }
            else {
                char* const ptr = self->b.ptr;
                self->capacity = 0;
                self->s.size = (uint8_t)self->b.size;
                memcpy(self->s.buf, ptr, self->s.size + 1);
                free(ptr);
            }
        }
    }
}


void String_pushBack(String* self, char ch) {
    assert(self != NULL);
    if(self->capacity == 0) {
        if((unsigned)self->s.size + 1 < sizeof(self->s.buf)) {
            self->s.buf[self->s.size++] = ch;
            self->s.buf[self->s.size] = 0;
        }
        else {
            const unsigned newcap = 32;
            char* const ptr = myc_malloc(newcap + 1);
            memcpy(ptr, self->s.buf, sizeof(self->s.buf) - 1);
            ptr[sizeof(self->s.buf)] = ch;
            ptr[sizeof(self->s.buf) + 1] = 0;
            self->capacity = newcap;
            self->b.size = self->s.size;
            self->b.ptr = ptr;
        }
    }
    else {
        if(self->b.size + 1 == self->capacity) {
            self->capacity = self->capacity * 2;
            self->b.ptr = myc_realloc(self->b.ptr, self->capacity + 1);
        }
        self->b.ptr[self->b.size++] = ch;
        self->b.ptr[self->b.size] = 0;
    }
}

char String_popBack(String* self) {
    assert(self != NULL);
    assert(String_size(self) > 0);
    char ch;
    if(self->capacity == 0) {
        ch = self->s.buf[--self->s.size];
        self->s.buf[self->s.size] = 0;
    }
    else {
        ch = self->b.ptr[--self->b.size];
        self->b.ptr[self->b.size] = 0;
    }
    return ch;
}


int String_cmp(String* self, String* other) {
    assert(self != NULL);
    assert(other != NULL);

    char* p1;
    size_t s1;
    if(self->capacity == 0) {
        p1 = self->s.buf;
        s1 = self->s.size;
    } else {
        p1 = self->b.ptr;
        s1 = self->b.size;
    }

    char* p2;
    size_t s2;
    if(other->capacity == 0) {
        p2 = other->s.buf;
        s2 = other->s.size;
    } else {
        p2 = other->b.ptr;
        s2 = other->b.size;
    }

    if(s1 < s2) return -1;
    if(s1 > s2) return  1;
    return memcmp(p1, p2, s1);
}



/*
 * String
 */
static inline StrHdr* StrPtr_hdr(char* str) {
    assert(str != NULL);
    return (StrHdr*)(str - offsetof(StrHdr, buf));
}


void StrPtr_free(StrPtr str) {
    if(str != NULL)
        free(StrPtr_hdr(str));
}


size_t StrPtr_size(StrPtr str) {
    return str == NULL ? 0 : StrPtr_hdr(str)->size;
}
size_t StrPtr_capacity(StrPtr str) {
    return str == NULL ? 0 : StrPtr_hdr(str)->capacity;
}


StrPtr StrPtr_reserve(StrPtr str, size_t size) {
    if(str == NULL) {
        if(size > 0) {
            StrHdr* self = myc_malloc(sizeof(StrHdr) + size + 1);
            self->capacity = 0;
            self->size = 0;
            self->buf[0] = 0;
            str = self->buf;
        }
    }
    else {
        StrHdr* self = StrPtr_hdr(str);
        if(size > self->capacity) {
            self = (StrHdr*)myc_realloc(self, sizeof(StrHdr) + size + 1);
            self->capacity = size;
            str = self->buf;
        }
    }
    return str;
}

StrPtr StrPtr_resize(StrPtr str, size_t size) {
    if(str == NULL) {
        if(size > 0) {
            StrHdr* self = myc_malloc(sizeof(StrHdr) + size + 1);
            self->capacity = size;
            self->size = size;
            self->buf[size] = 0;
            str = self->buf;
        }
    }
    else {
        StrHdr* self = StrPtr_hdr(str);
        if(size != self->size) {
            if(size > self->capacity) {
                self->capacity = size;
                self = (StrHdr*)myc_realloc(self, sizeof(StrHdr) + self->capacity + 1);
                str = self->buf;
            }
            self->size = size;
            self->buf[size] = 0;
        }
    }
    return str;
}

StrPtr StrPtr_fit(StrPtr str) {
    if(str != NULL) {
        StrHdr* self = StrPtr_hdr(str);
        if(self->size != self->capacity) {
            if(self->size == 0) {
                free(self);
                str = NULL;
            }
            else {
                self->capacity = self->size;
                self = (StrHdr*)myc_realloc(self, sizeof(StrHdr) + self->capacity + 1);
                str = self->buf;
            }
        }
    }
    return str;
}


StrPtr StrPtr_pushBack(StrPtr str, char ch) {
    StrHdr* self;
    if(str == NULL) {
        const unsigned capacity = 32;
        self = myc_malloc(sizeof(StrHdr) + capacity + 1);
        self->capacity = capacity;
        self->size = 0;
        str = self->buf;
        // no need to initialize self->buf[0]
    }
    else {
        self = StrPtr_hdr(str);
        if(self->size == self->capacity) {
            self->capacity = self->capacity * 2;
            self = myc_realloc(self, sizeof(StrHdr) + self->capacity + 1);
            str = self->buf;
        }
    }

    self->buf[self->size++] = ch;
    self->buf[self->size] = 0;
    return str;
}

char StrPtr_popBack(StrPtr str) {
    assert(str != NULL);
    StrHdr* self = StrPtr_hdr(str);
    assert(self->size > 0);

    const char ch = self->buf[--self->size];
    self->buf[self->size] = 0;
    return ch;
}


int StrPtr_cmp(StrPtr s1, StrPtr s2) {
    if(s1 == s2) return 0;
    if(s1 == NULL) return (s2 != NULL && StrPtr_hdr(s2)->size > 0) ? -1 : 0;
    if(s2 == NULL) return (s1 != NULL && StrPtr_hdr(s1)->size > 0) ?  1 : 0;

    StrHdr* self = StrPtr_hdr(s1);
    StrHdr* other = StrPtr_hdr(s2);
    if(self->size < other->size) return -1;
    if(self->size > other->size) return  1;
    return memcmp(self->buf, other->buf, self->size);
}
