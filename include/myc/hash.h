#pragma once
#include <stdint.h>


#define MYC_SMALLSTR_HASH0(s, i)     ((uint64_t)((s)[i]) << ((8u * (i)) & (8u * sizeof(uint64_t) - 1)))
#define MYC_SMALLSTR_HASH1(s, i, x)  ((x) ^ ((i) < sizeof(s) - 1 ? MYC_SMALLSTR_HASH0(s, i) : 0))
#define MYC_SMALLSTR_HASH2(s, i, x)  MYC_SMALLSTR_HASH1( s, i, MYC_SMALLSTR_HASH1( s, (i)+1, x))
#define MYC_SMALLSTR_HASH4(s, i, x)  MYC_SMALLSTR_HASH2( s, i, MYC_SMALLSTR_HASH2( s, (i)+1, x))
#define MYC_SMALLSTR_HASH8(s, i, x)  MYC_SMALLSTR_HASH4( s, i, MYC_SMALLSTR_HASH4( s, (i)+1, x))
#define MYC_SMALLSTR_HASH16(s, i, x) MYC_SMALLSTR_HASH8( s, i, MYC_SMALLSTR_HASH8( s, (i)+1, x))
#define MYC_SMALLSTR_HASH32(s, i, x) MYC_SMALLSTR_HASH16(s, i, MYC_SMALLSTR_HASH16(s, (i)+1, x))
#define MYC_SMALLSTR_HASH(s)         MYC_SMALLSTR_HASH32(s, 0, 0)

static inline uint64_t myc_smallstr_hash(const char* s) {
    uint64_t h = 0;
    for(unsigned i = 0; s[i] != 0; i++)
        h ^= MYC_SMALLSTR_HASH0(s, i);
    return h;
}
