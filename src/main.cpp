/****************************************************************************
** Copyright (C) 2010-2011 Frédéric Logier
** Contact: Frédéric Logier <frederic@logier.org>
**
** https://github.com/nodecast/geekast
**
** This file is part of geekast.
**
** Geekast is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Geekast is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Geekast.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <QtGui>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("nodecast");
    QCoreApplication::setOrganizationDomain("nodecast.net");
    QCoreApplication::setApplicationName("geekast");


    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Geekast"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    //char * client_locale = setlocale(LC_NUMERIC,"");
    setlocale(LC_NUMERIC,"C");

    MainWindow w;
    w.show();
    return a.exec();

    //setlocale(LC_NUMERIC, client_locale);
}


