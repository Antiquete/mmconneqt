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

    void hide();
    void setInt(int val);
    void setString(QString val);
    void setStringList(QStringList val);
    void demarshall(QDBusArgument arg);
    void demarshall_ao(const QDBusArgument &arg);
    void demarshall_a_s_su(const QDBusArgument &arg);
    void demarshall_s_uu(const QDBusArgument &arg);
    void demarshall_s_ub(const QDBusArgument &arg);

public:
    Propui(QWidget *parent = nullptr) {}

public slots:
    void set(Modem* m);
};

#endif // PROPUI_H
