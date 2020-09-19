// Copyright (C) 2019 Hari Saksena <hari.mail@protonmail.ch>
// 
// This file is part of MMconneqt.
// 
// MMconneqt is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// MMconneqt is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with MMconneqt.  If not, see <http://www.gnu.org/licenses/>.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "composer.h"
#include "autodeletes.h"

#include "core.h"
#include "modem.h"
#include "sms.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

private:
    Ui::MainWindow *ui;

    composer *cw = new composer();
    Autodeletes *aw = new Autodeletes();

    void initSysTray();
    void initUi();

private slots:
    // Slots for Modem Signals
    void infoUpdate(Modem*);
    void infoClear();

    // Slots for Messaging Signals
    void smsLoad(SMS*);
    void smsMultiLoad(QList<SMS*>);
    void clearViewer();
    void onSMSSingleSelect(SMS*);
    void onSMSMultiSelect(QList<SMS*>);
    void onSMSFilterSelect(SMSFilter);

    // Button Slots
    void onSendClicked();
    void onReplyClicked();
    void onResendClicked();
    void onAutoDelClicked();

    // Notification Slots
    void setStatus(QString, int);

    // SysTray Slots
    void openModemViewer();
    void openMessageViewer();
    void quit();
};
#endif // MAINWINDOW_H
