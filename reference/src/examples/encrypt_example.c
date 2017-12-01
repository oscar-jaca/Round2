/*
 * Copyright (c) 2017 Koninklijke Philips N.V. All rights reserved. A
 * copyright license for redistribution and use in source and binary
 * forms, with or without modification, is hereby granted for
 * non-commercial, experimental, research, public review and
 * evaluation purposes, provided that the following conditions are
 * met:
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution. If you wish to use this software commercially,
 *   kindly contact info.licensing@philips.com to obtain a commercial
 *   license.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * Example ENCRYPT application, shows the working of the algorithm.
 *
 * @author Hayo Baan
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>

#include "pst_api.h"
#include "cca_encrypt.h"
#include "misc.h"
#include "parameters.h"
#include "api_to_internal_parameters.h"
#include "randombytes.h"

/**
 * Mapping from each NIST security level to its key length and PQ security.
 */
static int sec_levels[][2] = {
    {128, 64},
    {256,86},
    {192,96},
    {384,128},
    {256,128}
};

/**
 * Prints the parameters on `stdout`.
 * @param[in] params the parameters
 * @param[in] fn     the variant for the generation of A
 */
static void print_parameters(const parameters *params, uint8_t fn) {
    printf("The chosen parameter set uses a ");
    if (params->d == params->n) {
        printf("ring structure.\n");
    } else {
        printf("non-ring structure.\n");
    }
    printf("d          = %u\n", (unsigned) params->d);
    printf("n          = %u\n", (unsigned) params->n);
    printf("k          = %u\n", (unsigned) params->k);
    printf("h          = %u\n", (unsigned) params->h);
    printf("q          = %u\n", (unsigned) params->q);
    printf("p_bits     = %u\n", (unsigned) params->p_bits);
    printf("t_bits     = %u\n", (unsigned) params->t_bits);
    printf("n̅ (n_bar)  = %u\n", (unsigned) params->n_bar);
    printf("m̅ (m_bar)  = %u\n", (unsigned) params->m_bar);
    printf("B          = %u\n", (unsigned) params->B);
    printf("ss_size    = %u\n", (unsigned) params->ss_size);
    printf("sk_size    = %u\n", (unsigned) params->sk_size);
    printf("pk_size    = %u\n", (unsigned) params->pk_size);
    printf("ct_size    = %u\n", (unsigned) params->ct_size);
    printf("fn         = %u\n", (unsigned) fn);
}

/**
 * Runs an example flow of the algorithm.
 *
 * @param[in] api_set_number the api set number ot use, -1 for the ones set by
 *                           the NIST API macro definitions
 * @param[in] fn             the variant ot use for the generation of A
 * @return __0__ in case of success
 */
static int example_run(int api_set_number, uint8_t fn) {
    parameters params;
    unsigned long long c_prime_prime_len, mlen;
    const char *message = "This is the message to be encrypted.";
    const unsigned long long message_len = strlen(message) + 1;
    /* Set up message containers */
    unsigned char *sk;
    unsigned char *pk;
    unsigned char *c_prime_prime;
    unsigned char *m;

    /* Set up parameters */
    if (api_set_number < 0) {
        if (set_parameters_from_api(&params)) {
            exit(1);
        }
        printf("Using API parameters:\n");
        /* Use params to allow api.h to actually specify KEM parameters, we will use the correct sizes anyway */
        printf("CRYPTO_SECRETKEYBYTES=%u\n", (unsigned) (params.sk_size + params.ss_size + params.pk_size));
        printf("CRYPTO_PUBLICKEYBYTES=%u\n", (unsigned) params.pk_size);
        printf("CRYPTO_BYTES         =%u\n", (unsigned) (params.ct_size + params.ss_size + 16 + 12));
        print_parameters(&params, fn);
    } else {
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
        printf("Using api set %d parameters:\n", api_set_number);
        printf("This set of parameters correspond to NIST security level %u.\n", (api_set_number % 5)+1);
        printf("%d bits of shared secret and %d bits of PQ security.\n", sec_levels[api_set_number % 5][0], sec_levels[api_set_number % 5][1]);
        print_parameters(&params, fn);
    }
    if (fn == 1 && params.n == 1) {
        unsigned char *seed = checked_malloc(params.ss_size);
        randombytes(seed, params.ss_size);
        print_hex("Generated A using seed", seed, params.ss_size, 1);
        create_A_fixed(seed, params.ss_size, &params);
        free(seed);
    }
    printf("\n");

    /* Set up message containers */
    sk = checked_malloc((size_t) (params.sk_size + params.ss_size + params.pk_size));
    pk = checked_malloc(params.pk_size);
    m = checked_malloc(message_len);
    c_prime_prime = checked_malloc((size_t) (params.ct_size + params.ss_size + 16 + 12) + (size_t) message_len);

    /* Initiator */
    printf("Initiator sets up key pair\n");
    crypto_encrypt_keypair_p(pk, sk, &params, fn);

    /* Initiator sends his pk */
    printf("Initiator sends his public key\n");

    /* Responder */
    printf("Responder encrypts message with public key and sends the cipher text\n");
    crypto_encrypt_p(c_prime_prime, &c_prime_prime_len, (const unsigned char *) message, message_len, pk, &params);

    /* Initiator */
    printf("Initiator decrypts cipher text with its secret key and determines the original message\n");
    crypto_encrypt_open_p(m, &mlen, c_prime_prime, c_prime_prime_len, sk, &params);

    printf("\n");
    printf("Comparing decrypted message with original: length=%s, message=%s\n", message_len != mlen ? "NOT OK" : "OK", memcmp(message, m, message_len) ? "NOT OK" : "OK");

    printf("\n");
    print_hex("Original Message ", (const unsigned char*) message, message_len, 1);
    print_hex("Decrypted Message", m, mlen, 1);

    free(sk);
    free(pk);
    free(c_prime_prime);
    free(m);

    return 0;
}

/**
 * Main program, runs an example algorithm flow on a set of parameters as
 * specified either by the NIST API macros definitions or from the specified
 * api parameter set (from `api_to_internal_parameters.h`). The variant for
 * the creation of matric A (fn) can also be specified.
 *
 * @param argc the number of command-line arguments (including the executable itself)
 * @param argv the command-line arguments
 * @return __0__ in case of success
 */
int main(int argc, char **argv) {
    long number;
    int ch;
    const long max_api_set_number = (long) (sizeof (api_to_internal_parameters) / sizeof (api_to_internal_parameters[0]));
    int api_set_number = 0;
    uint8_t fn = ROUND2_VARIANT_A;

    while ((ch = getopt(argc, argv, "a:f:")) != -1) {
        switch (ch) {
            case 'a':
                number = strtol(optarg, NULL, 10);
                if (number < 0 || number >= max_api_set_number) {
                    fprintf(stderr, "%s Invalid api set number specified: %s, must be less than %lu\n", argv[0], optarg, max_api_set_number);
                    exit(EXIT_FAILURE);
                }
                api_set_number = (int) number;
                break;
            case 'f':
                number = strtol(optarg, NULL, 10);
                if (number < 0 || number > 2) {
                    fprintf(stderr, "%s Invalid fn specified: %s, must be 0, 1, or 2\n", argv[0], optarg);
                    exit(EXIT_FAILURE);
                }
                fn = (uint8_t) number;
                break;
            default:
                fprintf(stderr, "%s: unknown option %s\n", argv[0], optarg);
                    exit(EXIT_FAILURE);
        }
    }
    argc -= optind;
    argv += optind;
    if (argc > 0) {
        fprintf(stderr, "Usage: %s [-a N] [-f N]\n", argv[0]);
                    exit(EXIT_FAILURE);
    }

    example_run(api_set_number, fn);
    return 0;
}
