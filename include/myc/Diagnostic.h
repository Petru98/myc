#pragma once
#include "SourceFile.h"


EXTERNC_BEG

enum {
    MYC_DIAGLVL_IGNORED,
    MYC_DIAGLVL_NOTE,
    MYC_DIAGLVL_REMARK,
    MYC_DIAGLVL_WARNING,
    MYC_DIAGLVL_ERROR,
    MYC_DIAGLVL_FATAL,

    MYC_DIAGLVL_COUNT
};
typedef uint8_t MycDiagLevel;
_Static_assert((MycDiagLevel)UINTMAX_MAX >= MYC_DIAGLVL_COUNT, "integer type too small for enum");



#define MYC_INTERNAL_DEFINE_DIAGS(DEFINE) \
    DEFINE(MYC_DIAG_UNEXPECTED_EOF            , MYC_DIAGLVL_FATAL, "unexpected end of file") \
    DEFINE(MYC_DIAG_NO_DELIM_BETWEEN_NUM_ID   , MYC_DIAGLVL_ERROR, "delimiter expected between number and identifier") \
    DEFINE(MYC_DIAG_CHAR_LIT_WRONG_CHARS_COUNT, MYC_DIAGLVL_ERROR, "char literal must have exactly one character") \
    DEFINE(MYC_DIAG_INVALID_HEX_DIGIT         , MYC_DIAGLVL_ERROR, "unknown hex digit") \
    DEFINE(MYC_DIAG_INVALID_OCT_DIGIT         , MYC_DIAGLVL_ERROR, "unknown octal digit") \
    DEFINE(MYC_DIAG_UNKNOWN_ESCAPE_CHAR       , MYC_DIAGLVL_ERROR, "unknown escape character")

enum {
    #define MYC_INTERNAL_DEFINE_DIAG(id, level, msg) id,
    MYC_INTERNAL_DEFINE_DIAGS(MYC_INTERNAL_DEFINE_DIAG)
    #undef MYC_INTERNAL_DEFINE_DIAG

    MYC_DIAG_COUNT
};
typedef uint16_t MycDiagId;
_Static_assert((MycDiagId)UINTMAX_MAX >= MYC_DIAG_COUNT, "integer type too small for enum");


typedef struct MycDiagInfo {
    const char* msg;
    // MycDiagSeverity severity;
    MycDiagLevel level;
} MycDiagInfo;

// extern const MycDiagInfo myc_diags[MYC_DIAG_COUNT];



typedef struct MycDiagnostic {
    MycSourceFile* source;
    MycSourcePosition cur;
    MycDiagId id;
} MycDiagnostic;


typedef struct MycDiagContext {
    void(*consumer)(struct MycDiagContext*, MycDiagnostic*, void*);
    myc_size_t errcount;
    myc_size_t warncount;
    MycDiagInfo diaginfos[MYC_DIAG_COUNT];
} MycDiagContext;

void MycDiagContext_init(MycDiagContext* ctx, void(*consumer)(MycDiagContext*, MycDiagnostic*, void*));

void myc_handleDiagnostic(MycDiagContext* ctx, MycDiagnostic* diag, void*);

EXTERNC_END
