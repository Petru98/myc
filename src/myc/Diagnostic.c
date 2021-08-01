#include "myc/Diagnostic.h"
#include <assert.h>


// const MycDiagInfo myc_diags[] = { // make member and modifiable
//     #define MYC_INTERNAL_DEFINE_DIAG(id, severity, msg) {msg, severity},
//     MYC_INTERNAL_DEFINE_DIAGS(MYC_INTERNAL_DEFINE_DIAG)
//     #undef MYC_INTERNAL_DEFINE_DIAG
// };


#define MYC_INTERNAL_DEFINE_DIAG(id, level, msg) {msg, level},
void MycDiagContext_init(MycDiagContext* ctx, void(*consumer)(MycDiagContext*, MycDiagnostic*, void*)) {
    (*ctx) = (MycDiagContext){consumer, 0, 0, {
        MYC_INTERNAL_DEFINE_DIAGS(MYC_INTERNAL_DEFINE_DIAG)
    }};
}
#undef MYC_INTERNAL_DEFINE_DIAG


void myc_handleDiagnostic(MycDiagContext* ctx, MycDiagnostic* diag, void* userdata) {
    const MycDiagLevel level = ctx->diaginfos[(size_t)diag->id].level;
    if(level != MYC_DIAGLVL_IGNORED) {
        if(level == MYC_DIAGLVL_WARNING) ctx->warncount++;
        else if(level == MYC_DIAGLVL_ERROR || level == MYC_DIAGLVL_FATAL) ctx->errcount++;

        assert(ctx->consumer);
        ctx->consumer(ctx, diag, userdata);
    }
}
