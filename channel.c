#include <math.h>
#include <stdio.h>
#include "channel.h"

static double db_to_mag(double db) {
    return pow(10.0, db / 20.0);
}

static double clamp_positive(double v) {
    return (v > 0.0) ? v : 0.0;
}

int generate_channel_transfer(const channel_profile_t *profile,
                              double f_start_ghz,
                              double f_stop_ghz,
                              double step_ghz,
                              const char *csv_path) {
    if (!profile || !csv_path || f_start_ghz <= 0.0 || f_stop_ghz <= 0.0 ||
        step_ghz <= 0.0 || f_stop_ghz < f_start_ghz) {
        return -1;
    }

    FILE *f = fopen(csv_path, "w");
    if (!f) {
        return -1;
    }

    fprintf(f, "freq_ghz,thru_mag,next_mag,fext_mag,combined_mag\n");

    for (double freq = f_start_ghz; freq <= f_stop_ghz + 1e-12; freq += step_ghz) {
        double freq_ratio = freq;

        /* Through channel insertion loss scales roughly with sqrt(f). */
        double il_db = profile->length_m * profile->att_db_per_m * sqrt(freq_ratio);
        double thru_mag = db_to_mag(-il_db);

        /* Crosstalk grows more quickly with frequency; model mild slopes. */
        double next_db = profile->length_m * profile->next_db_per_m * pow(freq_ratio, 0.3);
        double fext_db = profile->length_m * profile->fext_db_per_m * pow(freq_ratio, 0.6);

        double next_mag = db_to_mag(-next_db);
        double fext_mag = db_to_mag(-fext_db);

        double combined_mag = sqrt(clamp_positive(thru_mag * thru_mag +
                                                 next_mag * next_mag +
                                                 fext_mag * fext_mag));

        fprintf(f, "%.3f,%.6e,%.6e,%.6e,%.6e\n",
                freq, thru_mag, next_mag, fext_mag, combined_mag);
    }

    fclose(f);
    return 0;
}
