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

#include <QProcess>
#include <QObject>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QList>
#include <QString>
#include <QStringList>
#include <QDebug>

#define sysBusConnect(W,X,Y,Z) (Q_ASSERT(QDBusConnection::systemBus().connect(QString(), W, X, Y, this, Z)))
#define sysBusDisconnect(W,X,Y,Z) (Q_ASSERT(QDBusConnection::systemBus().disconnect(QString(), W, X, Y, this, Z)))

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
