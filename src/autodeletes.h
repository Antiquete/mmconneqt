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

#ifndef AUTODELETES_H
#define AUTODELETES_H

#include <QDialog>

#include "core.h"

namespace Ui {
class Autodeletes;
}

class Autodeletes : public QDialog
{
    Q_OBJECT

public:
    explicit Autodeletes(QWidget *parent = nullptr);
    ~Autodeletes();

public slots:
    void update();
    void showUpdated();

private:
    Ui::Autodeletes *ui;

private slots:
    void removeSelected();
};

#endif // AUTODELETES_H
