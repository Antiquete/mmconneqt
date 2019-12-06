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

#include "composer.h"
#include "ui_composer.h"

void composer::process(QAbstractButton* b)
{
    if(b == gui->buttonBox->button(QDialogButtonBox::Ok))
        emit sendSMS(gui->number->text(), gui->message->document()->toPlainText());
    else if(b == gui->buttonBox->button(QDialogButtonBox::Save))
        emit saveSMS(gui->number->text(), gui->message->document()->toPlainText());

    this->destroy();
}

composer::composer(QWidget *parent) : QDialog(parent), gui(new Ui::composer)
{
    gui->setupUi(this);
    gui->buttonBox->button(QDialogButtonBox::Ok)->setText("Send");

    connect(gui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(process(QAbstractButton*)));
}

composer::~composer()
{
    delete gui;
}

void composer::setNumber(QString num)
{
    gui->number->setText(num);
}

void composer::focusMessage()
{
    gui->message->setFocus();
}
