/*
 * websnap.cc
 * Copyright (C) 2013 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <QApplication>
#include <QWebSettings>
#include <QWebFrame>
#include <QWebPage>
#include <QUrl>
#include <cstdio>
#include "websnap.h"
#include "perf.h"


int
main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavaEnabled, false);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, false);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, false);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, false);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled, false);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageDatabaseEnabled, false);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, false);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, false);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::FrameFlatteningEnabled, true);

    QWebPage page;
    WebSnap snap(page);
    QUrl url(QUrl::fromUserInput((argc > 1) ? argv[1] : "http://igalia.com"));
    Perf perf(snap,
              (argc > 2) ? strtoul(argv[2], NULL, 0) : 1,
              (argc > 3) ? QString(argv[3]) : QString::null,
              (argc > 4) ? QString(argv[4]) : QString::null);

    page.mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    page.mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    page.mainFrame()->load(url);

    return app.exec();
}
