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

#include "modem.h"

Modem::Modem(QString dbusPath) : dbusPath(dbusPath)
{
    this->initInterfaces();
    this->initConnections();
}

Modem::~Modem()
{
    this->deleteConnections();
    this->deleteInterfaces();
}

MMDbusInterface* Modem::interface(Mt index)
{
    return this->_interfaces[index];
}
QString Modem::interfacePath(Mt index)
{
    return this->_interfacePaths[index];
}

QVariant Modem::get(QString name, Mt i)
{
    QDBusReply<QVariant> prop = this->Properties->call("Get",
                                                       QVariant::fromValue<QString>(interface(i)->interface()),
                                                       QVariant::fromValue<QString>(name));
    return prop.value();
}


void Modem::initInterfaces()
{
    this->Properties = new PropertiesInterface(this->dbusPath);

    for(int i = Mt::Generic; i <= Mt::Voice; i++)
    {
        this->_interfaces[i] = new MMDbusInterface(this->dbusPath,
                                             interfacePath(static_cast<Mt>(i)));
    }
}
void Modem::initConnections()
{
    // Modem Signals
    this->interface(Mt::Generic)->connect("StateChanged", this, SLOT(StateChanged(int, int, unsigned int)));


    // Messaging Signals
    this->interface(Mt::Messaging)->connect("Added", this, SLOT(smsAdded(const QDBusObjectPath&, bool)));
    this->interface(Mt::Messaging)->connect("Deleted", this, SLOT(smsDeleted(const QDBusObjectPath&)));
}
void Modem::deleteConnections()
{
    // Modem Signals
    this->interface(Mt::Generic)->disconnect("StateChanged", this, SLOT(StateChanged(int, int, unsigned int)));


    // Messaging Signals
    this->interface(Mt::Messaging)->disconnect("Added", this, SLOT(smsAdded(const QDBusObjectPath&, bool)));
    this->interface(Mt::Messaging)->disconnect("Deleted", this, SLOT(smsDeleted(const QDBusObjectPath&)));
}
void Modem::deleteInterfaces()
{
    delete this->Properties;
    for(int i = Mt::Generic; i <= Mt::Voice; i++)
    {
        delete this->_interfaces[i];
    }
}

// Dbus Signal Receivers
void Modem::StateChanged(int a, int b, unsigned int c){
    stub("StateChanged", QString("Signal Received, Params a:%d, b:%d, c:%d").arg(a).arg(b).arg(c));
}
void Modem::smsAdded(const QDBusObjectPath& op, bool flag){
    newSMS(op.path());
}
void Modem::smsDeleted(const QDBusObjectPath& op){
    smsRemoved(op.path());
}
