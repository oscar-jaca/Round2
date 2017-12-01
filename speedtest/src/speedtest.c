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

/** @cond DEVELOP
 * @file
 * Speed tests for the Round2 implementation.
 *
 * @author Hayo Baan
 * @endcond
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include "pst_api.h"
#include "cpa_kem.h"
#include "cca_encrypt.h"
#include "parameters.h"
#include "test_utils.h"
#include "misc.h"

/**
 * Runs the speed tests for the individual steps of the KEM algorithm.
 *
 * @param[in] nr_test_repeats the number of times the tests should be repeated
 * @return __0__ on success, __1__ on failure
 */
static unsigned int speedtest_kem(const unsigned int nr_test_repeats) {
    unsigned int i, subtest;
    unsigned int nr_failed = 0;
    const char *subtest_names[] = {
        "crypto_kem_keypair",
        "crypto_kem_enc",
        "crypto_kem_dec",
    };
    unsigned char ct[CRYPTO_CIPHERTEXTBYTES];
    unsigned char ss_r[CRYPTO_BYTES];
    unsigned char ss_i[CRYPTO_BYTES];
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];

    start_speed_test_suite("speed_tests", subtest_names, 3, nr_test_repeats);

    for (i = 0; i < nr_test_repeats; ++i) {
        subtest = 0;

        /* Setup key pairs */
        TIME_TEST_REPEAT(subtest++, i, crypto_kem_keypair(pk, sk));

        /* Encode  */
        TIME_TEST_REPEAT(subtest++, i, crypto_kem_enc(ct, ss_r, pk));

        /* Decode */
        TIME_TEST_REPEAT(subtest++, i, crypto_kem_dec(ss_i, ct, sk));

        if (memcmp(ss_r, ss_i, CRYPTO_BYTES)) {
            ++nr_failed;
            fprintf(stderr, "Failed test %u\n", i);
        }
    }

    if (nr_failed) {
        fprintf(stderr, "Failed %u times (%u%%)\n", nr_failed, 100 * nr_failed / nr_test_repeats);
    }

    end_speed_test_suite("Complete Round2.KEM");

    return nr_failed != 0;
}

/**
 * Runs the speed tests for the individual steps of the PKE algorithm.
 *
 * @param[in] nr_test_repeats the number of times the tests should be repeated
 * @return __0__ on success, __1__ on failure
 */
static unsigned int speedtest_encrypt(const unsigned int nr_test_repeats) {
    unsigned int i, subtest;
    unsigned int nr_failed = 0;
    const char *subtest_names[] = {
        "crypto_encrypt_keypair",
        "crypto_encrypt",
        "crypto_encrypt_open",
    };
    const char *message = "This is the message to be encrypted.";
    const unsigned long long message_len = strlen(message) + 1;
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];
    unsigned char *c = malloc(CRYPTO_BYTES + message_len);
    unsigned char *m = malloc(message_len);
    unsigned long long clen;
    unsigned long long mlen;

    memset(c, 0, CRYPTO_BYTES + message_len);
    memset(m, 0, message_len);

    start_speed_test_suite("speed_tests", subtest_names, 3, nr_test_repeats);

    for (i = 0; i < nr_test_repeats; ++i) {
        subtest = 0;

        /* Setup key pairs */
        TIME_TEST_REPEAT(subtest++, i, crypto_encrypt_keypair(pk, sk));

        /* Encode  */
        TIME_TEST_REPEAT(subtest++, i, crypto_encrypt(c, &clen, (const unsigned char *) message, message_len, pk));

        /* Decode */
        TIME_TEST_REPEAT(subtest++, i, crypto_encrypt_open(m, &mlen, c, clen, sk));

        if (message_len != mlen || memcmp(message, m, message_len)) {
            ++nr_failed;
            fprintf(stderr, "Failed test %u\n", i);
        }
    }

    if (nr_failed) {
        fprintf(stderr, "Failed %u times (%u%%)\n", nr_failed, 100 * nr_failed / nr_test_repeats);
    }

    end_speed_test_suite("Complete Round2.PKE");

    return nr_failed != 0;
}

/**
 * Prints a usage message on `stderr` and exits the program.
 *
 * @param[in] message additional message to show (`NULL` for no message)
 */
static void usage(const char *message) {
    if (message != NULL) {
        fprintf(stderr, "%s\n", message);
    }
    fprintf(stderr, "Usage: speedtest [-r <repeats>]\n");
    exit(EXIT_FAILURE);
}

/**
 * Runs the speed tests.
 *
 * @param[in] argc the number of command-line arguments
 * @param[in] argv the command-line arguments
 * @return __0__ if the tests were run successfully, __1__ otherwise
 */
int main(int argc, char** argv) {
    parameters params;
    long number;
    int ch;
    unsigned int nr_failed = 0;

    unsigned int nr_test_repeats = 100;

    while ((ch = getopt(argc, argv, "?r:")) != -1) {
        switch (ch) {
            case 'r':
                number = strtol(optarg, NULL, 10);
                if (number <= 0) {
                    usage("Invalid number of test repeats specified");
                }
                nr_test_repeats = (unsigned int) number;
                break;
            default:
                usage(NULL);
        }
    }
    argc -= optind;
    argv += optind;
    if (argc > 0)
        usage(NULL);

    if (set_parameters_from_api(&params)) {
        fprintf(stderr, "Incorrect API parameters\n");
        exit(EXIT_FAILURE);
    }

    printf("------ Configuration %sround2_%s",
            params.q_bits == 0 ? "n" : "u",
            params.sk_size == CRYPTO_SECRETKEYBYTES ? "kem" : "pke");
    if (params.n == 1) {
        printf("_n1_fn%d", ROUND2_VARIANT_A);
    } else {
        printf("_nd");
    }
    printf(" %s%s------------------------------------\n\n",
            params.sk_size == CRYPTO_SECRETKEYBYTES ? "----" : "",
            params.n == 1 ? "" : "----");

    printf("CRYPTO_SECRETKEYBYTES  = %u\n", CRYPTO_SECRETKEYBYTES);
    printf("CRYPTO_PUBLICKEYBYTES  = %u\n", CRYPTO_PUBLICKEYBYTES);
    printf("CRYPTO_BYTES           = %u\n", CRYPTO_BYTES);
    if (CRYPTO_CIPHERTEXTBYTES != 0) {
        printf("CRYPTO_CIPHERTEXTBYTES = %u\n", CRYPTO_CIPHERTEXTBYTES);
    }
    printf("Tests are repeated %u times\n\n", nr_test_repeats);

    if (CRYPTO_CIPHERTEXTBYTES != 0) {
        nr_failed += speedtest_kem(nr_test_repeats);
    } else {
        nr_failed += speedtest_encrypt(nr_test_repeats);
    }
    return nr_failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
