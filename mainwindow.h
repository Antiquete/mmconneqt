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
#include <QGroupBox>
#include <QVBoxLayout>
#include <QTreeWidgetItem>
#include <QDateTime>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QSettings>

#include "core.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QSettings settings;

    QDBusConnection sysBus = QDBusConnection::systemBus();

    QString connectedModem = "";
    QIcon* ico;
    QSystemTrayIcon* sysIco;

    QDBusInterface *Modem, *Messaging, *Sim;

    void listAutodeletes();

    void listModems();

    bool isRead(QString sms);
    void markRead(QString sms);
    void markUnread(QString sms);

    bool isSetToAutodelete(QString num);
    void markForAutodelete(QString num);
    void unmarkFromAutodelete(QString num);

    void deleteSMS(QTreeWidgetItem*);

private slots:
    void addModem(const QDBusObjectPath&);
    void removeModem(const QDBusObjectPath&);
    void selectModem(int index);

    void clearInfo();
    void clearSMS();

    void updateInfo();
    void updateSMS();
    void updateInterfaces();
    void updateConnections();

    void selectSMS();
    void addSMS(QString, bool notify = true, bool append = false);
    void newSMS(const QDBusObjectPath&);
    void smsChanged(QString,QDBusMessage);

    void onDeleteClicked();
    void onReplyClicked();
    void onSendClicked();
    void onNewClicked();
    void onAutoDeleteClicked();

    void notify(QString, QString);

public slots:
    void sendSMS(QString, QString);
    void saveSMS(QString, QString);
};
#endif // MAINWINDOW_H
