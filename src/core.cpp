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

#include "core.h"

QSystemTrayIcon *sysTray = nullptr;
QMenu *sysContext = nullptr;

void stub(QString caller, QString message)
{
    if(message == "")
        message = "Function not implemented.";
    qDebug() << "Stub(" << caller << "):" << message;
}
void Notify(QString title, QString msg)
{
    if(sysTray == nullptr)
    {
        sysTray = new QSystemTrayIcon(QIcon(ICON_FILE));
        sysTray->show();
    }
    sysTray->showMessage(title, msg);
}

// Handle Autodelete Storage
QStringList getAutoDeletes()
{
    return _settingsAutodelete.value("Autodeletes", QStringList()).toStringList();
}
void setAutoDeletes(QStringList autodels)
{
    _settingsAutodelete.setValue("Autodeletes", autodels);
}
void addAutoDelete(QString num)
{
    QStringList autodels = getAutoDeletes();
    autodels << num;
    setAutoDeletes(autodels);
}
void removeAutoDelete(QString num)
{
    QStringList autodels = getAutoDeletes();
    autodels.removeAll(num);
    setAutoDeletes(autodels);
}
