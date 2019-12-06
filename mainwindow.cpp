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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "composer.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->ui->cbStorageMT->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->ui->cbStorageMT->setFocusPolicy(Qt::NoFocus);
    this->ui->cbStorageSim->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->ui->cbStorageSim->setFocusPolicy(Qt::NoFocus);
    ui->rbProp_3gpp->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->rbProp_cdma->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->categories->setColumnHidden(1,true);

    listModems();
    selectModem(ui->modemSelector->count()-1);
    connect(ui->modemSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(selectModem(int)));
    sysBusConnect("/org/freedesktop/ModemManager1", "org.freedesktop.DBus.ObjectManager", "InterfacesAdded", SLOT(addModem(const QDBusObjectPath&)));
    sysBusConnect("/org/freedesktop/ModemManager1", "org.freedesktop.DBus.ObjectManager", "InterfacesRemoved", SLOT(removeModem(const QDBusObjectPath&)));

    connect(ui->categories, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(selectSMS()));

    connect(ui->buttonDel, SIGNAL(clicked()), this, SLOT(onDeleteClicked()));
    connect(ui->buttonReply, SIGNAL(clicked()), this, SLOT(onReplyClicked()));
    connect(ui->buttonSend, SIGNAL(clicked()), this, SLOT(onSendClicked()));
    connect(ui->buttonNew, SIGNAL(clicked()), this, SLOT(onNewClicked()));

    ico = new QIcon(":/icon.png");
    sysIco = new QSystemTrayIcon(*ico);
    sysIco->show();
}

MainWindow::~MainWindow()
{
    delete ui;
    exit(0);
}


void MainWindow::listModems()
{
    Keys k = Keys({"-L"});
    QStringList modems = k.getStack("modem-list");
    for(const auto& it:modems)
    {
        ui->modemSelector->addItem(Keys({"-m", it}).get("modem.generic.model"), it);
    }
}
void MainWindow::selectModem(int index)
{
    if(sender() != ui->modemSelector)
        ui->modemSelector->setCurrentIndex(index);

    if(index == 0)
    {
        clearInfo();
        clearSMS();
    }
    else
    {
        updateInterfaces();
        updateInfo();
        updateSMS();
        updateConnections();
    }
}
void MainWindow::clearInfo()
{
    QList<QLabel*> labels = ui->tab_modem->findChildren<QLabel*>(QRegularExpression("lblProp_.*"));
    for(const auto& l:labels)
    {
        l->setText("None");
    }
}
void MainWindow::clearSMS()
{
    QList<QTreeWidgetItem*> itms = ui->categories->topLevelItem(0)->takeChildren();
    for(const auto& it:itms)
    {
        sysBusDisconnect(it->text(1), "org.freedesktop.DBus.Properties", "PropertiesChanged", SLOT(updateSMS()));
    }
    itms = ui->categories->topLevelItem(1)->takeChildren();
    for(const auto& it:itms)
    {
        sysBusDisconnect(it->text(1), "org.freedesktop.DBus.Properties", "PropertiesChanged", SLOT(updateSMS()));
    }
    itms = ui->categories->topLevelItem(2)->takeChildren();
    for(const auto& it:itms)
    {
        sysBusDisconnect(it->text(1), "org.freedesktop.DBus.Properties", "PropertiesChanged", SLOT(updateSMS()));
    }
    itms = ui->categories->topLevelItem(3)->takeChildren();
    for(const auto& it:itms)
    {
        sysBusDisconnect(it->text(1), "org.freedesktop.DBus.Properties", "PropertiesChanged", SLOT(updateSMS()));
    }

    ui->from->clear();
    ui->cbStorageMT->setCheckState(Qt::Unchecked);
    ui->cbStorageSim->setCheckState(Qt::Unchecked);
    ui->smsc->setText("None");
    ui->smsContent->clear();
}
void MainWindow::updateInterfaces()
{
    if(Modem == nullptr)
        delete  Modem;
    Modem = new QDBusInterface("org.freedesktop.ModemManager1", ui->modemSelector->currentData().value<QString>(), "org.freedesktop.ModemManager1.Modem", QDBusConnection::systemBus());

    if(Messaging == nullptr)
        delete Messaging;
    Messaging = new QDBusInterface("org.freedesktop.ModemManager1", ui->modemSelector->currentData().value<QString>(), "org.freedesktop.ModemManager1.Modem.Messaging", QDBusConnection::systemBus());
}
void MainWindow::updateInfo()
{
    QString op = ui->modemSelector->currentData().value<QString>();
    Keys k = Keys({"-m", op});

    QList<QLabel*> labels = ui->tabWidget->findChildren<QLabel*>(QRegularExpression("lblProp_.*"));
    for(const auto& l:labels)
    {
        QString lkey = l->property("Key").value<QString>();
        bool larrayed = l->property("isKeyArrayed").value<bool>();
        if(lkey != "")
        {
            if(lkey.contains(","))
            {
                QStringList ltexts;
                QStringList lparts = lkey.split(",");
                for(const auto& lp:lparts)
                {
                    QStringList lsp = lp.split(":");
                    if(lsp.count() == 2)
                    {
                        if(larrayed)
                            lsp[1] = k.getStack(lsp[1]).join(", ");
                        else
                            lsp[1] = k.get(lsp[1]);
                        ltexts << lsp.join(": ");
                    }
                    else
                    {
                        if(larrayed)
                            ltexts << k.getStack(lsp[0]).join(", ");
                        else
                            ltexts << k.get(lsp[0]);
                    }
                }
                l->setText(ltexts.join(" | "));
            }
            else
            {
                if(larrayed)
                    l->setText(k.getStack(lkey).join(", "));
                else
                    l->setText(k.get(lkey));
            }
        }
    }

    if(k.get("modem.3gpp.imei") == "--")
        ui->rbProp_cdma->setChecked(true);
    else
        ui->rbProp_3gpp->setChecked(true);

    QDBusObjectPath sim = Modem->property("Sim").value<QDBusObjectPath>();
    if(Sim != nullptr)
        delete Sim;
    Sim = new QDBusInterface("org.freedesktop.ModemManager1", sim.path(), "org.freedesktop.ModemManager1.Sim", QDBusConnection::systemBus());

    ui->lblDProp_imsi->setText(Sim->property("Imsi").value<QString>());
    ui->lblDProp_sim_identifier->setText(Sim->property("SimIdentifier").value<QString>());
}
void MainWindow::updateSMS()
{
    clearSMS();

    QDBusMessage reply = Messaging->call("List");
    const QDBusArgument &dbusArgs = reply.arguments().at( 0 ).value<QDBusArgument>();
    dbusArgs.beginArray();
    while(!dbusArgs.atEnd())
    {
        QDBusObjectPath sms;
        dbusArgs >> sms;
        addSMS(sms.path(), false, true);
        sysBusConnect(sms.path(), "org.freedesktop.DBus.Properties", "PropertiesChanged", SLOT(updateSMS()));
    }
    dbusArgs.endArray();
}
void MainWindow::updateConnections()
{
    if(connectedModem != "")
    {
        sysBusDisconnect(connectedModem, "org.freedesktop.dbus.properties", "PropertiesChanged", SLOT(updateInfo()));
        sysBusDisconnect(connectedModem, "org.freedesktop.ModemManager1.Modem.Messaging", "Added", SLOT(updateSMS()));
    }

    connectedModem = ui->modemSelector->currentData().value<QString>();
    qDebug() << connectedModem;
    sysBusConnect(ui->modemSelector->currentData().value<QString>(), "org.freedesktop.dbus.properties", "PropertiesChanged", SLOT(updateInfo()));
    sysBusConnect(ui->modemSelector->currentData().value<QString>(), "org.freedesktop.ModemManager1.Modem.Messaging", "Added", SLOT(newSMS(const QDBusObjectPath&)));
}
void MainWindow::addModem(const QDBusObjectPath& op)
{
    QString modemModel = Keys({"-m", op.path()}).get("modem.generic.model");
    ui->modemSelector->addItem(modemModel, op.path());
    notify("New Modem", modemModel);
    if(ui->modemSelector->count() == 2)
        selectModem(1);
}
void MainWindow::removeModem(const QDBusObjectPath& op)
{
    int index = ui->modemSelector->findData(op.path());
    notify("Modem Disconnected", ui->modemSelector->itemText(index));
    if(index == ui->modemSelector->currentIndex())
        selectModem(index-1);
    ui->modemSelector->removeItem(index);
}
void MainWindow::selectSMS()
{
    if(!ui->buttonDel->isEnabled())
        ui->buttonDel->setEnabled(true);
    if(ui->buttonReply->isEnabled())
        ui->buttonReply->setEnabled(false);
    if(ui->buttonSend->isEnabled())
        ui->buttonSend->setEnabled(false);

    ui->cbStorageMT->setCheckState(Qt::Unchecked);
    ui->cbStorageSim->setCheckState(Qt::Unchecked);
    ui->smsTime->setTime(ui->smsTime->minimumTime());
    ui->smsDate->setDate(ui->smsDate->minimumDate());
    ui->smsc->clear();

    QList<QTreeWidgetItem*> itms = ui->categories->selectedItems();
    if(itms.count() > 1)
    {
        ui->buttonDel->setText("Delete Multiple ["+QString::number(itms.count())+" Messages]");
    }
    else
    {
        QTreeWidgetItem* it = itms.first();
        if(!it->parent())
        {
            ui->buttonDel->setText("Clear "+it->text(0));
        }
        else
        {
            ui->buttonDel->setText("Delete");

            if(it->parent()->text(0) == "Inbox") ui->buttonReply->setEnabled(true);
            else if(it->parent()->text(0) == "Drafts") ui->buttonSend->setEnabled(true);

            Keys k = Keys({"-s", it->text(1)});
            ui->smsc->setText(k.get("sms.properties.smsc"));
            QString stime = k.get("sms.properties.timestamp");

            QStringList dateTimeZone = {"    -  -  ", "  :  :  ", "00:00"};
            if(stime != "--") dateTimeZone = stime.split(QRegExp("\\T|\\+"));

            QDate date = QDate::fromString(dateTimeZone[0], "yyyy-MM-dd");
            QTime time = QTime::fromString(dateTimeZone[1], "hh:mm:ss");

            // TODO: Put in code for time zone

            ui->smsTime->setTime(time);
            ui->smsDate->setDate(date);
        }
    }

    QStringList nums, texts;
    for(const auto& it:itms)
    {
        if(!it->parent())
            continue;
        Keys k = Keys({"-s", it->text(1)});
        nums << k.get("sms.content.number");
        texts << k.get("sms.content.number") + ":\n\n" + k.get("sms.content.text");

        QString storage = k.get("sms.properties.storage");
        if(storage == "mt")
        {
            ui->cbStorageMT->setCheckState(Qt::Checked);
        }
        else //TODO: Add logic for other storage
        {
            ui->cbStorageSim->setCheckState(Qt::Checked);
        }
    }
    ui->from->setText(nums.join(", "));
    ui->smsContent->setText(texts.join("\n\n----------------\n\n"));
}
void MainWindow::addSMS(QString sms, bool notifyRecieved, bool append)
{
    Keys k = Keys({"-s", sms});

    QTreeWidgetItem* it = new QTreeWidgetItem({k.get("sms.content.number")});
    it->setText(1, sms);

    QString type = k.get("sms.properties.pdu-type");
    QString state = k.get("sms.properties.state");

    QTreeWidgetItem* parent;
    if(type == "deliver" && state == "received")
    {
        parent = ui->categories->topLevelItem(0);
        if(notifyRecieved)
            notify(k.get("sms.content.number"),k.get("sms.content.text"));
    }
    else if(type == "submit" && state == "sent")
        parent = ui->categories->topLevelItem(1);
    else if(type == "submit")
        parent = ui->categories->topLevelItem(2);
    else if(state == "receiving")
        parent = ui->categories->topLevelItem(3);
    else
        parent = ui->categories->topLevelItem(3);

    if(append)
        parent->addChild(it);
    else
        parent->insertChild(0, it);
}
void MainWindow::newSMS(const QDBusObjectPath& op)
{
    QString sms = op.path();
    addSMS(sms);
    sysBusConnect(sms, "org.freedesktop.DBus.Properties", "PropertiesChanged", SLOT(smsChanged(QString,QDBusMessage)));
}
void MainWindow::smsChanged(QString s, QDBusMessage msg)
{
    QString sms = msg.path();
    QList<QTreeWidgetItem*> itms = ui->categories->findItems(sms, Qt::MatchContains|Qt::MatchRecursive, 1);
    if(itms.count() != 0)
    {
        itms.first()->parent()->removeChild(itms.first());
    }
    addSMS(sms);
}
void MainWindow::onDeleteClicked()
{
    if(ui->categories->selectedItems().count() == 1 && !ui->categories->selectedItems().first()->parent())
    {
        QTreeWidgetItem* parent = ui->categories->selectedItems().first();
        while(parent->childCount() != 0)
        {
            Keys({"-m", ui->modemSelector->currentData().value<QString>(), "--messaging-delete-sms="+parent->child(0)->text(1)});
            parent->removeChild(parent->child(0));
        }
    }
    else
    {
        QList<QTreeWidgetItem*> itms = ui->categories->selectedItems();
        for(const auto& it:itms)
        {
            if(!it->parent())
                return;
            Keys({"-m", ui->modemSelector->currentData().value<QString>(), "--messaging-delete-sms="+it->text(1)});
            it->parent()->removeChild(it);
        }
    }
}
void MainWindow::onReplyClicked()
{
    composer* cw = new composer();
    connect(cw, SIGNAL(sendSMS(QString,QString)), this, SLOT(sendSMS(QString, QString)));
    connect(cw, SIGNAL(saveSMS(QString,QString)), this, SLOT(saveSMS(QString, QString)));
    cw->setNumber(ui->categories->selectedItems().first()->text(0));
    cw->focusMessage();
    cw->show();
}
void MainWindow::onSendClicked()
{
    Keys({"-s", ui->categories->selectedItems().first()->text(1), "--send"});
}
void MainWindow::onNewClicked()
{
    composer* cw = new composer();
    connect(cw, SIGNAL(sendSMS(QString,QString)), this, SLOT(sendSMS(QString, QString)));
    connect(cw, SIGNAL(saveSMS(QString,QString)), this, SLOT(saveSMS(QString, QString)));
    cw->show();
}
void MainWindow::sendSMS(QString number, QString text)
{
    QStringList args;
    args << "-m";
    args << ui->modemSelector->currentData().value<QString>();
    args << "--messaging-create-sms=""text='" + text + "',number='" + number + "'""";
    QString draft = Keys(args).get("Successfully created new SMS");
    Keys({"-s", draft, "--send"});
}
void MainWindow::saveSMS(QString number, QString text)
{
    QStringList args;
    args << "-m";
    args << ui->modemSelector->currentData().value<QString>();
    args << "--messaging-create-sms=""text='" + text + "',number='" + number + "'""";
    Keys(args).get("Successfully created new SMS");
}
void MainWindow::notify(QString title, QString msg)
{
    sysIco->showMessage(title, msg, *ico);
}
