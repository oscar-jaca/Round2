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
 * Implementation of the random bytes function. Uses `/dev/urandom` to
 * generate unpredictable and cryptographically secure random numbers (bytes).
 *
 * @author Hayo Baan
 * @endcond
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "randombytes.h"

/*******************************************************************************
 * Private variables, and macros
 ******************************************************************************/

/**
 * Define a chunk size of 1MB. The chunk size is the maximum number of bytes
 * that is read in one go from /dev/urandom by randombytes().
 */
#define CHUNK_SIZE 1048576

/**
 * The file descriptor of /dev/urandom, -1 means uninitialised.
 */
static int urandom = -1;

/*******************************************************************************
 * Public functions
 ******************************************************************************/

int randombytes(unsigned char *x, unsigned long long xlen) {
    ssize_t s;

    /* Open /dev/urandom (if not already done) */
    while (urandom == -1) {
        urandom = open("/dev/urandom", O_RDONLY);
        if (urandom == -1) sleep(1); /* Wait a bit before retrying */
    }

    /* Get the random bytes in chunks */
    while (xlen > 0) {
        s = read(urandom, x, (size_t) (xlen < CHUNK_SIZE ? xlen : CHUNK_SIZE));
        /* Note: we completely ignore read errors (these really should never
         * occur with /dev/urandom anyway) and retry until successful. */
        if (s >= 0) {
            /* Move to next chunk */
            x += s;
            xlen -= (unsigned long long) s;
        }
    }
    return 0;
}
