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


    void setCategory(int pdu, int state);

public:
    QString dbusPath;
    SMSFilter category;

    SMS(QString dbusPath);
    ~SMS();

    void setRef(QTreeWidgetItem *ref);
    QVariant get(const char* name);
    QString getString(const char* name);
    void send();

private slots:
    void onPropChange(QString interface, QMap<QString, QVariant> changes, QStringList invalidated);

signals:
    void moveMessage(QString, QTreeWidgetItem*);
};

#endif // SMS_H
