#include "argp.h"
#include "myc/Diagnostic.h"
#include "myc/SourceFile.h"
#include "myc/string.h"
#include "myc/Token.h"
#include "myc/lexer.h"
#include <asm-generic/errno-base.h>
#include <stdio.h>


void handleDiag(MycDiagContext* ctx, MycDiagnostic* diag, void* userdata) {
    (void)userdata;
    fprintf(stderr, "%s:%u:%u: %s\n", diag->source->name, diag->cur.line, diag->cur.col, ctx->diaginfos[diag->id].msg);
}


int main(int argc, char** argv)
{
    (void)argc, (void)argv;
    int exit_code = 0;

    MycDiagContext diagctx;
    MycDiagContext_init(&diagctx, &handleDiag);

    MycSourceFile src;
    MycSourceFile_init(&src);
    if(!MycSourceFile_open(&src, "src/myc/util.c")) {
        printf("%d\n", errno);
        return 1;
    }

    MycLexerContext lexctx;
    MycLexerContext_init(&lexctx, &src, &diagctx);

    while(myc_tokenizeNext(&lexctx) > 0) {
        switch(lexctx.tok.id) {
        case MYC_TOK_LITCHAR:
            printf("'%c'\t", lexctx.tok.charlit);
            break;
        case MYC_TOK_LITSTRING:
            printf("\"%s\"\t", String_data(&lexctx.tok.str));
            break;
        case MYC_TOK_IDENTIFIER:
            printf("%s\t", String_data(&lexctx.tok.str));
            break;
        default:
            if(lexctx.tok.id >= 0)
                printf("%c\t", lexctx.tok.id);
        }
        printf("%d\n", lexctx.tok.id);
    }

    return exit_code;
}
