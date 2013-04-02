/*
 * perf.h
 * Copyright (C) 2013 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __perf_h__
#define __perf_h__

#include <QObject>
#include <QString>

class WebSnap;


class Perf : public QObject
{
    Q_OBJECT

private:
    WebSnap& _snap;
    uint     _times;
    QString  _output;
    QString  _pngPath;

public:
    Perf(WebSnap& snap,
         uint times = 1,
         const QString output = QString::null,
         const QString pngPath = QString::null);

public Q_SLOTS:
    void run();
};

#endif /* !__perf_h__ */

