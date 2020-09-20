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

#ifndef CORE_H
#define CORE_H

// Essential Includes
#include <QString>
#include <QStringList>

// DBus Incldues
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QDBusMetaType>

// MMCli Process Interface Includes (Old)
#include <QProcess>

// Other Interfaces
#include <QSettings>
#include <QSystemTrayIcon>
#include <QMenu>

// Debugging Includes
#include <QDebug>


// Autodeletion Storage
static QSettings _settingsAutodelete("MMConneqt", "Autodeletes");

extern QStringList getAutoDeletes();
extern void setAutoDeletes(QStringList autodels);
extern void addAutoDelete(QString num);
extern void removeAutoDelete(QString num);

// Notifications
const QString ICON_FILE_SCALABLE=":/icons/icon.svg";
const QString ICON_FILE=":/icons/icon.png";
extern QSystemTrayIcon *sysTray;
extern QMenu *sysContext;

extern void stub(QString caller, QString message="");
extern void Notify(QString title, QString msg);

// Modem Manager Interfaces
const QString MM_SERVICE = "org.freedesktop.ModemManager1";
const QString MM_PROPERTIES_INTERFACE = "org.freedesktop.DBus.Properties";

typedef QMap<QString, QVariant> D_SV;
typedef QMap<QString, D_SV> D_S_DSV;
typedef QMap<QDBusObjectPath, D_S_DSV> D_OP_DSDSV;

Q_DECLARE_METATYPE(D_SV);
Q_DECLARE_METATYPE(D_S_DSV);
Q_DECLARE_METATYPE(D_OP_DSDSV);

class MMDbusInterface : public QDBusInterface{
    Q_OBJECT
public:
    MMDbusInterface(QString path, QString interface);

    void connect(QString signal, QObject *receiver, const char* slot);
    void disconnect(QString signal, QObject *receiver, const char* slot);
};
class PropertiesInterface : public MMDbusInterface{
    Q_OBJECT
public:
    PropertiesInterface(QString path) :
        MMDbusInterface(path, MM_PROPERTIES_INTERFACE) {}

    QVariant get(QString interface, QString name);
};

// MMCli Interfacing through Keys (Old)
class Keys{
private:
    QHash<QString, QString> KeyPairs = {};

public:
    Keys(QStringList args)
    {
        QProcess mmcli;
        mmcli.start("mmcli", QStringList() << args << "-K");
        mmcli.waitForFinished();
        QString out = mmcli.readAllStandardOutput();
        QStringList lines = out.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
        for(const auto& line:lines)
        {
            int pos = line.indexOf(":");
            if(pos != -1)
            {
                QString left = line.left(pos).trimmed();
                QString right = line.right(line.size()-pos-1).trimmed();
                KeyPairs[left] = right;
            }
        }
    }

    QString get(QString property)
    {
        return KeyPairs[property];
    }

    QStringList getStack(QString prefix)
    {
        QStringList ret = {};
        int count = KeyPairs[prefix+".length"].toInt();
        for(int i=1; i<=count; i++)
        {
            ret.push_back(KeyPairs[prefix+".value["+QString::number(i)+"]"]);
        }
        return ret;
    }
    QHash<QString, QString> getAll()
    {
        return KeyPairs;
    }

    static void mmDeleteSMS(QString modem, QString sms)
    {
        Keys({"-m", modem, "--messaging-delete-sms="+sms});
    }
};

#endif // CORE_H
