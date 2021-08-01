#include "myc/lexer.h"
#include "myc/Diagnostic.h"
#include "myc/SourceFile.h"
#include "myc/string.h"
#include "myc/Token.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>


static inline int digitHexToInt(char ch) {
    if(ch >= '0' && ch <= '9')
        return ch - '0';
    if(ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if(ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return -1;
}
static inline int digitDecToInt(char ch) {
    if(ch >= '0' && ch <= '9')
        return ch - '0';
    return -1;
}
static inline int digitOctToInt(char ch) {
    if(ch >= '0' && ch <= '7')
        return ch - '0';
    return -1;
}
static inline int digitBinToInt(char ch) {
    if(ch == '0' && ch == '1')
        return ch - '0';
    return -1;
}

static inline bool isIdentifierCharBeg(char ch) {
    return ch == '_' || isalpha(ch);
}
static inline bool isIdentifierChar(char ch) {
    return ch == '_' || isalnum(ch);
}



static inline char peek(MycLexerContext* ctx) {
    const int ch = MycSourceFile_peek(ctx->src);
    assert(ch >= 0);
    return (char)ch;
}
static inline char getc(MycLexerContext* ctx) {
    const int ch = MycSourceFile_getc(ctx->src);
    assert(ch >= 0);
    return (char)ch;
}
static inline int peeki(MycLexerContext* ctx) {
    return MycSourceFile_peek(ctx->src);
}
static inline int getci(MycLexerContext* ctx) {
    return MycSourceFile_getc(ctx->src);
}


#define REQUIRE(count) \
    do {                                                    \
        const int r = MycSourceFile_reqbuf(ctx->src, count);   \
        if(r <= 0) {                                        \
            if(r == 0)                                      \
                handleUnexpectedEof(ctx);                   \
            return r;                                       \
        }                                                   \
    } while(0)

#define SKIP(count) \
    do {                                                \
        const int r = MycSourceFile_skip(ctx->src, count); \
        assert(r != 0);                                 \
        if(r <= 0)                                      \
            return r;                                   \
    } while(0)

#define SKIPONE() \
    do {                                        \
        const int r = MycSourceFile_getc(ctx->src); \
        assert(r != 0);                         \
        if(r <= 0)                              \
            return r;                           \
    } while(0)


static inline void handleDiag(MycLexerContext* ctx, MycSourcePosition cur, MycDiagId id, void* userdata) {
    myc_handleDiagnostic(ctx->diagctx, &(MycDiagnostic){
        .source=ctx->src,
        .cur=cur,
        .id=id
    }, userdata);
}
static inline void handleDiagCur(MycLexerContext* ctx, MycDiagId id, void* userdata) {
    handleDiag(ctx, ctx->src->pos, id, userdata);
}
static inline void handleDiagBeg(MycLexerContext* ctx, MycDiagId id, void* userdata) {
    handleDiag(ctx, ctx->tok.beg, id, userdata);
}
static inline void handleUnexpectedEof(MycLexerContext* ctx) {
    handleDiag(ctx, ctx->tok.beg, MYC_DIAG_UNEXPECTED_EOF, NULL);
}


static MycTokenId getIdentifierType(const char* ptr, myc_size_t size) {
    #define CMP(str, id) if(memcmp(ptr, str, sizeof(str) - 1) == 0) return id
    #define LEN(n) case n: switch(ptr[0])

    switch(size) {
    LEN(2) {
        case 'd': CMP("do", MYC_TOK_KWDO); break;
        case 'i': CMP("if", MYC_TOK_KWIF); break;
    } break;
    LEN(3) {
        case 'f': CMP("for", MYC_TOK_KWFOR); break;
        case 'i': CMP("int", MYC_TOK_KWINT); break;
    } break;
    LEN(4) {
        case 'a': CMP("auto", MYC_TOK_KWAUTO); break;
        case 'c': CMP("case", MYC_TOK_KWCASE);
                  CMP("char", MYC_TOK_KWCHAR); break;
        case 'e': CMP("else", MYC_TOK_KWELSE);
                  CMP("enum", MYC_TOK_KWENUM); break;
        case 'l': CMP("long", MYC_TOK_KWLONG); break;
        case 'g': CMP("goto", MYC_TOK_KWGOTO); break;
        case 'v': CMP("void", MYC_TOK_KWVOID); break;
    } break;
    LEN(5) {
        case 'b': CMP("break", MYC_TOK_KWBREAK); break;
        case 'c': CMP("const", MYC_TOK_KWCONST); break;
        case 'f': CMP("float", MYC_TOK_KWFLOAT); break;
        case 's': CMP("short", MYC_TOK_KWSHORT); break;
        case 'u': CMP("union", MYC_TOK_KWUNION); break;
        case 'w': CMP("while", MYC_TOK_KWWHILE); break;
        case '_': CMP("_Bool", MYC_TOK_KW_BOOL); break;
    } break;
    LEN(6) {
        case 'd': CMP("double", MYC_TOK_KWDOUBLE); break;
        case 'e': CMP("extern", MYC_TOK_KWEXTERN); break;
        case 'i': CMP("inline", MYC_TOK_KWINLINE); break;
        case 'r': CMP("return", MYC_TOK_KWRETURN); break;
        case 's': CMP("signed", MYC_TOK_KWSIGNED);
                  CMP("sizeof", MYC_TOK_KWSIZEOF);
                  CMP("static", MYC_TOK_KWSTATIC);
                  CMP("struct", MYC_TOK_KWSTRUCT);
                  CMP("switch", MYC_TOK_KWSWITCH); break;
    } break;
    LEN(7) {
        case 'd': CMP("default", MYC_TOK_KWDEFAULT); break;
        case 't': CMP("typedef", MYC_TOK_KWTYPEDEF); break;
        case '_': CMP("_Atomic", MYC_TOK_KW_ATOMIC); break;
    } break;
    LEN(8) {
        case 'c': CMP("continue", MYC_TOK_KWCONTINUE); break;
        case 'r': CMP("register", MYC_TOK_KWREGISTER);
                  CMP("restrict", MYC_TOK_KWRESTRICT); break;
        case 'u': CMP("unsigned", MYC_TOK_KWUNSIGNED); break;
        case 'v': CMP("volatile", MYC_TOK_KWVOLATILE); break;
        case '_': CMP("_Alignas", MYC_TOK_KW_ALIGNAS);
                  CMP("_Alignof", MYC_TOK_KW_ALIGNOF);
                  CMP("_Complex", MYC_TOK_KW_COMPLEX);
                  CMP("_Generic", MYC_TOK_KW_GENERIC); break;
    } break;
    LEN(9) {
        case '_': CMP("_Noreturn", MYC_TOK_KW_NORETURN); break;
    } break;
    LEN(10) {
        case '_': CMP("_Imaginary", MYC_TOK_KW_IMAGINARY); break;
    } break;
    LEN(13) {
        case '_': CMP("_Thread_local", MYC_TOK_KW_THREAD_LOCAL); break;
    } break;
    LEN(14) {
        case '_': CMP("_Static_assert", MYC_TOK_KW_STATIC_ASSERT); break;
    } break;
    }
    #undef CMP
    #undef LEN

    return MYC_TOK_IDENTIFIER;
}

static int getEscapedChar(MycLexerContext* ctx) {
    REQUIRE(1);
    int ch = (unsigned char)peek(ctx);

    if(ch == 'x' || ch == 'X') {
        SKIPONE();
        REQUIRE(2);

        int d[2];
        ch = 0;

        for(size_t i = 0; i < 2; i++) {
            d[i] = digitHexToInt(peek(ctx));
            if(d[i] == -1) {
                ch = -1;
                handleDiagCur(ctx, MYC_DIAG_INVALID_HEX_DIGIT, NULL);
            }
            SKIPONE();
        }

        if(ch != -1) {
            ch = d[0]*16 + d[1];
        }
    }
    else if(ch >= '0' && ch <= '7') {
        REQUIRE(3);

        int d[3];
        ch = 0;

        for(size_t i = 0; i < 3; i++) {
            d[i] = digitOctToInt(peek(ctx));
            if(d[i] == -1) {
                ch = -1;
                handleDiagCur(ctx, MYC_DIAG_INVALID_OCT_DIGIT, NULL);
            }
            SKIPONE();
        }

        if(ch != -1) {
            ch = d[0]*64 + d[1]*8 + d[2];
        }
    }
    else {
        switch(ch)
        {
        case '\'': ch = '\''; break;
        case '\"': ch = '\"'; break;
        case '\\': ch = '\\'; break;
        case 'a' : ch = '\a'; break;
        case 'b' : ch = '\b'; break;
        case 'f' : ch = '\f'; break;
        case 'n' : ch = '\n'; break;
        case 'r' : ch = '\r'; break;
        case 't' : ch = '\t'; break;
        case 'v' : ch = '\v'; break;
        default:
            ch = -1;
            handleDiagCur(ctx, MYC_DIAG_UNKNOWN_ESCAPE_CHAR, NULL);
        }
        SKIPONE();
    }

    return ch;
}



static inline int tokenizeSingleCharacter(MycLexerContext* ctx) {
    ctx->tok.id = (MycTokenId)getc(ctx);
    return 1;
}


static inline int tokenizeIdentifier(MycLexerContext* ctx) {
    if(!isIdentifierCharBeg(peek(ctx)))
        return 0;

    String_init(&ctx->tok.str);
    int ch = peeki(ctx);

    while(ch >= 0 && isIdentifierChar((char)ch)) {
        String_pushBack(&ctx->tok.str, (char)ch);
        getc(ctx);
        ch = peeki(ctx);
    }

    if(ch == -1 && errno != 0) {
        String_free(&ctx->tok.str);
        return -1;
    }

    ctx->tok.id = getIdentifierType(String_data(&ctx->tok.str), String_size(&ctx->tok.str));
    return 1;
}


static inline int tokenizeCharLiteral(MycLexerContext* ctx) {
    if(peek(ctx) != '\'')
        return 0;

    ctx->tok.id = MYC_TOK_LITCHAR;
    REQUIRE(3);
    SKIPONE();
    int ret = 1;

    switch(peek(ctx)) {
    case '\'': {
        ctx->tok.charlit = 0;
        handleDiagBeg(ctx, MYC_DIAG_CHAR_LIT_WRONG_CHARS_COUNT, NULL);
        ret = -1;
        break;
    }
    case '\\': {
        SKIPONE();
        const int ch = getEscapedChar(ctx);
        if(ch == -1)
            ret = -1;
        ctx->tok.charlit = (char)ch;
        REQUIRE(1);
        break;
    }
    default: {
        ctx->tok.charlit = peek(ctx);
        SKIPONE();
    }}

    if(peek(ctx) == '\'') {
        SKIPONE();
    }
    else {
        int ch = getci(ctx);
        while(ch >= 0 && ch != '\'')
            ch = getci(ctx);

        if(ch < 0 && errno == 0)
            handleUnexpectedEof(ctx);
        else
            handleDiagBeg(ctx, MYC_DIAG_CHAR_LIT_WRONG_CHARS_COUNT, NULL);
        ret = -1;
    }

    return ret;
}


static inline int tokenizeString(MycLexerContext* ctx) {
    if(peek(ctx) != '\"')
        return 0;

    ctx->tok.id = MYC_TOK_LITSTRING;
    String_init(&ctx->tok.str);
    int ret = 1;

    SKIPONE();
    int ch = peeki(ctx);

    while(ch >= 0 && ch != '\"') {
        if(ch == '\\') {
            SKIPONE();
            ch = getEscapedChar(ctx);
            if(ch == -1)
                ret = -1;
        }
        String_pushBack(&ctx->tok.str, (char)ch);
        getc(ctx);
        ch = peeki(ctx);
    }

    if(ch < 0) {
        String_free(&ctx->tok.str);
        if(errno == 0)
            handleUnexpectedEof(ctx);
        return -1;
    }

    SKIPONE();
    return ret;
}



void MycLexerContext_init(MycLexerContext* ctx, MycSourceFile* src, MycDiagContext* diagctx) {
    ctx->src = src;
    ctx->diagctx = diagctx;
}


int myc_tokenizeNext(MycLexerContext* ctx) {
    int ch = peeki(ctx);
    while(ch >= 0 && isspace(ch)) {  /* TODO maybe take spaces into account somewhere */
        getc(ctx);
        ch = peeki(ctx);
    }

    if(ch < 0) {
        if(errno != 0)
            return -1;
        ctx->tok.id = MYC_TOK_EOF;
        ctx->tok.beg = ctx->src->pos;
        ctx->tok.end = ctx->src->pos;
        return 0;
    }

    int (*const tokenizers[])(MycLexerContext*) = {
        tokenizeIdentifier,
        tokenizeCharLiteral,
        tokenizeString
    };

    ctx->tok.beg = ctx->src->pos;
    for(unsigned i = 0; i < sizeof(tokenizers)/sizeof(tokenizers[0]); i++) {
        const int r = (*tokenizers[i])(ctx);
        if(r != 0) {
            ctx->tok.end = ctx->src->pos;
            return r;
        }
    }

    const int r = tokenizeSingleCharacter(ctx);
    assert(r != 0);
    ctx->tok.end = ctx->src->pos;
    return r;
}
