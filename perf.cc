/*
 * perf.cc
 * Copyright (C) 2013 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "perf.h"
#include "websnap.h"
#include "gcpuprofiler.h"
#include "clock.h"
#include <cstdio>
#include <QApplication>
#include <QWebFrame>


Perf::Perf(WebSnap& snap, uint times, const QString output, const QString pngPath):
    QObject(),
    _snap(snap),
    _times(times),
    _output(output),
    _pngPath(pngPath)
{
    connect(&_snap, SIGNAL(ready()), SLOT(run()), Qt::UniqueConnection);
}


void Perf::run()
{
    Clock clk;
    GCpuProfiler *profiler = (_output.isEmpty()) ? 0 : new GCpuProfiler(_output.toAscii());

    if (profiler)
        profiler->start();

    for (uint i = 0; i < _times; i++)
        _snap.render();

    clk.sample();
    if (profiler)
        profiler->stop();


    std::printf("%s %u %lu\n",
                qPrintable(_snap.page().mainFrame()->url().toString()),
                _times,
                (unsigned long) (clk.elapsed() / 1000 / 1000));

    if (!_pngPath.isEmpty())
        _snap.render(_pngPath);

    delete profiler;
    QApplication::exit();
}

