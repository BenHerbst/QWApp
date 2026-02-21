//
// Created by ben on 16.02.26.
//

#ifndef UNTITLED2_WEBPAGE_H
#define UNTITLED2_WEBPAGE_H
#include <QWebEnginePage>


class Webpage : public QWebEnginePage {
    Q_OBJECT;

public:
    explicit Webpage(QWebEngineProfile *profile, QObject *parent = nullptr);

protected:
    QWebEnginePage *createWindow(WebWindowType type) override;
};


#endif //UNTITLED2_WEBPAGE_H