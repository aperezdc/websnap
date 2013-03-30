/*
 * websnap.h
 * Copyright (C) 2013 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __websnap_h__
#define __websnap_h__

#include <QObject>
#include <QWebPage>

class QWebFrame;


class WebSnapPage : public QWebPage
{
    Q_OBJECT

protected:
    QString chooseFile(QWebFrame* frame, const QString& suggestedFile);
    void javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& sourceId);
    bool javaScriptPrompt(QWebFrame* frame, const QString& msg, const QString& defaultValue, QString* result);
    void javaScriptAlert(QWebFrame *frame, const QString& msg);
    bool javaScriptConfirm(QWebFrame* frame, const QString& msg);
};


class WebSnap : public QObject
{
    Q_OBJECT

private:
    class QWebPage& _page;
    bool _layoutCompleted;
    bool _loadCompleted;

public:
    WebSnap(QWebPage& page);
    void render(const QString& path = QString::null);

public Q_SLOTS:
    void onLoadFinished(bool ok);
    void onLoadProgress(int progress);
    void onInitialLayoutCompleted();

Q_SIGNALS:
    void ready();
};

#endif /* !__websnap_h__ */

