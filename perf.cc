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
#include <QWebFrame>


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
    Clock clk;
    for (uint i = 0; i < _times; i++)
        _snap.render();
    clk.sample();

    std::printf("%s %u %lu\n",
                qPrintable(_snap.page().mainFrame()->url().toString()),
                _times,
                (unsigned long) (clk.elapsed() / 1000 / 1000));

    if (!_output.isEmpty())
        _snap.render(_output);

    QApplication::exit();
}

