#include "myc/util.h"


size_t myc_bsearchin(const void* key, const void* ptr, size_t count, size_t size, int (*comp)(const void*, const void*)) {
    size_t beg = 0;
    size_t end = count;
    while(beg < end)
    {
        const size_t i = (beg + end) / 2;
        const void* const iptr = (const void*)((const char*)ptr + (i * size));
        const int cmpres = (*comp)(key, iptr);
        if(cmpres < 0)
            end = i;
        else if(cmpres > 0)
            beg = i + 1;
        else
            return beg;
    }
    return beg;
}
size_t myc_upboundin(const void* key, const void* ptr, size_t count, size_t size, int (*comp)(const void*, const void*)) {
    size_t beg = 0;
    size_t end = count;
    while(beg < end)
    {
        const size_t i = (beg + end) / 2;
        const void* const iptr = (const void*)((const char*)ptr + (i * size));
        const int cmpres = (*comp)(key, iptr);
        if(cmpres < 0)
            end = i;
        else if(cmpres > 0)
            beg = i + 1;
        else
            return beg + 1;
    }
    return beg;
}
size_t myc_loboundin(const void* key, const void* ptr, size_t count, size_t size, int (*comp)(const void*, const void*)) {
    size_t beg = 0;
    size_t end = count;
    while(beg < end)
    {
        const size_t i = (beg + end) / 2;
        const void* const iptr = (const void*)((const char*)ptr + (i * size));
        const int cmpres = (*comp)(key, iptr);
        if(cmpres < 0)
            end = i;
        else if(cmpres > 0)
            beg = i + 1;
        else
        {
            while((*comp)(key, (const void*)((const char*)ptr + ((beg - 1) * size))) == 0)
                beg--;
            return beg;
        }
    }
    return beg;
}


const char* const myc_errmsg[] = {
    #define MYC_INTERNAL_DEFINE_ERROR(no, msg) msg,
    MYC_INTERNAL_DEFINE_ERRORS(MYC_INTERNAL_DEFINE_ERROR)
    #undef MYC_INTERNAL_DEFINE_ERROR
};
