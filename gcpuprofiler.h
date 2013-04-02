/*
 * gcpuprofiler.h
 * Copyright (C) 2013 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __gcpuprofiler_h__
#define __gcpuprofiler_h__

#include <QObject>


class GCpuProfiler : public QObject
{
    Q_OBJECT

public:
    GCpuProfiler(const char *filename, ulong timeout = 0, QObject* parent = 0);
    virtual ~GCpuProfiler();

    bool isEnabled();

public slots:
    void start();
    void stop();
    void flush();

signals:
    void timeout();

private:
    typedef int  (*ProfilerStartFunc)  (const char*);
    typedef void (*ProfilerStopFunc)   ();
    typedef void (*ProfilerFlushFunc)  ();
    typedef int  (*ProfilerEnabledFunc)();

    ulong               _timeout;
    const char         *_filename;
    ProfilerStartFunc   _profilerStart;
    ProfilerStopFunc    _profilerStop;
    ProfilerFlushFunc   _profilerFlush;
    ProfilerEnabledFunc _profilerEnabled;

    bool initProfiler();

private slots:
    void _onTimeout();
};

#endif /* !__gcpuprofiler_h__ */

