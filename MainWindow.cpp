//
// Created by ben on 15.02.26.
//

#include "MainWindow.h"
#include <QApplication>
#include <QMenuBar>
#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QUrl>
#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusInterface>
#include <QStandardPaths>
#include <QSettings>
#include <QWebEngineNotification>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QCloseEvent>
#include "SettingsDialog.h"
#include <QMessageBox>
#include <QWebEngineSettings>
#include <QFileDialog>
#include <QWebEnginePermission>
#include <QTimer>
#include "webpage.h"

void MainWindow::saveWindow() {
    QSettings settings("com.404coding", "QWApp");

    settings.setValue("state", saveState());

    if (isMaximized()) {
        settings.setValue("isMaximized", isMaximized());
    } else {
        settings.setValue("geometry", saveGeometry());
        settings.setValue("isMaximized", false);
    }
}

void MainWindow::loadWindow() {
    QSettings settings("com.404coding", "QWApp");

    restoreState(settings.value("state").toByteArray());

    restoreGeometry(settings.value("geometry").toByteArray());

    if (settings.value("isMaximized").toBool()) {
        showMaximized();
    }
}

void MainWindow::showHide() {
    if (isVisible()) {
        saveWindow();
        hide();
        page->runJavaScript("if (window.gc) window.gc();");
        page->setVisible(false);
    } else {
        show();
        page->setVisible(true);
        raise();
        activateWindow();
        loadWindow();
    }
}

MainWindow::MainWindow() : QMainWindow() {
    loadWindow();

    setMinimumSize(800, 600);
    setWindowTitle("QWApp");

    appIcon = QIcon::fromTheme("whatsapp");

    if (appIcon.isNull()) {
        appIcon = QIcon(":/icons/app.png");
    }

    setWindowIcon(appIcon);

    QMenuBar *menubar = menuBar();
    auto *fileMenu = new QMenu("&File", this);
    auto *settingsAction = new QAction("Settings", this);
    auto *reloadAction = new QAction("Reload Website", this);
    auto *quitAction = new QAction("Quit", this);
    hideAction = new QAction("Hide", this);
    fileMenu->addAction(settingsAction);
    fileMenu->addAction(reloadAction);
    fileMenu->addAction(quitAction);
    fileMenu->addAction(hideAction);
    quitAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
    hideAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_H));
    settingsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_P));
    reloadAction->setShortcut(QKeySequence(Qt::Key_F5));

    addAction(settingsAction);
    addAction(reloadAction);

    connect(settingsAction, &QAction::triggered, this, &MainWindow::showSettings);

    connect(quitAction, &QAction::triggered, this, &MainWindow::quitApp);
    connect(hideAction, &QAction::triggered, this, &MainWindow::showHide);
    connect(reloadAction, &QAction::triggered, this, [&]() {
        page->triggerAction(QWebEnginePage::Reload);
    });

    auto *viewMenu = new QMenu("&View", this);
    auto *fullScreenAct = new QAction("Toggle Fullscreen", this);
    showMenubarAct = new QAction("Show Menubar", this);
    auto resetScale = new QAction("Reset scale", this);
    auto increaseScale = new QAction("Increase scale", this);
    auto decreaseScale = new QAction("Decrease scale", this);
    viewMenu->addAction(fullScreenAct);
    viewMenu->addAction(showMenubarAct);
    viewMenu->addAction(resetScale);
    viewMenu->addAction(increaseScale);
    viewMenu->addAction(decreaseScale);
    fullScreenAct->setShortcut(QKeySequence(Qt::Key_F11));
    showMenubarAct->setCheckable(true);
    showMenubarAct->setChecked(true);
    showMenubarAct->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_M));
    resetScale->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    increaseScale->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Plus));
    decreaseScale->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus));
    this->addAction(showMenubarAct);

    QSettings settings("com.404coding", "QWApp");

    connect(fullScreenAct, &QAction::triggered, [&]() {
        if (isFullScreen()) {
            showNormal();
            loadWindow();
        } else {
            saveWindow();
            showFullScreen();
        }
    });

    connect(showMenubarAct, &QAction::toggled, [menubar, this]() {
        QSettings settings("com.404coding", "QWApp");
        settings.setValue("showMenuBar", showMenubarAct->isChecked());

        if (menubar->isHidden()) {
            menubar->show();
        } else {
            menubar->hide();
        }
    });

    showMenubarAct->setChecked(settings.value("showMenuBar", true).toBool());

    auto *helpMenu = new QMenu("&Help", this);
    auto *aboutAction = new QAction("About", this);
    helpMenu->addAction(aboutAction);

    connect(aboutAction, &QAction::triggered, [&]() {
        QMessageBox::about(this, "About QWApp",
        "<h3>QWApp</h3>"
           "<p>Version 1.0</p>"
           "<p>A simple, stable client built with Qt & WebEngine.</p>"
           "<p>Made by Ben Herbst</p>");
    });

    menubar->addMenu(fileMenu);
    menubar->addMenu(viewMenu);
    menubar->addMenu(helpMenu);

    applyTraySettings();

    QString storagePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    if (!QDir(storagePath).exists()) {
        QDir().mkdir(storagePath);
    }

    QString profilePath = storagePath + "/ProfileData";

    if (!QDir(profilePath).exists()) {
        QDir().mkdir(profilePath);
    }

    qInfo() << "Profile Path: " + profilePath;

    auto *profile = new QWebEngineProfile(QStringLiteral("PersistentProfile"), this);
    profile->setHttpUserAgent("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36");
    profile->setPersistentStoragePath(profilePath);
    profile->setCachePath(storagePath);
    profile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);
    profile->setHttpCacheType(QWebEngineProfile::DiskHttpCache);
    profile->setPushServiceEnabled(true);

    profile->setNotificationPresenter([&](std::unique_ptr<QWebEngineNotification> notification) {
        QSettings settings("com.404coding", "QWApp");
        if (settings.value("enableNotifications", true).toBool()) {
            QString title = notification->title();
            QString msg = notification->message();
            QImage iconImage = notification->icon();

            QString iconPath;

            if (!iconImage.isNull()) {
                iconPath = QDir::tempPath() + QDir::separator() + "qwapp_notif_" +
                           QString::number(QDateTime::currentMSecsSinceEpoch()) + ".png";
                iconImage.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation).save(iconPath);
                iconPath = "file://" + iconPath;
            }

            sendNotification(title, msg, iconPath);
        }
    });

    page = new Webpage(profile, this);

    updateAudioSettings();

    connect(page->profile(), &QWebEngineProfile::downloadRequested, this, &MainWindow::handleDownload);

    view = new QWebEngineView(this);
    view->setPage(page);

    changeScale(settings.value("scale", 1.0).toDouble());

    connect(resetScale, &QAction::triggered, [this]() {
        changeScale(1.0);
    });

    connect(increaseScale, &QAction::triggered, [this]() {
        changeScale(getScale() + 0.1);
    });

    connect(decreaseScale, &QAction::triggered, [this]() {
        changeScale(getScale() - 0.1);
    });

    addAction(resetScale);
    addAction(increaseScale);
    addAction(decreaseScale);

    connect(page, &QWebEnginePage::permissionRequested,
        [&](const QWebEnginePermission &permission) {
            if (permission.permissionType() == QWebEnginePermission::PermissionType::Notifications
                || permission.permissionType() == QWebEnginePermission::PermissionType::MediaAudioCapture
                || permission.permissionType() == QWebEnginePermission::PermissionType::MediaVideoCapture) {
                permission.grant();
            }
    });

    view->load(QUrl("https://web.whatsapp.com/"));

    setCentralWidget(view);
}

void MainWindow::quitApp() {
    QApplication::quit();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QSettings settings("com.404coding", "QWApp");

    saveWindow();

    if (settings.value("enableTray", true).toBool()) {
        hide();
        event->ignore();
    }
}

void MainWindow::showSettings() {
    QSettings settings("com.404coding", "QWApp");

    bool oldAudioSetting = settings.value("enableNotifications", true).toBool();

    SettingsDialog dialog(this);
    dialog.setMenuBarChecked(showMenubarAct->isChecked());

    if (dialog.exec() == QDialog::Accepted) {
        settings.sync();

        showMenubarAct->setChecked(dialog.isMenuBarChecked());

        applyTraySettings();

        bool newAudioSetting = settings.value("enableNotifications", true).toBool();

        if (newAudioSetting != oldAudioSetting) {
            updateAudioSettings();
            page->triggerAction(QWebEnginePage::Reload);
        }
    }
}

void MainWindow::applyTraySettings() {
    QSettings settings("com.404coding", "QWApp");

    bool trayEnabled = settings.value("enableTray", true).toBool();

    if (trayEnabled) {
        if (!trayIcon) {
            trayIcon = new QSystemTrayIcon(this);
            connect(trayIcon, &QSystemTrayIcon::messageClicked,
                this, &MainWindow::onNotificationClick);
            auto *contextMenu = new QMenu(this);
            auto *showHideAct = new QAction("Show / Hide Window", this);
            auto *quitAct = new QAction("Quit", this);
            auto *settingsAct = new QAction("Settings", this);

            connect(showHideAct, &QAction::triggered, this, &MainWindow::showHide);
            connect(settingsAct, &QAction::triggered, this, &MainWindow::showSettings);
            connect(quitAct, &QAction::triggered, this, &MainWindow::quitApp);

            contextMenu->addAction(showHideAct);
            contextMenu->addAction(quitAct);
            contextMenu->addAction(settingsAct);

            trayIcon->setContextMenu(contextMenu);
            trayIcon->setIcon(appIcon);

            connect(trayIcon, &QSystemTrayIcon::activated, [&](QSystemTrayIcon::ActivationReason reason) {
                if (reason == QSystemTrayIcon::Trigger) {
                    showHide();
                }
            });

            trayIcon->show();
        }
    } else {
        if (trayIcon) {
            trayIcon->hide();
            trayIcon->deleteLater();
            trayIcon = nullptr;
        }
    }

    if (hideAction) {
        hideAction->setVisible(trayEnabled);
    }
}

void MainWindow::onNotificationClick() {
    showNormal();
    raise();
    activateWindow();
    page->runJavaScript("document.querySelector('footer input').focus();");
}

void MainWindow::updateAudioSettings() {
    QSettings settings("com.404coding", "QWApp");

    page->settings()->setAttribute(QWebEngineSettings::PlaybackRequiresUserGesture, !settings.value("enableNotifications", true).toBool());
}

void MainWindow::handleDownload(QWebEngineDownloadRequest *download) {
    QString suggestedPath = QDir::homePath() + "/Downloads/" + download->suggestedFileName();

    QString path = QFileDialog::getSaveFileName(this, "Save file", suggestedPath);

    if (path.isEmpty()) {
        download->cancel();
    }

    download->setDownloadDirectory(QFileInfo(path).path());
    download->setDownloadFileName(QFileInfo(path).fileName());
    download->accept();
}

void MainWindow::sendNotification(const QString &title, const QString &text, const QString profilePic) {
    QDBusInterface notify("org.freedesktop.Notifications",
                          "/org/freedesktop/Notifications",
                          "org.freedesktop.Notifications",
                          QDBusConnection::sessionBus());

    if (!notify.isValid()) {
        qWarning() << "Notification service not available";
        return;
    }

    QVariantMap hints;
    hints["desktop-entry"] = "com._404coding.qwapp.desktop";

    if (!profilePic.isEmpty()) {
        hints["image-path"] = profilePic;
    }

    notify.call("Notify",
               "QWApp",                 // app_name (string)
               quint32(0),              // replaces_id (uint32)
               QIcon::fromTheme("whatsapp").isNull() ? "" : "whatsapp",                      // app_icon
               title,                   // summary (string)
               text,                    // body (string)
               QStringList(),           // actions (string list)
               hints,                   // hints (variant map)
               qint32(5000));           // timeout (int32)

    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [profilePic]() {
        QFile::remove(profilePic);
    });
    timer->start(1000);
}

void MainWindow::changeScale(double newScale) {
    QSettings settings("com.404coding", "QWApp");

    settings.setValue("scale", newScale);
    view->setZoomFactor(newScale);
}

double MainWindow::getScale() {
    return view->zoomFactor();
}