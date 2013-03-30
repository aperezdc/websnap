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
#include <cstdio>

namespace posix_rt {
#include <time.h>
};

namespace posix {
#include <unistd.h>
};


Clock::Clock(bool autoReset):
    _start(0),
    _end(0)
{
    if (autoReset)
        reset();
}


void Clock::reset()
{
    struct posix_rt::timespec tv;

    if (posix_rt::clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tv) == -1)
        std::fprintf(stderr, "Clock::reset(): %s\n", std::strerror(errno));

    _start = (tv.tv_sec * 1000 * 1000 * 1000) + tv.tv_nsec;
}


uint64_t Clock::sample()
{
    struct posix_rt::timespec tv;

    if (posix_rt::clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tv) == -1)
        std::fprintf(stderr, "Clock::reset(): %s\n", std::strerror(errno));

    _end = (tv.tv_sec * 1000 * 1000 * 1000) + tv.tv_nsec;
    return elapsed();
}


uint64_t Clock::resolution()
{
    struct posix_rt::timespec tv;

    if (posix_rt::clock_getres(CLOCK_PROCESS_CPUTIME_ID, &tv) == -1)
        return (uint64_t) -1;
    return (tv.tv_sec * 1000 * 1000 * 1000) + tv.tv_nsec;
}


bool Clock::supported()
{
    return resolution() != (uint64_t) -1;
}


#ifdef MAIN
int main(int argc, char *argv[])
{
    if (Clock::supported()) {
        Clock process_time;
        Clock resolution;

        while (resolution.sample() == 0) /* nothing */;

        std::printf("CLOCK_PROCESS_CPUTIME_ID is supported\n");
        std::printf("Resolution (advertised/empirical): %lu/%luns\n",
                    (unsigned long) Clock::resolution(),
                    (unsigned long) resolution.elapsed());
        std::printf("Printing the two lines above took %luns\n",
                    (unsigned long) process_time.sample());
    }
    else {
        std::printf("CLOCK_PROCESS_CPUTIME_ID is NOT supported\n");
    }
}
#endif // MAIN

