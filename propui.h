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

#ifndef PROPUI_H
#define PROPUI_H

#include <QLabel>
#include <QGroupBox>

#include "core.h"
#include "modem.h"

enum TargetType{
    ao      = 0,
    a_s_su  = 1,
    s_uu    = 2,
    s_ub    = 3
};

class Propui : public QLabel
{
    Q_OBJECT
private:
    QStringList _types = {
                            "ao",
                            "a(su)",
                            "(uu)",
                            "(ub)",
                         };

    void hide(){
        stub("Propui::hide");
        this->setEnabled(false);
        qDebug() << this->layout();
    }

    void setInt(int val)
    {
        this->setText(QString::number(val));
    }
    void setString(QString val)
    {
        this->setText(val);
    }
    void setStringList(QStringList val)
    {
        this->setText("[" + val.join(",") + "]");
    }

    void demarshall(QDBusArgument arg)
    {
        QString t = arg.currentSignature();

        switch (_types.indexOf(t))
        {
        case ao:
            demarshall_ao(arg);
            break;

        case a_s_su:
            demarshall_a_s_su(arg);
            break;

        case s_uu:
            demarshall_s_uu(arg);
            break;

        case s_ub:
            demarshall_s_ub(arg);
            break;

        default:
            qDebug() << "Type not handled: " + t;
            this->setText("Unknown");
        }
    }

    void demarshall_ao(const QDBusArgument &arg)
    {
        QStringList ops;
        arg.beginArray();
        while (!arg.atEnd())
        {
            QDBusObjectPath op;
            arg >> op;
            ops.push_back(op.path());
        }
        arg.endArray();
        this->setText("[" + ops.join(",") + "]");
    }

    void demarshall_a_s_su(const QDBusArgument &arg)
    {
        QStringList entries;
        arg.beginArray();
        while (!arg.atEnd())
        {
            QString str;
            uint u;
            arg.beginStructure();
            arg >> str >> u;
            arg.endStructure();
            entries.push_back(str + " (" + QString::number(u) + ")");
        }
        arg.endArray();
        this->setText(entries.join(", "));
    }

    void demarshall_s_uu(const QDBusArgument &arg)
    {
        uint u1, u2;
        arg.beginStructure();
        arg >> u1 >> u2;
        arg.endStructure();
        this->setText(QString::number(u1) + ":" + QString::number(u2));
    }

    void demarshall_s_ub(const QDBusArgument &arg)
    {
        uint u;
        bool b;
        arg.beginStructure();
        arg >> u >> b;
        arg.endStructure();
        this->setText(QString::number(u) + ": " + (b ? "True":"False"));
    }

public:
    Propui(QWidget *parent = nullptr) {}

public slots:
    void set(Modem* m)
    {
        QVariant variant = m->get(this->property("Target").value<QString>(),
                                  static_cast<Mt>(this->property("TargetMt").value<QString>().toInt()));

        switch (variant.userType())
        {
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
            setInt(variant.value<int>());
            break;

        case 1031:  // Object Path
            setString(variant.value<QDBusObjectPath>().path());
            break;

        case QMetaType::QString:
            setString(variant.value<QString>());
            break;

        case QMetaType::QStringList:
            setStringList(variant.value<QStringList>());
            break;

        case 1029: // DBusArgument (Demarshall)
            demarshall(variant.value<QDBusArgument>());
            break;

        default:
            qDebug() << "Not Implemented: " << this->property("Target") << "-> Type:" << variant.userType();
            this->setText("Not Found");
            this->hide();
        }
    }


};

#endif // PROPUI_H
