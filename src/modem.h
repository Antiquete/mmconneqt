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

#ifndef MODEM_H
#define MODEM_H

#include "core.h"

enum Mt{
    Generic = 0,
    Messaging = 1,
    Firmware = 2,
    Location = 3,
    Modem3GPP = 4,
    Modem3GPP_USSD = 5,
    Simple = 6,
    Voice = 7
};

class Modem : public QObject{
    Q_OBJECT
private:
    QString _interfacePaths[8] = {MM_SERVICE + ".Modem",
                                  MM_SERVICE + ".Modem.Messaging",
                                  MM_SERVICE + ".Modem.Firmware",
                                  MM_SERVICE + ".Modem.Location",
                                  MM_SERVICE + ".Modem.Modem3gpp",
                                  MM_SERVICE + ".Modem.Modem3gpp.Ussd",
                                  MM_SERVICE + ".Modem.Simple",
                                  MM_SERVICE + ".Modem.Voice"};

    MMDbusInterface *_interfaces[8];

    // Private Functions
    void initInterfaces();
    void initConnections();
    void deleteConnections();
    void deleteInterfaces();

public:
    QString dbusPath;
    PropertiesInterface *Properties;

    Modem(QString dbusPath);
    ~Modem();

    MMDbusInterface *interface(Mt index);
    QString interfacePath(Mt index);
    QVariant get(QString name, Mt i);

private slots:
    // Dbus Signal Receivers
    void StateChanged(int a, int b, unsigned int c);
    void smsAdded(const QDBusObjectPath& op, bool flag);
    void smsDeleted(const QDBusObjectPath& op);

signals:
    void newSMS(QString dbusPath);
    void smsRemoved(QString dbusPath);
};

#endif // MODEM_H
