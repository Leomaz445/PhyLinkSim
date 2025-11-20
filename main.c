#include <stdio.h>
#include "link_sim.h"
#include "waveform.h"

int main(void) {
    link_context_t ctx;
    if (link_init(&ctx, 50, 1e-5, 1e-5) != 0) {
        fprintf(stderr, "failed to init link context\n");
        return 1;
    }
    if (link_run_training(&ctx, "link_log.csv") != 0) {
        fprintf(stderr, "failed to run training\n");
        return 1;
    }

    if (ctx.state == LINK_STATE_LOCKED) {
        printf("Link locked after %d steps, final BER = %g\n",
               ctx.current_step, ctx.params.ber);

        const char *bits = "01011011001100110101";
        if (generate_link_waveform(&ctx, bits, 30.0, 32,
                                   "link_waveform.csv") != 0) {
            fprintf(stderr, "failed to generate waveform\n");
            return 1;
                                   }
        printf("Waveform written to link_waveform.csv\n");

    } else {
        printf("Link failed, final state = %d, BER = %g\n",
               ctx.state, ctx.params.ber);
    }

    return 0;
}