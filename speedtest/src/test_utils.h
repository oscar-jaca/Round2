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
 * Declaration of utility macros & functions for use in tests.
 *
 * @author  Hayo Baan
 * @endcond
 */

#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdlib.h>
#include <stdint.h>

/**
 * Puts a wrapper around the given code to time the test repeat.
 * Note: Only for use in the speed test suite!
 *
 * @param[in] subtest   the number of the subtest
 * @param[in] repeat_nr the number of the test repeat
 * @param[in] code the  code to run for the test repeat
 */
#define TIME_TEST_REPEAT(subtest, repeat_nr, code) \
        start_speed_subtest_timing(); \
        code; \
        stop_speed_subtest_timing(subtest, repeat_nr);

/**
 * Calculates the number of elapsed milliseconds since the given start time
 *
 * @param[in] start the start time to calculate the elapsed time from
 * @return the elapsed time in milliseconds (as `double`)
 */
#define ms_elapsed_from(start) \
    ((double) (1000*(clock() - start)) / CLOCKS_PER_SEC)

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Converts a number string to the value it represents.
     *
     * @param[in] str the NULL terminated string representing the value
     * @returns the value represented by the string
     */
    int64_t string_to_number(const char *str);

    /**
     * Converts a value to a string.
     *
     * Note: It uses a fixed buffer that is shared between calls to this
     * function so do not use it twice within e.g. the same `printf` statement.
     *
     * @param[in] value the value to convert to a string
     * @return the NULL terminated string representing the value (decimal
     *         notation)
     */
    const char* number_to_string(uint64_t value);

    /**
     * Prints the message at the start of the test suite and starts its timer.
     *
     * @param[in] suite the name of the test suite
     */
    void start_test_suite(const char *suite);

    /**
     * Prints the message at the start of a test and starts its timer.
     *
     * @param[in] test the name of the test
     */
    void start_test(const char *test);

    /**
     * Prints the message at the end of a test and returns the test status.
     *
     * @param[in] error NULL in case of a successful test, otherwise the error
     *                  to be include in the test failure message
     * @return __0__ in case of success, __1__ in case of failure
     */
    unsigned int end_test(const char *error);

    /**
     * Prints the message at the end of the test suite.
     * @param[in] nr_failed the number of failed tests (0 for success)
     */
    void end_test_suite(const unsigned int nr_failed);

    /**
     * Prints the message at the start of the speed test suite and starts its
     * timer.
     *
     * @param[in] suite    the name of the test suite
     * @param[in] names    array with the names of the subtests
     * @param[in] subtests the number of subtests
     * @param[in] repeats  the number of times each speed test is repeated
     */
    void start_speed_test_suite(const char *suite, const char *names[], const unsigned int subtests, const unsigned int repeats);

    /**
     * Prints the message at the start of a speed test and starts its timer.
     *
     * @param[in] test the name of the test
     */
    void start_speed_test(const char *test);

    /**
     * Starts the cpu cycle timing of a single test repeat of a subtest.
     */
    void start_speed_subtest_timing(void);

    /**
     * Stops the cpu cycle timing of a single speed subtest test repeat.
     *
     * @param[in] subtest the subtest number
     * @param[in] repeat_nr the number of the test repeat
     */
    void stop_speed_subtest_timing(const unsigned int subtest, const unsigned int repeat_nr);

    /**
     * Administrates the completion of a speed subtests.
     *
     * @param[in] subtest number of the subtest.
     * @param[in] test_name name of the subtest.
     */
    void done_speed_test(const unsigned int subtest, char *test_name);

    /**
     * Prints the message at the end of the speed test suite.
     * @param[in] summary pointer to a string describing the summary, or NULL if
     *                    no summary should be printed
     */
    void end_speed_test_suite(const char *summary);

#ifdef __cplusplus
}
#endif

#endif /* TEST_UTILS_H */
