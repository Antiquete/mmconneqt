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

#include "autodeletes.h"
#include "ui_autodeletes.h"

#include "core.h"

Autodeletes::Autodeletes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Autodeletes)
{
    ui->setupUi(this);

    // Connection: Remove Button
    connect(ui->buttonRemove, SIGNAL(clicked()), this, SLOT(removeSelected()));

    // Update List
    update();
}

Autodeletes::~Autodeletes()
{
    delete ui;
}

void Autodeletes::update()
{
    ui->list->clear();

    QStringList autodels = getAutoDeletes();
    for(const auto& autodel:autodels)
        ui->list->addItem(autodel);

    if(ui->list->count() == 0)
        ui->buttonRemove->setEnabled(false);
    else
        ui->buttonRemove->setEnabled(true);
}

void Autodeletes::showUpdated()
{
    update();
    this->show();
}

void Autodeletes::removeSelected()
{
    if(ui->list->count() >= 1)
    {
        removeAutoDelete(
                    ui->list->takeItem(ui->list->currentRow())->text()
                    );

        if(ui->list->count() == 0)
            ui->buttonRemove->setEnabled(false);
    }
}
