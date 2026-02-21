//
// Created by ben on 16.02.26.
//

#include "webpage.h"
#include <QDesktopServices>
#include <QWebEngineNewWindowRequest>

Webpage::Webpage(QWebEngineProfile *profile, QObject *parent)
    : QWebEnginePage(profile, parent) {}

QWebEnginePage *Webpage::createWindow(WebWindowType type) {
    QWebEnginePage *tempPage = new QWebEnginePage(this->profile(), this);

    connect(tempPage, &QWebEnginePage::urlChanged, this, [tempPage](const QUrl &url) {
        if (url.isValid() && url.toString() != "about:blank") {
            QDesktopServices::openUrl(url);

            tempPage->deleteLater();
        }
    });

    return tempPage;
}