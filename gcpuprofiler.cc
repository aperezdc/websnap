/*
 * gcpuprofiler.cc
 * Copyright (C) 2013 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "gcpuprofiler.h"
#include <QApplication>
#include <QTimer>
#include <QDebug>
#include <unistd.h>
#include <stdlib.h>
#include <dlfcn.h>


static const char* ProfilerStartFunc_Name   = "ProfilerStart";
static const char* ProfilerStopFunc_Name    = "ProfilerStop";
static const char* ProfilerFlushFunc_Name   = "ProfilerFlush";
static const char* ProfilerEnabledFunc_Name = "ProfilingIsEnabledForAllThreads";


GCpuProfiler::GCpuProfiler(const char *filename, ulong timeout, QObject* parent):
    QObject(parent),
    _timeout(timeout),
    _filename(::strdup(filename)),
    _profilerStart(0),
    _profilerStop(0),
    _profilerFlush(0),
    _profilerEnabled(0)
{
};


bool GCpuProfiler::isEnabled()
{
    if (_profilerEnabled)
        return (*_profilerEnabled)();
    return false;
}


void GCpuProfiler::start()
{
    if (initProfiler() && !isEnabled()) {
        Q_ASSERT(_profilerStart);
        int result = (*_profilerStart)(_filename);
        if (_timeout)
            QTimer::singleShot(_timeout, this, SLOT(_onTimeout()));
        qDebug("Profiling started, code: %#x, timeout: %lu", result, _timeout);
    }
}


void GCpuProfiler::stop()
{
    if (initProfiler() && isEnabled()) {
        Q_ASSERT(_profilerStop);
        (*_profilerStop)();
    }
}


void GCpuProfiler::flush()
{
    stop();
    if (_profilerFlush)
        (*_profilerFlush)();
}


GCpuProfiler::~GCpuProfiler()
{
    flush();
}


bool GCpuProfiler::initProfiler()
{
    static bool initDone = false;
    static bool enabled = false;

    if (initDone)
        return enabled;

    void *dlhandle = ::dlopen(NULL, RTLD_LAZY | RTLD_NOLOAD | RTLD_GLOBAL);
    if (dlhandle == 0) {
        initDone = true;
        enabled = false;
        return false;
    }

    _profilerStart   = reinterpret_cast<ProfilerStartFunc>
                     (::dlsym(dlhandle, ProfilerStartFunc_Name));
    _profilerStop    = reinterpret_cast<ProfilerStopFunc>
                     (::dlsym(dlhandle, ProfilerStopFunc_Name));
    _profilerFlush   = reinterpret_cast<ProfilerFlushFunc>
                     (::dlsym(dlhandle, ProfilerFlushFunc_Name));
    _profilerEnabled = reinterpret_cast<ProfilerEnabledFunc>
                     (::dlsym(dlhandle, ProfilerEnabledFunc_Name));

    ::dlclose(dlhandle);

    if (_profilerStart == 0 || _profilerStop == 0) {
        _profilerStart   = 0;
        _profilerStop    = 0;
        _profilerFlush   = 0;
        _profilerEnabled = 0;
        initDone = true;
        enabled = false;
        return false;
    }

    qDebug("libprofile.so detected (%p, %p), output '%s'",
           _profilerStart, _profilerStop, _filename);

    initDone = true;
    enabled = true;
    return true;
}


void GCpuProfiler::_onTimeout()
{
    stop();
    emit timeout();
}
