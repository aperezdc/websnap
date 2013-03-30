/*
 * websnap.cc
 * Copyright (C) 2013 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <QApplication>
#include <QWebFrame>
#include <QWebPage>
#include <QUrl>
#include <cstdio>
#include "websnap.h"



int
main(int argc, char *argv[])
{
    QApplication app(argc, argv, QApplication::GuiServer);
    std::fprintf(stderr, "QApplication created\n");

    WebSnapPage page;
    std::fprintf(stderr, "Page created\n");
    QUrl url(QUrl::fromUserInput((argc > 1) ? argv[1] : "http://igalia.com"));
    std::fprintf(stderr, "Loading %s\n", qPrintable(url.toString()));

    WebSnap snap(page);

    page.mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    page.mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    page.mainFrame()->load(url);

    return app.exec();
}
