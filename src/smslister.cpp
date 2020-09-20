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

#include "smslister.h"

SMSLister::SMSLister(QWidget *parent):
    smsDb(new QSettings("MMConneqt", "Messaging"))
{
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelect()));
}

SMS* SMSLister::smsObject(QTreeWidgetItem *it)
{
    return it->data(0, Qt::UserRole).value<SMS*>();
}

void SMSLister::initUi()
{
    this->addTopLevelItem(new QTreeWidgetItem(QStringList() << "Inbox"));
    this->addTopLevelItem(new QTreeWidgetItem(QStringList() << "Sent"));
    this->addTopLevelItem(new QTreeWidgetItem(QStringList() << "Drafts"));
    this->addTopLevelItem(new QTreeWidgetItem(QStringList() << "Unknown"));
}
void SMSLister::initMessages()
{
    QDBusReply<QList<QDBusObjectPath>> smsList = modem->interface(Mt::Messaging)->call("List");
    for(const auto& it:smsList.value())
    {
        addSMS(it.path());
    }
}
void SMSLister::addSMS(QString dbusPath)
{
    // Create SMS
    SMS *s = new SMS(dbusPath);
    QString num = s->get("Number").value<QString>();

    QTreeWidgetItem *itm = new QTreeWidgetItem(QStringList() << num);
    itm->setData(0, Qt::UserRole, QVariant::fromValue(s));

    // Handle Read/Unread Status.
    if(s->category == Inbox)
    {
        if(isRead(itm))
            setRead(itm);
        else
            setUnread(itm);
    }

    // Connect signals
    connect(s, SIGNAL(moveMessage(QString, QTreeWidgetItem*)), this, SLOT(onSMSMove(QString, QTreeWidgetItem*)));

    // Add SMS to TreeWidget
    this->topLevelItem(s->category)->addChild(itm);

    // Add reference of TreeWidgetItem back to SMS
    s->setRef(itm);

    // Handle Auto Deletion (After entry added to TreeWidget)
    if(getAutoDeletes().contains(num))
        deleteSMS(itm);
}
void SMSLister::deleteSMS(QTreeWidgetItem *it)
{
    setUnread(it);
    SMS *s = smsObject(it);
    modem->interface(Mt::Messaging)->call("Delete", QVariant::fromValue(QDBusObjectPath(s->dbusPath)));
    delete s;

    it->parent()->removeChild(it);
    delete it;
}
bool SMSLister::isRead(QTreeWidgetItem *it)
{
    // Default everything unread
    return smsDb->value(smsObject(it)->dbusPath, false).toBool();
}
void SMSLister::setRead(QTreeWidgetItem *it)
{
    // Normal style for read messages, store read status
    it->setForeground(0, QBrush());
    smsDb->setValue(smsObject(it)->dbusPath, true);
}
void SMSLister::setUnread(QTreeWidgetItem *it)
{
    // Colored unread message, remove read status
    it->setForeground(0, QBrush(Qt::red));
    smsDb->remove(smsObject(it)->dbusPath);
}


SMS* SMSLister::selectedSMS()
{
    return this->selectedItems().first()->data(0, Qt::UserRole).value<SMS*>();
}
QString SMSLister::category(SMSFilter sfilter)
{
    return this->topLevelItem(sfilter)->text(0);
}
void SMSLister::deleteListed(QList<QTreeWidgetItem*> itms)
{
    for(const auto& it : itms)
        deleteSMS(it);
}


void SMSLister::setModem(Modem *m)
{
    if(modem != nullptr)
        disconnect(modem, SIGNAL(newSMS(QString)), this, SLOT(onSMSReceive(QString)));

    this->modem = m;
    this->clear();

    this->initUi();
    this->initMessages();

    connect(modem, SIGNAL(newSMS(QString)), this, SLOT(onSMSReceive(QString)));
}
void SMSLister::clearMessages()
{
    for(int i=0; i<this->topLevelItemCount(); i++)
    {
        for(const auto& it:this->topLevelItem(i)->takeChildren())
        {
            delete it->data(0, Qt::UserRole).value<SMS*>();
        }
    }
    this->clear();
}

//Messaging Actions Slots
void SMSLister::saveSendSMS(QString number, QString content)
{
    SMS s(saveSMS(number,content));
    s.send();
}
QString SMSLister::saveSMS(QString number, QString content)
{
    QMap<QString, QVariant> message;
    message["number"] = number;
    message["text"] = content;

    QDBusReply<QDBusObjectPath> smsOP = modem->interface(Mt::Messaging)->call("Create", message);

    if(!smsOP.isValid())
        qDebug() << "smsLister::saveSMS(): Failed." << message << "\n" << smsOP.error();

    return smsOP.value().path();
}
void SMSLister::deleteSelection()
{
    switch (SelectionType)
    {
    case SelectState::SingleSelect:
        deleteSMS(this->selectedItems().first());
        break;

    case SelectState::MultiSelect:
        for(const auto& it : this->selectedItems())
            if(it->parent())
                deleteSMS(it);
        break;

    case SelectState::FilterSelect:
        while(this->selectedItems().first()->childCount() != 0)
            deleteSMS(this->selectedItems().first()->child(0));
        break;

    default:
        break;
    }
}

void SMSLister::onItemSelect()
{
    int selectedCount = this->selectedItems().count();
    if(selectedCount > 1)
    {
        SelectionType = SelectState::MultiSelect;
        QList<SMS*> slist;
        for(const auto& it:this->selectedItems())
        {
            if(it->parent() != nullptr)
                slist.push_back(it->data(0, Qt::UserRole).value<SMS*>());
        }

        if(slist.count() >= 1)      /*Avoid crash in case two categories selected*/
            multiSelected(slist);
    }
    else if(selectedCount == 1)
    {
        QTreeWidgetItem *it = this->selectedItems().first();
        if(!it->parent())
        {
            SelectionType = SelectState::FilterSelect;
            filterSelected(SMSFilter(this->indexOfTopLevelItem(it)));
        }
        else
        {
            SelectionType = SelectState::SingleSelect;
            SMS *s = it->data(0, Qt::UserRole).value<SMS*>();
            if(s->category == Inbox)
                setRead(it);
            smsSelected(s);
        }
    }
    else
    {
        SelectionType = SelectState::None;
    }
}
void SMSLister::onSMSReceive(QString dbusPath)
{
    addSMS(dbusPath);
}
void SMSLister::onSMSMove(QString dbusPath, QTreeWidgetItem *ref)
{
    // Cleanup old
    delete smsObject(ref);
    ref->parent()->removeChild(ref);

    // Setup new
    addSMS(dbusPath);

    // Notify in case Inbox
    SMS s(dbusPath);
    if(s.category == SMSFilter::Inbox)
    {
        Notify("New SMS Received from " + s.getString("Number"), s.getString("Text"));
    }
}
