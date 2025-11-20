#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "waveform.h"

int generate_link_waveform(const link_context_t *ctx,
                           const char *bit_str,
                           double bit_rate_gbps,
                           int samples_per_ui,
                           const char *csv_path) {


    int n_bits = (int)strlen(bit_str);
    if (n_bits == 0 || samples_per_ui <= 0 || bit_rate_gbps <= 0.0) {
        return -1;
    }
    double ui = 1.0 / (bit_rate_gbps * 1e9);
    double dt = ui / samples_per_ui;

    double swing = ctx->params.tx_power / 100.0;
    if (swing <= 0.0) {
        swing = 1.0;
    }
    FILE *f = fopen(csv_path, "w");
    if (!f) {
        return -1;
    }

    fprintf(f, "t,voltage\n");

    double t = 0.0;
    for (int i = 0; i < n_bits; ++i) {
        char b = bit_str[i];
        double level;

        if (b == '0') {
            level = -swing;
        } else if (b == '1') {
            level = swing;
        } else {
            fclose(f);
            return -1;
        }

        for (int s = 0; s < samples_per_ui; ++s) {
            fprintf(f, "%.12e,%.6f\n", t, level);
            t += dt;
        }
    }

    fclose(f);
    return 0;
}