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

#ifndef MODEMLISTER_H
#define MODEMLISTER_H

#include <QComboBox>

#include "core.h"
#include "modem.h"
#include "sms.h"

class ModemLister : public QComboBox
{
    Q_OBJECT
private:
    MMDbusInterface *ModemManager;
    Modem *currentModem = nullptr;

    void initModems();

public:
    ModemLister(QWidget *parent = nullptr);
    ~ModemLister();

    void setDefaultModem();
    void selectModem(int index);

private slots:
    void onModemChange(int index);
    void addModem(const QDBusObjectPath& op, D_S_DSV props);
    void removeModem(const QDBusObjectPath& op);
    void onSMSReceive(QString dbusPath);
    void onSMSDelete(QString dbusPath);

signals:
    void modemConnected(Modem*);
    void modemLoaded(Modem*);
    void modemUnloaded();
    void modemDisconnected(QString);
    void smsReceived(SMS*);
    void smsDeleted(QString dbusPath);
};

#endif // MODEMLISTER_H
