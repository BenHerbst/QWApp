//
// Created by ben on 15.02.26.
//

#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QSettings>
#include <QCheckBox>
#include <QCoreApplication>
#include <QDir>

bool isRunningInFlatpak() {
    // Check for the Flatpak metadata file in the sandbox root
    return QFile::exists("/.flatpak-info");
}

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Settings");
    setFixedSize(400, 300);

    auto *layout = new QVBoxLayout(this);
    auto *tabs = new QTabWidget(this);

    auto *generalTab = new QWidget(this);
    auto *generalLayout = new QVBoxLayout(generalTab);
    auto *viewTab = new QWidget(this);
    auto *viewLayout = new QVBoxLayout(viewTab);

    startMinimizedCheck = new QCheckBox("Start minimized");
    startOnBootCheck = new QCheckBox("Start on system boot");
    enableNotificationsCheck = new QCheckBox("Enable notifications");
    enableTrayCheck = new QCheckBox("Enable tray");

    connect(enableTrayCheck, &QCheckBox::toggled,[&](bool checked) {
        startMinimizedCheck->setEnabled(checked);
        if (!checked) {
            startMinimizedCheck->setChecked(false);
        }
    });

    generalLayout->addWidget(startMinimizedCheck);
    generalLayout->addWidget(startOnBootCheck);
    generalLayout->addWidget(enableNotificationsCheck);
    generalLayout->addWidget(enableTrayCheck);

    generalLayout->addStretch();

    showMenubarCheck = new QCheckBox("Show menubar");

    viewLayout->addWidget(showMenubarCheck);

    viewLayout->addStretch();

    tabs->addTab(generalTab, "General");
    tabs->addTab(viewTab, "View");
    layout->addWidget(tabs);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::saveSettings);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    layout->addWidget(buttonBox);

    loadSettings();
}

void SettingsDialog::saveSettings() {
    QSettings settings("com.404coding", "QWApp");

    settings.setValue("startMinimized", startMinimizedCheck->isChecked());
    settings.setValue("startOnBoot", startOnBootCheck->isChecked());
    settings.setValue("enableNotifications", enableNotificationsCheck->isChecked());
    settings.setValue("enableTray", enableTrayCheck->isChecked());

    QString autostartPath = QDir::homePath() + "/.config/autostart/qwapp.desktop";

    QString execPath = "";

    if (isRunningInFlatpak()) {
        // Flatpak
        execPath = "/usr/bin/flatpak run com._404coding.qwapp";
    } else {
        execPath = QCoreApplication::applicationFilePath();
    }

    if (startOnBootCheck->isChecked()) {
        QFile file(autostartPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "[Desktop Entry]\n";
            out << "Type=Application\n";
            out << "Name=QWApp\n";

            out << "Exec=" << execPath + "\n";
            out << "Terminal=false\n";
        }
    } else {
        QFile::remove(autostartPath);
    }

    accept();
}

void SettingsDialog::loadSettings() {
    QSettings settings("com.404coding", "QWApp");

    startMinimizedCheck->setChecked(settings.value("startMinimized", false).toBool());
    startOnBootCheck->setChecked(settings.value("startOnBoot", false).toBool());
    enableNotificationsCheck->setChecked(settings.value("enableNotifications", true).toBool());
    enableTrayCheck->setChecked(settings.value("enableTray", true).toBool());
    startMinimizedCheck->setEnabled(enableTrayCheck->isChecked());
}

void SettingsDialog::setMenuBarChecked(bool checked) {
    showMenubarCheck->setChecked(checked);
}

bool SettingsDialog::isMenuBarChecked() {
    return showMenubarCheck->isChecked();
}