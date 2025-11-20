#ifndef LINK_SIM_H
#define LINK_SIM_H

#include <stdio.h>

typedef enum {
    LINK_STATE_RESET = 0,
    LINK_STATE_TRAINING,
    LINK_STATE_LOCKED,
    LINK_STATE_ERROR
} link_state_t;

typedef struct {
    int tx_power; /*0..100*/
    int eq_level; /*0..100*/
    double ber; /*rate of errors*/
} link_params_t;

typedef struct {
    int step;
    link_state_t state;
    link_params_t params;

} link_sample_t;


typedef struct {
    link_state_t state;
    link_params_t params;
    int max_steps;
    int current_step;
    double target_ber;
    double noise_level;
} link_context_t;

int link_init(link_context_t *ctx, int max_steps, double target_ber, double noise_level);
int link_step(link_context_t *ctx, link_sample_t *out_sample);
int link_run_training(link_context_t *ctx, const char *log_path);

#endif