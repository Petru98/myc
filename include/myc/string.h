#pragma once
#include "util.h"
#include <stdlib.h>
#include <string.h>


typedef struct String {
    size_t capacity;
    union {
        struct {
            uint8_t size;
            char buf[sizeof(size_t) + sizeof(char*) - sizeof(uint8_t)];
        } s;
        struct {
            size_t size;
            char* ptr;
        } b;
    };
} String;


void String_init(String* self);
void String_free(String* self);

char* String_data(String* self);
size_t String_size(String* self);

void String_reserve(String* self, size_t size);
void String_resize(String* self, size_t size);
void String_fit(String* self);

void String_pushBack(String* self, char ch);
char String_popBack(String* self);

int String_cmp(String* self, String* other);





typedef struct StrHdr {
    size_t capacity;
    size_t size;
    char buf[];
} StrHdr;
typedef char* StrPtr;


void StrPtr_free(StrPtr str);

size_t StrPtr_size(StrPtr str);
size_t StrPtr_capacity(StrPtr str);

StrPtr StrPtr_reserve(StrPtr str, size_t size);
StrPtr StrPtr_resize(StrPtr str, size_t size);
StrPtr StrPtr_fit(StrPtr str);

StrPtr StrPtr_pushBack(StrPtr str, char ch);
char StrPtr_popBack(StrPtr str);

int StrPtr_cmp(StrPtr s1, StrPtr s2);
