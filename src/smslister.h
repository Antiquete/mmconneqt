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

#ifndef SMSLISTER_H
#define SMSLISTER_H

#include <QTreeWidget>

#include "core.h"
#include "modem.h"
#include "sms.h"
#include "autodeletes.h"

enum SelectState{
    None,
    SingleSelect,
    MultiSelect,
    FilterSelect
};


class SMSLister : public QTreeWidget
{
    Q_OBJECT

private:
    Modem *modem = nullptr;
    QSettings *smsDb;

    SMS* smsObject(QTreeWidgetItem *it);

    void initUi();
    void initMessages();
    void addSMS(QString dbusPath);
    void deleteSMS(QTreeWidgetItem *it);
    bool isRead(QTreeWidgetItem *it);
    void setRead(QTreeWidgetItem *it);
    void setUnread(QTreeWidgetItem *it);
public:
    SelectState SelectionType = SelectState::None;

    SMS* selectedSMS();
    QString category(SMSFilter sfilter);

    SMSLister(QWidget *parent = nullptr);

    void deleteListed(QList<QTreeWidgetItem*> itms);


public slots:
    void setModem(Modem *m);
    void clearMessages();

    //Messaging Actions Slots
    void saveSendSMS(QString number, QString content);
    QString saveSMS(QString number, QString content);
    void deleteSelection();

private slots:
    void onItemSelect();
    void onSMSReceive(QString dbusPath);
    void onSMSMove(QString dbusPath, QTreeWidgetItem *ref);

signals:
    void smsSelected(SMS*);
    void multiSelected(QList<SMS*>);
    void filterSelected(SMSFilter);
};

#endif // SMSLISTER_H
