#ifndef CHANNEL_H
#define CHANNEL_H

#include <stddef.h>

/* Simple channel profile describing attenuation and crosstalk slopes. */
typedef struct {
    double length_m;         /* physical length of the channel */
    double att_db_per_m;     /* insertion loss per meter at 1 GHz */
    double next_db_per_m;    /* near-end crosstalk per meter at 1 GHz */
    double fext_db_per_m;    /* far-end crosstalk per meter at 1 GHz */
} channel_profile_t;

int generate_channel_transfer(const channel_profile_t *profile,
                              double f_start_ghz,
                              double f_stop_ghz,
                              double step_ghz,
                              const char *csv_path);

#endif
