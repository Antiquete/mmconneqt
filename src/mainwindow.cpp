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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    this->initSysTray();
    ui->setupUi(this);

    // Connections: ModemLister -> MainWindow
    connect(ui->modemSelector, SIGNAL(modemLoaded(Modem*)), this, SLOT(infoUpdate(Modem*)));
    connect(ui->modemSelector, SIGNAL(modemUnloaded()),     this, SLOT(infoClear()));

    // Connections: ModemLister -> SMSLister
    connect(ui->modemSelector, SIGNAL(modemLoaded(Modem*)), ui->smsSelecter, SLOT(setModem(Modem*)));
    connect(ui->modemSelector, SIGNAL(modemUnloaded()),     ui->smsSelecter, SLOT(clearMessages()));

    // Connections: SMSLister -> MainWindow
    connect(ui->smsSelecter, SIGNAL(smsSelected(SMS*)),             this, SLOT(smsLoad(SMS*)));
    connect(ui->smsSelecter, SIGNAL(multiSelected(QList<SMS*>)),    this, SLOT(smsMultiLoad(QList<SMS*>)));
    connect(ui->smsSelecter, SIGNAL(filterSelected(SMSFilter)),     this, SLOT(clearViewer()));
    connect(ui->smsSelecter, SIGNAL(smsSelected(SMS*)),             this, SLOT(onSMSSingleSelect(SMS*)));
    connect(ui->smsSelecter, SIGNAL(multiSelected(QList<SMS*>)),    this, SLOT(onSMSMultiSelect(QList<SMS*>)));
    connect(ui->smsSelecter, SIGNAL(filterSelected(SMSFilter)),     this, SLOT(onSMSFilterSelect(SMSFilter)));

    // Connections: Composer -> SMSLister
    connect(cw, SIGNAL(sendSMS(QString,QString)), ui->smsSelecter, SLOT(saveSendSMS(QString, QString)));
    connect(cw, SIGNAL(saveSMS(QString,QString)), ui->smsSelecter, SLOT(saveSMS(QString, QString)));

    // Connections: Buttons
    connect(ui->buttonNew,      SIGNAL(clicked()), cw,              SLOT(show()));
    connect(ui->buttonSend,     SIGNAL(clicked()), this,            SLOT(onSendClicked()));
    connect(ui->buttonReply,    SIGNAL(clicked()), this,            SLOT(onReplyClicked()));
    connect(ui->buttonResend,   SIGNAL(clicked()), this,            SLOT(onResendClicked()));
    connect(ui->buttonDel,      SIGNAL(clicked()), ui->smsSelecter, SLOT(deleteSelection()));
    connect(ui->buttonAutoDel,  SIGNAL(clicked()), this,            SLOT(onAutoDelClicked()));
    connect(ui->buttonAutoDel,  SIGNAL(clicked()), aw,              SLOT(update()));
    connect(ui->buttonView,     SIGNAL(clicked()), aw,              SLOT(showUpdated()));

    // Select a modem
    ui->modemSelector->setDefaultModem();
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::initSysTray()
{
    sysTray = new QSystemTrayIcon(QIcon(ICON_FILE));

    connect(sysTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(show()));

    sysContext = new QMenu();
    sysContext->addAction("Modem Viewer", this, SLOT(openModemViewer()));
    sysContext->addAction("Message Viewer", this, SLOT(openMessageViewer()));
    sysContext->addSeparator();
    // sysContext->addAction("Notifications");
    // sysContext->addAction("Settings");
    sysContext->addSeparator();
    sysContext->addAction("Exit", this, SLOT(quit()));
    sysTray->setContextMenu(sysContext);

    sysTray->show();
}

void MainWindow::initUi()
{
    this->ui->cbStorageME->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->ui->cbStorageME->setFocusPolicy(Qt::NoFocus);
    this->ui->cbStorageSM->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->ui->cbStorageSM->setFocusPolicy(Qt::NoFocus);
    ui->rbProp_3gpp->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->rbProp_cdma->setAttribute(Qt::WA_TransparentForMouseEvents);
}

// Tab Info
void MainWindow::infoUpdate(Modem *m)
{
    QList<Propui*> labels = ui->tab_info->findChildren<Propui*>();
    for(const auto& lbl:labels)
    {
        lbl->set(m);
    }

    if(!m->interface(Mt::Modem3GPP)->isValid())
        ui->rbProp_cdma->setChecked(true);
    else
        ui->rbProp_3gpp->setChecked(true);
}
void MainWindow::infoClear()
{
    QList<Propui*> props = ui->tab_info->findChildren<Propui*>();
    for(const auto& prop:props)
    {
        prop->setText("None");
    }
}

// Tab Messaging
void MainWindow::smsLoad(SMS *s)
{
    clearViewer();

    ui->from->setText(s->get("Number").value<QString>());
    ui->smsContent->setText(s->get("Text").value<QString>());

    /******** Storage Specification *******
    * From Modem Manager Page
    * https://www.freedesktop.org/software/ModemManager/api/1.0.0/ModemManager-Flags-and-Enumerations.html#MMSmsStorage
    ***************************************
    * 0 MM_SMS_STORAGE_UNKNOWN   - Storage unknown.
    * 1 MM_SMS_STORAGE_SM        - SIM card storage area.
    * 2 MM_SMS_STORAGE_ME        - Mobile equipment storage area.
    * 3 MM_SMS_STORAGE_MT        - Sum of SIM and Mobile equipment storages
    * 4 MM_SMS_STORAGE_SR        - Status report message storage area.
    * 5 MM_SMS_STORAGE_BM        - Broadcast message storage area.
    * 6 MM_SMS_STORAGE_TA        - Terminal adaptor message storage area.
    ***************************************/
    switch (s->get("Storage").value<unsigned int>())
    {
        case 1: ui->cbStorageME->setChecked(false);
                ui->cbStorageSM->setChecked(true);
                break;

        case 2: ui->cbStorageME->setChecked(true);
                ui->cbStorageSM->setChecked(false);
                break;

        case 3: ui->cbStorageME->setChecked(true);
                ui->cbStorageSM->setChecked(true);
                break;

        default: ui->cbStorageME->setChecked(false);
                 ui->cbStorageSM->setChecked(false);
                 break;
    }

    ui->smsc->setText(s->get("SMSC").value<QString>());

    QDateTime dateTime = QDateTime::fromString(s->get("Timestamp").value<QString>(),
                                               Qt::ISODate);
    ui->smsTime->setTime(dateTime.time());
    ui->smsDate->setDate(dateTime.date());
}
void MainWindow::smsMultiLoad(QList<SMS*> smslist){
    QStringList from;
    QStringList smsContents;

    QDateTime min;
    QDateTime max = min = QDateTime::fromString(smslist.first()->get("Timestamp").value<QString>(),
                                                Qt::ISODate);

    QStringList smscenters;

    for(const auto& s:smslist)
    {
        from << s->get("Number").value<QString>();
        smsContents << from.last() + ":\n\n" + s->get("Text").value<QString>();

        QDateTime dateTime = QDateTime::fromString(s->get("Timestamp").value<QString>(),
                                                   Qt::ISODate);
        if(dateTime < min)
            min = dateTime;

        if(dateTime > max)
            max = dateTime;

        QString smsc = s->get("SMSC").value<QString>();
        if(!smscenters.contains(smsc))
            smscenters << smsc;
    }

    ui->from->setText(from.join(","));
    ui->smsContent->setText(smsContents.join("\n\n----------------\n\n"));

    ui->smsc->setText("Multiple");
    ui->smsc->setToolTip(smscenters.join(","));

    ui->cbStorageME->setChecked(false);
    ui->cbStorageSM->setChecked(false);

    ui->smsTime->setTime(max.time());
    ui->smsDate->setDate(max.date());
    ui->smsTime->setToolTip("From: " + min.time().toString() + " - To: " + max.time().toString());
    ui->smsDate->setToolTip("From: " + min.date().toString() + " - To: " + max.date().toString());
}
void MainWindow::clearViewer(){
    ui->from->setText("");
    ui->smsContent->setText("");
    ui->smsc->setText("None");
    ui->cbStorageME->setChecked(false);
    ui->cbStorageSM->setChecked(false);

    ui->buttonAutoDel ->  setVisible(false);

    ui->smsTime->setTime(QTime::currentTime());
    ui->smsDate->setDate(QDate::currentDate());
    ui->smsTime->setToolTip("Time of Message Arrival.");
    ui->smsDate->setToolTip("Date of Message Arrival.");
}
void MainWindow::onSMSFilterSelect(SMSFilter sfilter)
{
    ui->buttonAutoDel ->  setVisible(false);
    ui->buttonSend    ->  setVisible(false);
    ui->buttonReply   ->  setVisible(false);
    ui->buttonResend  ->  setVisible(false);

    ui->buttonDel     ->  setText("Clear " + ui->smsSelecter->category(sfilter));
}
void MainWindow::onSMSMultiSelect(QList<SMS*> slist)
{
    ui->buttonAutoDel ->  setVisible(false);
    ui->buttonSend    ->  setVisible(false);
    ui->buttonReply   ->  setVisible(false);
    ui->buttonResend  ->  setVisible(false);

    ui->buttonDel     ->  setText("Delete Multiple [" + QString::number(slist.count()) + " Messages]");
}
void MainWindow::onSMSSingleSelect(SMS *s)
{
    ui->buttonAutoDel ->  setVisible(false);
    ui->buttonDel     ->  setText("Delete");

    switch (s->category)
    {
    case SMSFilter::Inbox:
        ui->buttonSend   ->  setVisible(false);
        ui->buttonReply  ->  setVisible(true);
        ui->buttonResend ->  setVisible(false);

        if(!getAutoDeletes().contains(s->get("Number").value<QString>()))
            ui->buttonAutoDel->setVisible(true);

        break;

    case SMSFilter::Sent:
        ui->buttonSend   ->  setVisible(false);
        ui->buttonReply  ->  setVisible(false);
        ui->buttonResend ->  setVisible(true);
        break;

    case SMSFilter::Drafts:
        ui->buttonSend   ->  setVisible(true);
        ui->buttonReply  ->  setVisible(false);
        ui->buttonResend ->  setVisible(false);
        break;

    default:
        ui->buttonSend   ->  setVisible(false);
        ui->buttonReply  ->  setVisible(false);
        ui->buttonResend ->  setVisible(false);
        break;
    }
}

// Button Slots
void MainWindow::onSendClicked()
{
    ui->smsSelecter->selectedSMS()->send();
}
void MainWindow::onReplyClicked()
{
    cw->setNumber(ui->smsSelecter->selectedItems().first()->text(0));
    cw->focusMessage();
    cw->show();
}
void MainWindow::onResendClicked()
{
    cw->setNumber(ui->smsSelecter->selectedSMS()->get("Number").value<QString>());
    cw->setMessage(ui->smsSelecter->selectedSMS()->get("Text").value<QString>());
    cw->focusMessage();
    cw->show();
}
void MainWindow::onAutoDelClicked()
{
    QString num = ui->smsSelecter->selectedSMS()->get("Number").value<QString>();
    addAutoDelete(num);
    QList<QTreeWidgetItem*> itms = ui->smsSelecter->findItems(num, Qt::MatchRecursive);
    ui->smsSelecter->deleteListed(itms);
    clearViewer();
}

// Notification
void MainWindow::setStatus(QString text, int timeout)
{
    stub("MainWindow::setStatus");
}

// SysTray
void MainWindow::openModemViewer()
{
    this->ui->tabWidget->setCurrentIndex(0);
    this->show();
}
void MainWindow::openMessageViewer()
{
    this->ui->tabWidget->setCurrentIndex(1);
    this->show();
}
void MainWindow::quit()
{
    delete sysContext;
    delete sysTray;
    this->quit();
}
