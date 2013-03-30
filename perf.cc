/*
 * perf.cc
 * Copyright (C) 2013 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "perf.h"
#include "websnap.h"
#include "clock.h"
#include <cstdio>
#include <QApplication>


Perf::Perf(WebSnap& snap, uint times, QString output):
    QObject(),
    _snap(snap),
    _times(times),
    _output(output)
{
    connect(&_snap, SIGNAL(ready()), SLOT(run()), Qt::UniqueConnection);
}


void Perf::run()
{
    std::printf("Rendering %u times...\n", _times);

    Clock clk;
    for (uint i = 0; i < _times; i++) {
        _snap.render();
    }
    clk.sample();

    std::printf("Total: %luµs, %luµs/render\n",
                (unsigned long) (clk.elapsed() / 1000),
                (unsigned long) (clk.elapsed() / 1000 / _times));

    if (!_output.isEmpty()) {
        _snap.render(_output);
        std::printf("Saved to '%s'\n", qPrintable(_output));
    }

    QApplication::exit();
}

