/*
 * websnap.cc
 * Copyright (C) 2013 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <QApplication>
#include <QWebFrame>
#include <QPainter>
#include <QImage>
#include <cstdio>
#include "websnap.h"


WebSnap::WebSnap(QWebPage& page):
    QObject(),
    _page(page),
    _layoutCompleted(false),
    _loadCompleted(false)
{
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
    std::fprintf(stderr, "Page loading %s\n", ok ? "succeeded" : "failed");
    _loadCompleted = true;

    if (!ok) {
        QApplication::exit(1);
    }
    else {
        if (_loadCompleted && _layoutCompleted)
            emit ready();
    }
}

void
WebSnap::onLoadProgress(int progress)
{
    std::fprintf(stderr, "\rLoading %i%%", progress);
    if (progress > 99)
        std::fputc('\n', stderr);
    std::fflush(stderr);
}

void
WebSnap::onInitialLayoutCompleted()
{
    std::fprintf(stderr, "Layout completed\n");
    _layoutCompleted = true;

    if (_loadCompleted && _layoutCompleted)
        emit ready();
}


void
WebSnap::render(const QString& path)
{
    _page.setViewportSize(_page.mainFrame()->contentsSize());

    QPainter painter;
    QImage image(_page.viewportSize(), QImage::Format_ARGB32);
    painter.begin(&image);
    _page.mainFrame()->render(&painter);
    painter.end();

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
