/*
 * Copyright (c) 2017 Koninklijke Philips N.V. All rights reserved. A copyright
 * license for redistribution and use in source and binary forms, with or
 * without modification, is hereby granted for non-commercial, experimental and
 * research purposes, provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution. If you wish to use
 *   this software commercially, kindly contact info.licensing@philips.com to
 *   obtain a commercial license.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * Application to generate an A_fixed matrix using the parameters from the 
 * API parameter set as specified on the command-line. The output of the
 * application can be used to set up the fixed A  matrix in the file
 * `a_fixed.h` e.g. when using the NIST API versions of the algorithm
 * interface.
 *
 * @author Hayo Baan
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include "api_to_internal_parameters.h"
#include "parameters.h"
#include "randombytes.h"
#include "drng.h"
#include "misc.h"

/**
 * Outputs the definition of a fixed A matrix based on the API parameter set as
 * specified on the command-line.
 * 
 * @param argc the number of command-line arguments (including the executable itself)
 * @param argv the command-line arguments
 * @return __0__ in case of success
 */
int main(int argc, char **argv) {
    parameters params;
    unsigned char *seed;
    size_t i, j;
    uint16_t *A_fixed;
    size_t len_a_fixed;
    uint16_t mask_ceil_log2q;

    unsigned long api_set_number = 0;

    if (argc > 1) {
        api_set_number = (unsigned long) strtol(argv[1], NULL, 10);
    }

    if (api_set_number > sizeof (api_to_internal_parameters) / sizeof (api_to_internal_parameters[0])) {
        fprintf(stderr, "%s: invalid api set number specified (%s), must be less than %lu\n", argv[0], argv[1], sizeof (api_to_internal_parameters) / sizeof (api_to_internal_parameters[0]));
        exit(EXIT_FAILURE);
    }

    if (api_to_internal_parameters[api_set_number][POS_N] != 1) {
        fprintf(stderr, "%s: invalid api set number specified (%s), must be a Non-Ring parameter set\n", argv[0], argv[1]);
        exit(EXIT_FAILURE);
    }

    /* Set up */
    set_parameters(&params,
            (uint8_t) api_to_internal_parameters[api_set_number][POS_SS],
            api_to_internal_parameters[api_set_number][POS_D],
            api_to_internal_parameters[api_set_number][POS_N],
            api_to_internal_parameters[api_set_number][POS_H],
            api_to_internal_parameters[api_set_number][POS_Q],
            (uint8_t) api_to_internal_parameters[api_set_number][POS_P_BITS],
            (uint8_t) api_to_internal_parameters[api_set_number][POS_T_BITS],
            api_to_internal_parameters[api_set_number][POS_N_BAR],
            api_to_internal_parameters[api_set_number][POS_M_BAR],
            (uint8_t) api_to_internal_parameters[api_set_number][POS_B]);
    mask_ceil_log2q = (uint16_t) ((1U << ceil_log2(params.q)) - 1);
    len_a_fixed = (size_t) (params.d * params.d);
    A_fixed = malloc(len_a_fixed * sizeof (*A_fixed));

    /* Initialise drng */
    seed = checked_malloc(params.ss_size);
    randombytes(seed, params.ss_size);
    init_drng(seed, params.ss_size);

    /* Generate random A_fixed */
    for (i = 0; i < len_a_fixed; ++i) {
        do {
            drng((unsigned char *) &A_fixed[i], sizeof (*A_fixed));
            A_fixed[i] &= mask_ceil_log2q;
        } while (A_fixed[i] >= params.q);
    }

    /* Print A_fixed */
    printf("/* Seed used for the generation of A_fixed: ");
    print_hex(NULL, seed, params.ss_size, 1);
    printf(" */\n");
    printf("static uint16_t *A_fixed = (uint16_t[%u]){\n", params.d * params.d);
    for (i = 0; i < params.d; ++i) {
        if (i > 0) {
            printf(",\n");
        }
        printf("    ");
        for (j = 0; j < params.d; ++j) {
            if (j > 0) {
                if (j % 16 == 0) {
                    printf(",\n    ");
                } else {
                    printf(", ");
                }
            }
            printf("%hu", A_fixed[i * params.d + j]);
        }
    }
    printf("\n    };\n");

    return 0;
}
