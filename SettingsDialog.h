//
// Created by ben on 15.02.26.
//

#ifndef UNTITLED2_SETTINGSDIALOG_H
#define UNTITLED2_SETTINGSDIALOG_H

#include <QDialog>
#include <QCheckBox>

class SettingsDialog : public QDialog {
    Q_OBJECT;

private:
    QCheckBox *startMinimizedCheck;
    QCheckBox *startOnBootCheck;
    QCheckBox *enableNotificationsCheck;
    QCheckBox *enableTrayCheck;
    QCheckBox *showMenubarCheck;

public:
    SettingsDialog(QWidget *parent = nullptr);

    void saveSettings();

    void loadSettings();

    void setMenuBarChecked(bool checked);

    bool isMenuBarChecked();
};

#endif //UNTITLED2_SETTINGSDIALOG_H