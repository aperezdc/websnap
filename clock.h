/*
 * clock.h
 * Copyright (C) 2013 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __clock_h__
#define __clock_h__

#include <stdint.h>


class Clock
{
private:
    uint64_t _start;
    uint64_t _end;

public:
    Clock(bool autoReset = true);

    // Elapsed time since at the moment of the last call to sample(), ns
    uint64_t elapsed() const { return _end - _start; }

    // Take a sample of the current time, return elapsed time, ns
    uint64_t sample();

    // Reset the time counter, e.g. set the “starting time”
    void reset();

    // Helper class to use RAII to measure a code block, use it like this:
    //
    //   Clock clk;
    //   /* ... */
    //
    //   {
    //      Clock::Measure measure(clk);
    //      /* ... */
    //   }
    //
    //   time_elapsed = clk.elapsed();
    //
    class Measure {
        private:
            Clock& _clock;
        public:
            Measure(Clock& clock): _clock(clock) { _clock.reset(); }
           ~Measure() { _clock.sample(); }
    };
};

#endif /* !__clock_h__ */

