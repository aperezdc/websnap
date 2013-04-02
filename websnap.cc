/*
 * websnap.cc
 * Copyright (C) 2013 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif // !_POSIX_SOURCE

#include <QApplication>
#include <QWebFrame>
#include <QPainter>
#include <QImage>
#include <cstdio>
#include <cstdarg>
#include <unistd.h>
#include "websnap.h"


static void
reportProgress(const char* fmt, ...)
{
    static bool print_progress = ::isatty(::fileno(stderr));
    va_list args;

    va_start(args, fmt);
    if (print_progress) {
        vfprintf(stderr, fmt, args);
        fflush(stderr);
    }
    va_end(args);
}


WebSnap::WebSnap(QWebPage& page):
    QObject(),
    _page(page),
    _layoutCompleted(false),
    _loadCompleted(false)
{
    _page.setViewportSize(QSize(800, 600));

    connect(&_page,
            SIGNAL(loadFinished(bool)),
            SLOT(onLoadFinished(bool)),
            Qt::UniqueConnection);

    connect(&_page,
            SIGNAL(loadProgress(int)),
            SLOT(onLoadProgress(int)),
            Qt::UniqueConnection);

    connect(_page.mainFrame(),
            SIGNAL(initialLayoutCompleted()),
            SLOT(onInitialLayoutCompleted()),
            Qt::UniqueConnection);
}


void
WebSnap::onLoadFinished(bool ok)
{
    _loadCompleted = true;
    if (!ok) {
        reportProgress("Load failed\n");
        QApplication::exit(1);
    }
    else {
        reportProgress("Load successful\n");
        if (_loadCompleted && _layoutCompleted)
            emit ready();
    }
}

void
WebSnap::onLoadProgress(int progress)
{
    reportProgress("\rLoading %i%% ", progress);
    if (progress > 99)
        reportProgress("\n");
}

void
WebSnap::onInitialLayoutCompleted()
{
    _layoutCompleted = true;
    reportProgress("Layout completed");
    if (_loadCompleted && _layoutCompleted)
        emit ready();
}


void
WebSnap::render(const QString& path)
{
    const QSize size(_page.mainFrame()->contentsSize());

    if (!size.width() || !size.height()) {
        std::fprintf(stderr, "Size is zero (%ux%u)\n",
                     size.width(), size.height());
        QApplication::exit(2);
    }

    _page.setViewportSize(size);
    QImage image(size, QImage::Format_RGB32);

    QPainter painter(&image);
    _page.mainFrame()->render(&painter);

    if (!path.isEmpty())
        image.save(path, "png");
}


QString
WebSnapPage::chooseFile(QWebFrame*, const QString&)
{
    return QString::null;
}

bool
WebSnapPage::javaScriptConfirm(QWebFrame*, const QString&)
{
    return true;
}

bool
WebSnapPage::javaScriptPrompt(QWebFrame*, const QString&, const QString&, QString*)
{
    return true;
}

void
WebSnapPage::javaScriptConsoleMessage(const QString&, int, const QString&)
{
    /* nothing */
}

void
WebSnapPage::javaScriptAlert(QWebFrame*, const QString&)
{
    /* nothing */
}
