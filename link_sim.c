#include <stdio.h>
#include <stdlib.h>
#include "link_sim.h"

static double rand_unit(void) {
    return (double)rand() / (double)RAND_MAX;
}

static double model_ber(int tx_power,int eq_level,double noise_level) {
    double p_center = 60.0;
    double eq_center = 50.0;

    double dp = (tx_power - p_center) / 40.0;
    double deq = (eq_level - eq_center) / 40.0;

    double dist2 = dp * dp + deq * deq;
    double noise = (rand_unit() - 0.5) * noise_level;
    double base = 1e-6 + dist2 * 5e-4+noise;

    if (base < 1e-8) {
        base = 1e-8;
    }
    return base;

}

int link_init(link_context_t *ctx, int max_steps, double target_ber, double noise_level) {
    if (!ctx) return -1;
    ctx->state = LINK_STATE_RESET;
    ctx->params.tx_power = 50;
    ctx->params.eq_level = 40;
    ctx->params.ber = 1.0;
    ctx->max_steps = max_steps;
    ctx->current_step = 0;
    ctx->target_ber = target_ber;
    ctx->noise_level = noise_level;
    return 0;

}

int link_step(link_context_t *ctx, link_sample_t *out_sample) {
    if (!ctx || !out_sample) {
        return -1;
    }
    if (ctx->state == LINK_STATE_LOCKED || ctx->state == LINK_STATE_ERROR) {
        out_sample->step = ctx->current_step;
        out_sample->state = ctx->state;
        out_sample->params = ctx->params;
        return 1;
    }

    if (ctx->state == LINK_STATE_RESET) {
        ctx->state = LINK_STATE_TRAINING;
    }

    ctx->params.ber = model_ber(ctx->params.tx_power,
                               ctx->params.eq_level,
                               ctx->noise_level);
    printf("%lf\n",ctx->params.ber);
    if (ctx->state == LINK_STATE_TRAINING) {
        if (ctx->params.ber <= ctx->target_ber) {
            ctx->state = LINK_STATE_LOCKED;
        } else if (ctx->current_step >= ctx->max_steps) {
            ctx->state = LINK_STATE_ERROR;
        } else {
            if (ctx->params.tx_power < 60) {
                ctx->params.tx_power += 1;
            } else if (ctx->params.tx_power > 60) {
                ctx->params.tx_power -= 1;
            }

            if (ctx->params.eq_level < 50) {
                ctx->params.eq_level += 1;
            } else if (ctx->params.eq_level > 50) {
                ctx->params.eq_level -= 1;
            }
        }
    }
    out_sample->step = ctx->current_step;
    out_sample->state = ctx->state;
    out_sample->params = ctx->params;
    ctx->current_step++;

    if (ctx->state == LINK_STATE_LOCKED || ctx->state == LINK_STATE_ERROR) {
        return 1;
    }

    return 0;



}

int link_run_training(link_context_t *ctx, const char *log_path) {
    if (!ctx || !log_path) {
        return -1;
    }

    FILE *f = fopen(log_path, "w");
    if (!f) {
        perror("fopen log file");
        return -1;
    }


    fprintf(f, "step,state,tx_power,eq_level,ber\n");

    link_sample_t sample;
    int rc;

    while (1) {
        rc = link_step(ctx, &sample);
        if (rc < 0) {
            fclose(f);
            return -1;
        }

        fprintf(f, "%d,%d,%d,%d,%g\n",
                sample.step,
                sample.state,
                sample.params.tx_power,
                sample.params.eq_level,
                sample.params.ber);

        if (rc > 0) {
            break;
        }

        if (ctx->current_step > ctx->max_steps + 10) {
            fprintf(stderr, "safety break: too many steps\n");
            break;
        }
    }

    fclose(f);
    return 0;
}