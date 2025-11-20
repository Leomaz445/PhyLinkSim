#ifndef WAVEFORM_H
#define WAVEFORM_H

#include "link_sim.h"

int generate_link_waveform(const link_context_t *ctx,
                           const char *bit_str,
                           double bit_rate_gbps,
                           int samples_per_ui,
                           const char *csv_path);

#endif