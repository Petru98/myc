#pragma once
#include "Token.h"
#include "Diagnostic.h"


EXTERNC_BEG

typedef struct MycLexerContext {
    MycSourceFile* src;
    MycDiagContext* diagctx;

    MycToken tok;
} MycLexerContext;

void MycLexerContext_init(MycLexerContext* ctx, MycSourceFile* src, MycDiagContext* diagctx);
int myc_tokenizeNext(MycLexerContext* ctx);

EXTERNC_END
