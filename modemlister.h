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

    void initModems()
    {
        // Add default
        this->addItem("None");

        // Add all connected
        QDBusMessage reply = ModemManager->call("GetManagedObjects");
        const QDBusArgument &dbusArgs = reply.arguments().at( 0 ).value<QDBusArgument>();
        D_OP_DSDSV modemList;
        dbusArgs >> modemList;

        D_OP_DSDSV::const_iterator it = modemList.constBegin();
        auto end = modemList.constEnd();
        while(it != end){
            Modem *m = new Modem(it.key().path());

            this->addItem(it.value()["org.freedesktop.ModemManager1.Modem"]["Model"].value<QString>(), QVariant::fromValue(m));
            it++;
        }
    }

public:
    ModemLister(QWidget *parent = nullptr) :
        ModemManager(new MMDbusInterface("/org/freedesktop/ModemManager1", "org.freedesktop.DBus.ObjectManager"))
    {
        initModems();
        connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onModemChange(int)));

        // Signal connections for dbus
        ModemManager->connect("InterfacesAdded", this, SLOT(addModem(const QDBusObjectPath&, D_S_DSV)));
        ModemManager->connect("InterfacesRemoved", this, SLOT(removeModem(const QDBusObjectPath&)));
    }
    ~ModemLister()
    {
        ModemManager->disconnect("InterfacesAdded", this, SLOT(addModem(const QDBusObjectPath&)));
        ModemManager->disconnect("InterfacesRemoved", this, SLOT(removeModem(const QDBusObjectPath&)));
    }

    void setDefaultModem()
    {
        selectModem(this->count()-1);
    }
    void selectModem(int index)
    {
        this->setCurrentIndex(index);
    }

private slots:
    void onModemChange(int index)
    {
        if(currentModem != nullptr)
        {
            currentModem->disconnect();
            currentModem = nullptr;
        }

        if(index == 0)
            modemUnloaded();
        else
        {
            currentModem = this->currentData().value<Modem*>();
            connect(currentModem, SIGNAL(newSMS(QString)), this, SLOT(onSMSReceive(QString)));
            connect(currentModem, SIGNAL(smsRemoved(QString)), this, SLOT(onSMSDelete(QString)));
            modemLoaded(currentModem);
        }
    }
    void addModem(const QDBusObjectPath& op, D_S_DSV props)
    {
        Modem *m = new Modem(op.path());
        this->addItem(props["org.freedesktop.ModemManager1.Modem"]["Model"].value<QString>(),
                QVariant::fromValue(m));

        modemConnected(m);
        Notify("Modem Connected: ", props["org.freedesktop.ModemManager1.Modem"]["Model"].value<QString>());

        if(this->count() == 2)
            this->selectModem(1);
    }
    void removeModem(const QDBusObjectPath& op)
    {
        for(int i = 1; i<this->count(); i++)    //Start from 1 since first value is none
        {
            Modem *m = this->itemData(i).value<Modem*>();
            if(m->dbusPath == op.path())
            {
                Notify("Modem Disconnected: ", this->currentText());
                if(i == this->currentIndex())
                    this->selectModem(i-1);
                delete m;
                this->removeItem(i);
                modemDisconnected(op.path());
                break;
            }
        }
    }
    void onSMSReceive(QString dbusPath)
    {
        smsReceived(new SMS(dbusPath));
    }
    void onSMSDelete(QString dbusPath)
    {
        smsDeleted(dbusPath);
    }

signals:
    void modemConnected(Modem*);
    void modemLoaded(Modem*);
    void modemUnloaded();
    void modemDisconnected(QString);
    void smsReceived(SMS*);
    void smsDeleted(QString dbusPath);
};

#endif // MODEMLISTER_H
