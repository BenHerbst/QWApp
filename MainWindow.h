//
// Created by ben on 15.02.26.
//

#ifndef UNTITLED2_MAINWINDOW_H
#define UNTITLED2_MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QWebEngineView>

QT_FORWARD_DECLARE_CLASS(QWebEngineProfile)
QT_FORWARD_DECLARE_CLASS(QSettings)


class MainWindow : public QMainWindow {
    Q_OBJECT;

    public:
        MainWindow();

    protected:
        void closeEvent(QCloseEvent *event) override;

    void showSettings();

    void applyTraySettings();

    void onNotificationClick();

    void updateAudioSettings();

    void handleDownload(QWebEngineDownloadRequest *download);

    void sendNotification(const QString &title, const QString &text, QString profilePic);

    void changeScale(double newScale);

    double getScale();

private:
        QIcon appIcon;
        QSystemTrayIcon *trayIcon = nullptr;
        QAction *hideAction;
        QAction *showMenubarAct;
        QWebEnginePage *page;
        QWebEngineView *view;

        void saveWindow();

        void loadWindow();

        void showHide();
            void quitApp();
};


#endif //UNTITLED2_MAINWINDOW_H