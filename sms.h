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

#ifndef SMS_H
#define SMS_H

#include <QTreeWidgetItem>
#include "core.h"

const QString MM_SMS_INTERFACE = MM_SERVICE + ".Sms";

enum SMSFilter{
    Inbox       = 0,
    Sent        = 1,
    Drafts      = 2,
    Unknown     = 3
};

class SMS : public QObject{
    Q_OBJECT
private:
    MMDbusInterface *properties = nullptr;
    QTreeWidgetItem *_refItem = nullptr;


    void setCategory(int pdu, int state)
    {
        switch (pdu)
        {
        case 2:                         //PDU - Submit
        case 5:                         //PDU - CDMA Submit
         if(state == 5)                 //State - Sent
             category = Sent;
         else
             category = Drafts;
            break;

        default:
            if(state == 3)              //State - Received
                category = Inbox;
            else
                category = Unknown;
        }
    }

public:
    QString dbusPath;
    SMSFilter category;

    SMS(QString dbusPath) : dbusPath(dbusPath)
    {
        properties = new MMDbusInterface(dbusPath, MM_PROPERTIES_INTERFACE);


        int pdu = get("PduType").value<int>();
        int state = get("State").value<int>();

        setCategory(pdu, state);

        properties->connect("PropertiesChanged", this, SLOT(onPropChange(QString, QMap<QString, QVariant>, QStringList)));
    }
    ~SMS()
    {
        properties->disconnect("PropertiesChanged", this, SLOT(onPropChange(QString, QMap<QString, QVariant>, QStringList)));
        delete properties;
    }

    void setRef(QTreeWidgetItem *ref)
    {
        _refItem = ref;
    }

    QVariant get(const char* name)
    {
        QDBusReply<QVariant> prop = this->properties->call("Get",
                                                           QVariant::fromValue<QString>("org.freedesktop.ModemManager1.Sms"),
                                                           QVariant::fromValue<QString>(name));
        return prop.value();
    }

    QString getString(const char* name)
    {
        return get(name).value<QString>();
    }

    void send()
    {
        MMDbusInterface smsInterface(this->dbusPath, MM_SMS_INTERFACE);
        smsInterface.call("Send");
    }

private slots:
    void onPropChange(QString interface, QMap<QString, QVariant> changes, QStringList invalidated)
    {
        if(_refItem == nullptr)
            qDebug() << "ref is set to nullptr, unsafe use detected!";

        moveMessage(this->dbusPath, _refItem);
    }

signals:
    void moveMessage(QString, QTreeWidgetItem*);
};

#endif // SMS_H
