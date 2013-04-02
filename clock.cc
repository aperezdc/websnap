/*
 * clock.cc
 * Copyright (C) 2013 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif // !_POSIX_C_SOURCE

#include "clock.h"
#include <cstring>
#include <cerrno>
#include <cassert>
#include <cstdio>
#include <time.h>
#include <unistd.h>


//
// If the claimed POSIX clock resolution and the measured one exceed
// MAX_RESOLUTION_DELTA then it will be assumed that the clock source
// is not reliable and the standard clock() function will be used.
//
#define CLOCK_MAX_RESOLUTION_DELTA  10000


static uint64_t
posixClockTheoricalResolution()
{
    struct timespec tv;
    if (clock_getres(CLOCK_PROCESS_CPUTIME_ID, &tv) == -1)
        return (uint64_t) -1;
    return (tv.tv_sec * 1000 * 1000 * 1000) + tv.tv_nsec;
}


static bool
posixClockAvailable()
{
    return posixClockTheoricalResolution() != (uint64_t) -1;
}


static inline uint64_t
posixClockEmpiricalResolution()
{
    struct timespec tv;
    uint64_t start;
    uint64_t now;

    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tv) == -1) {
        assert(false);
    }
    start = (tv.tv_sec * 1000 * 1000 * 1000) + tv.tv_nsec;

    do {
        if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tv) == -1)
            assert(false);
        now = (tv.tv_sec * 1000 * 1000 * 1000) + tv.tv_nsec;
    } while (start >= now);

    return (now - start);
}


bool usePosixClock()
{
    static bool checked = false;
    static bool useposix;

    if (!checked) {
        if (posixClockAvailable()) {
            uint64_t res_theorical = posixClockTheoricalResolution();
            uint64_t res_empirical = posixClockEmpiricalResolution();
            uint64_t delta = (res_theorical > res_empirical)
                           ? (res_theorical - res_empirical)
                           : (res_empirical - res_theorical);

            if (delta > CLOCK_MAX_RESOLUTION_DELTA) {
                std::fprintf(stderr,
                             "CLOCK_PROCESS_CPUTIME_ID does not work accurately, "
                             "using fallback\n");
                useposix = false;
            }
            else {
                useposix = true;
            }
        }
        else {
            std::fprintf(stderr,
                         "CLOCK_PROCESS_CPUTIME_ID not available, using fallback\n");
            useposix = false;
        }
        checked = true;
    }
    return useposix;
}


Clock::Clock(bool autoReset):
    _start(0),
    _end(0)
{
    if (autoReset)
        reset();
}


void Clock::reset()
{
    if (usePosixClock()) {
        struct timespec tv;
        if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tv) == -1)
            assert(false);
        _start = (tv.tv_sec * 1000 * 1000 * 1000) + tv.tv_nsec;
    }
    else {
        _start = clock() * 1000 * 1000 * 1000 / CLOCKS_PER_SEC;
    }
}


uint64_t Clock::sample()
{
    if (usePosixClock()) {
        struct timespec tv;
        if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tv) == -1)
            assert(false);
        _end = (tv.tv_sec * 1000 * 1000 * 1000) + tv.tv_nsec;
    }
    else {
        _end = clock() * 1000 * 1000 * 1000 / CLOCKS_PER_SEC;
    }
    return elapsed();
}


#ifdef MAIN
int main(int argc, char *argv[])
{
    Clock process_time;
    if (posixClockAvailable()) {
        std::printf("CLOCK_PROCESS_CPUTIME_ID is supported\n");
        std::printf("Resolution (advertised/empirical): %lu/%luns\n",
                    (unsigned long) posixClockTheoricalResolution(),
                    (unsigned long) posixClockEmpiricalResolution());
    }
    else {
        std::printf("CLOCK_PROCESS_CPUTIME_ID is NOT supported\n");
    }

    Clock resolution;
    while (!resolution.sample()) /* nothing */ ;

    std::printf("Sampled resolution: %luns\n",
                (unsigned long) resolution.elapsed());
    std::printf("Printing the lines above took %luns\n",
                (unsigned long) process_time.sample());
}
#endif // MAIN

