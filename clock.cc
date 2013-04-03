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
#include <cmath>
#include <time.h>
#include <unistd.h>

#define CLOCK_MAX_RESOLUTION_DELTA  (10000.0 * 1e-9)
#define CLOCK_MIN_RESOLUTION_DELTA  (1e-10)


static double
posixClockTheoricalResolution()
{
    struct timespec tv;
    if (clock_getres(CLOCK_PROCESS_CPUTIME_ID, &tv) == -1)
        return NAN;
    return (double) tv.tv_sec + (double) tv.tv_nsec * 1e-9;
}


static bool
posixClockAvailable()
{
    return !isnan(posixClockTheoricalResolution());
}


static inline double
posixClockEmpiricalResolution()
{
    struct timespec tv;
    double start;
    double now;

    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tv) == -1)
        assert(false);

    start = (double) tv.tv_sec + (double) tv.tv_nsec * 1e-9;

    do {
        if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tv) == -1)
            assert(false);
        now = (double) tv.tv_sec + (double) tv.tv_nsec * 1e-9;
    } while ((now - start) < CLOCK_MIN_RESOLUTION_DELTA);

    return (now - start);
}


bool usePosixClock()
{
    static bool checked = false;
    static bool useposix;

    if (!checked) {
        if (posixClockAvailable()) {
            double res_theorical = posixClockTheoricalResolution();
            double res_empirical = posixClockEmpiricalResolution();
            double res_delta = fabs(res_theorical - res_empirical);

            if (res_delta > CLOCK_MAX_RESOLUTION_DELTA) {
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
    _start(0.0),
    _end(0.0)
{
    if (autoReset)
        reset();
}


void
Clock::reset()
{
    if (usePosixClock()) {
        struct timespec tv;
        if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tv) == -1)
            assert(false);
        _start = (double) tv.tv_sec + (double) tv.tv_nsec * 1e-9;
    }
    else {
        _start = (double) clock() / (double) CLOCKS_PER_SEC;
    }
}


double
Clock::sample()
{
    if (usePosixClock()) {
        struct timespec tv;
        if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tv) == -1)
            assert(false);
        _end = (double) tv.tv_sec + (double) tv.tv_nsec * 1e-9;
    }
    else {
        _end = (double) clock() / (double) CLOCKS_PER_SEC;
    }
    return elapsed();
}


#ifdef MAIN
int
main(int argc, char *argv[])
{
    Clock process_time;
    if (posixClockAvailable()) {
        std::printf("CLOCK_PROCESS_CPUTIME_ID is supported\n");
        std::printf("Resolution (advertised/empirical): %.10f/%.10fs\n",
                    posixClockTheoricalResolution(),
                    posixClockEmpiricalResolution());
    }
    else {
        std::printf("CLOCK_PROCESS_CPUTIME_ID is NOT supported\n");
    }

    Clock resolution;
    while (resolution.sample() < CLOCK_MIN_RESOLUTION_DELTA) /* nothing */ ;

    std::printf("Sampled resolution: %.10fs\n", resolution.elapsed());
    std::printf("Printing the lines above took %.10fs\n", process_time.sample());
}
#endif // MAIN

