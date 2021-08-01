#pragma once
#include "util.h"


#define Array2(T, size_type) \
    struct {                \
        size_type capacity; \
        size_type size;     \
        T* data;            \
    }
#define Array(T) \
    Array2(T, size_t)


#define Array_free(arr) \
    do {                        \
        if((arr)->capacity)     \
            free((arr)->data);  \
        (arr)->size = 0;        \
        (arr)->capacity = 0;    \
        (arr)->data = NULL;     \
    } while(0)


#define Array_pushBack(arr, elem) \
    do {                                                                                                    \
        if((arr)->size == (arr)->capacity) {                                                                \
            (arr)->capacity = (arr)->capacity ? (arr)->capacity * 2 : 16;                                   \
            (*((void**)(&((arr)->data)))) = myc_realloc((arr)->data, (arr)->capacity * sizeof(*(arr)->data));  \
        }                                                                                                   \
        (arr)->data[(arr)->size++] = elem;                                                                  \
    } while(0)


#define Array_popBack(arr)      \
    do {                        \
        assert((arr)->data);    \
        assert((arr)->size);    \
        (arr)->size--;          \
    } while(0)


#define Array_erase(arr, i) \
    do {                                                                                                    \
        assert((arr)->data);                                                                                \
        assert((arr)->size);                                                                                \
        memmove((arr)->data + (i), (arr)->data + (i) + 1, ((arr)->size - (i) - 1) * sizeof(*(arr)->data));  \
        (arr)->size--;                                                                                      \
    } while(0)


#define Array_back(arr) \
    (arr)->data[(arr)->size - 1]


#define Array_realloc(arr, len) \
    do {                                                                                        \
        const size_t size = (len);                                                              \
        assert(size >= (arr)->size);                                                            \
        if(size != (arr)->capacity) {                                                           \
            (*((void**)(&((arr)->data)))) = myc_realloc((arr)->data, size * sizeof(*(arr)->data)); \
            (arr)->capacity = size;                                                             \
        }                                                                                       \
    } while(0)


#define Array_reserve(arr, len) \
    do {                                                                                        \
        const size_t size = (len);                                                              \
        if(size > (arr)->capacity) {                                                            \
            (*((void**)(&((arr)->data)))) = myc_realloc((arr)->data, size * sizeof(*(arr)->data)); \
            (arr)->capacity = size;                                                             \
        }                                                                                       \
    } while(0)


#define Array_fit(arr) \
    do {                                                                                                \
        if((arr)->size < (arr)->capacity) {                                                             \
            (*((void**)(&((arr)->data)))) = myc_realloc((arr)->data, (arr)->size * sizeof(*(arr)->data));  \
            (arr)->capacity = (arr)->size;                                                              \
        }                                                                                               \
    } while(0)



/*
 * View
 */
#define View2(T, size_type) \
    struct {            \
        T* data;        \
        size_type size; \
    }
#define View(T) \
    View2(T, size_t)

typedef View(char) StringView;
typedef View(const char) CStringView;




#define MYC_ARR_TYPEDEF(S, T, size_type) \
    typedef struct S##Hdr { \
        size_type capacity; \
        size_type size;     \
        T buf[];            \
    } S##Hdr;               \
    typedef T* S##Ptr;  /*NOLINT(bugprone-macro-parentheses)*/


#define MYC_ARR_DECLARE_FUNCS(S, T, size_type, specifiers) \
    specifiers void S##Ptr_delete(S##Ptr ptr);                                                      \
                                                                                                    \
    specifiers size_type S##Ptr_size(S##Ptr ptr);                                                   \
    specifiers size_type S##Ptr_capacity(S##Ptr ptr);                                               \
                                                                                                    \
    specifiers S##Ptr S##Ptr_reserve(S##Ptr ptr, size_type size);                                   \
    specifiers S##Ptr S##Ptr_resize(S##Ptr ptr, size_type size);                                    \
    specifiers S##Ptr S##Ptr_fit(S##Ptr ptr);                                                       \
                                                                                                    \
    specifiers S##Ptr S##Ptr_pushBackP(S##Ptr ptr, T* x);  /*NOLINT(bugprone-macro-parentheses)*/   \
    specifiers S##Ptr S##Ptr_pushBack(S##Ptr ptr, T x);                                             \
    specifiers void S##Ptr_popBack(S##Ptr ptr);            /*NOLINT(bugprone-macro-parentheses)*/


#define MYC_ARR_DEFINE_FUNCS(S, T, size_type, specifiers, sentry) \
    static inline S##Hdr* S##Ptr_hdr(S##Ptr ptr) {                                      \
        assert(ptr != NULL);                                                            \
        return (S##Hdr*)(ptr - offsetof(S##Hdr, buf));                                  \
    }                                                                                   \
                                                                                        \
    /*NOLINTNEXTLINE(bugprone-macro-parentheses)*/                                      \
    static inline void S##Ptr_sentryset(T* ptr) {                                       \
        assert(ptr != NULL);                                                            \
        _Generic(sentry, _Notype: (void)0, default: (*ptr) = (sentry));                 \
    }                                                                                   \
    static inline int S##Ptr_sentrycnt() {                                              \
        return _Generic(sentry, _Notype: 0, default: 1);                                \
    }                                                                                   \
                                                                                        \
    static inline S##Hdr* S##Ptr_malloc(size_type size) {                               \
        return (S##Hdr*)myc_malloc(size + sizeof(S##Hdr) + S##Ptr_sentrycnt());         \
    }                                                                                   \
    static inline S##Hdr* S##Ptr_realloc(S##Hdr* ptr, size_type size) {                 \
        return (S##Hdr*)myc_realloc(ptr, size + sizeof(S##Hdr) + S##Ptr_sentrycnt());   \
    }                                                                                   \
    static inline void S##Ptr_free(S##Hdr* ptr) {                                       \
        return free(ptr);                                                               \
    }                                                                                   \
                                                                                        \
                                                                                        \
    /*NOLINTNEXTLINE(bugprone-macro-parentheses)*/                                      \
    specifiers S##Ptr S##Ptr_delete(S##Ptr ptr) {                                       \
        if(ptr != NULL)                                                                 \
            S##Ptr_free(S##Ptr_hdr(ptr));                                               \
        return NULL;                                                                    \
    }                                                                                   \
                                                                                        \
    specifiers size_type S##Ptr_size(S##Ptr ptr) {                                      \
        return ptr == NULL ? 0 : S##Ptr_hdr(ptr)->size;                                 \
    }                                                                                   \
    specifiers size_type S##Ptr_capacity(S##Ptr ptr) {                                  \
        return ptr == NULL ? 0 : S##Ptr_hdr(ptr)->capacity;                             \
    }                                                                                   \
                                                                                        \
    specifiers S##Ptr S##Ptr_reserve(S##Ptr ptr, size_type size) {                      \
        if(ptr == NULL) {                                                               \
            if(size > 0) {                                                              \
                S##Hdr* self = S##Ptr_malloc(size);                                     \
                self->capacity = 0;                                                     \
                self->size = 0;                                                         \
                S##Ptr_sentryset(self->buf);                                            \
                ptr = self->buf;                                                        \
            }                                                                           \
        }                                                                               \
        else {                                                                          \
            S##Hdr* self = S##Ptr_hdr(ptr);                                             \
            if(size > self->capacity) {                                                 \
                self = (S##Hdr*)S##Ptr_realloc(self, size);                             \
                self->capacity = size;                                                  \
                ptr = self->buf;                                                        \
            }                                                                           \
        }                                                                               \
        return ptr;                                                                     \
    }                                                                                   \
    specifiers S##Ptr S##Ptr_resize(S##Ptr ptr, size_type size) {                       \
        if(ptr == NULL) {                                                               \
            if(size > 0) {                                                              \
                S##Hdr* self = S##Ptr_malloc(size);                                     \
                self->capacity = size;                                                  \
                self->size = size;                                                      \
                S##Ptr_sentryset(self->buf + size);                                     \
                ptr = self->buf;                                                        \
            }                                                                           \
        }                                                                               \
        else {                                                                          \
            S##Hdr* self = S##Ptr_hdr(ptr);                                             \
            if(size != self->size) {                                                    \
                if(size > self->capacity) {                                             \
                    self->capacity = size;                                              \
                    self = (S##Hdr*)S##Ptr_realloc(self, self->capacity);               \
                    ptr = self->buf;                                                    \
                }                                                                       \
                self->size = size;                                                      \
                S##Ptr_sentryset(self->buf + size);                                     \
            }                                                                           \
        }                                                                               \
        return ptr;                                                                     \
    }                                                                                   \
    specifiers S##Ptr S##Ptr_fit(S##Ptr ptr) {                                          \
        if(ptr != NULL) {                                                               \
            S##Hdr* self = S##Ptr_hdr(ptr);                                             \
            if(self->size != self->capacity) {                                          \
                if(self->size == 0) {                                                   \
                    free(self);                                                         \
                    ptr = NULL;                                                         \
                }                                                                       \
                else {                                                                  \
                    self->capacity = self->size;                                        \
                    self = (S##Hdr*)S##Ptr_realloc(self, self->capacity);               \
                    ptr = self->buf;                                                    \
                }                                                                       \
            }                                                                           \
        }                                                                               \
        return ptr;                                                                     \
    }                                                                                   \
                                                                                        \
    /*NOLINTNEXTLINE(bugprone-macro-parentheses)*/                                      \
    specifiers S##Ptr S##Ptr_pushBackP(S##Ptr ptr, T* x) {                              \
        MYC_INTERNAL_ARR_PUSHBACK(S, T, ptr, *x);                                       \
        return ptr;                                                                     \
    }                                                                                   \
    specifiers S##Ptr S##Ptr_pushBack(S##Ptr ptr, T x) {                                \
        MYC_INTERNAL_ARR_PUSHBACK(S, T, ptr, x);                                        \
        return ptr;                                                                     \
    }                                                                                   \
                                                                                        \
    /*NOLINTNEXTLINE(bugprone-macro-parentheses)*/                                      \
    specifiers void S##Ptr_popBack(S##Ptr ptr) {                                        \
        assert(ptr != NULL);                                                            \
        S##Hdr* self = S##Ptr_hdr(ptr);                                                 \
        assert(self->size > 0);                                                         \
                                                                                        \
        self->size--;                                                                   \
        S##Ptr_sentryset(self->buf + self->size);                                       \
    }


#define MYC_INTERNAL_ARR_PUSHBACK(S, T, ptr, x) \
    do {                                                        \
        S##Hdr* self;                                           \
        if((ptr) == NULL) {                                     \
            const unsigned capacity = 32;                       \
            self = S##Ptr_malloc(capacity);                     \
            self->capacity = capacity;                          \
            self->size = 0;                                     \
            (ptr) = self->buf;                                  \
        }                                                       \
        else {                                                  \
            self = S##Ptr_hdr(ptr);                             \
            if(self->size == self->capacity) {                  \
                self->capacity = self->capacity * 2;            \
                self = S##Ptr_realloc(self, self->capacity);    \
                (ptr) = self->buf;                              \
            }                                                   \
        }                                                       \
                                                                \
        self->buf[self->size++] = (x);                          \
        S##Ptr_sentryset(self->buf + self->size);               \
    } while(0)
