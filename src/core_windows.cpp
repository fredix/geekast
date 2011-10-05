/****************************************************************************
** Copyright (C) 2010-2011 Frédéric Logier
** Contact: Frédéric Logier <frederic@logier.org>
**
** https://github.com/nodecast/geekast
**
** This file is part of Geekast.
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

/*
void get_uptime (void) {


PerformanceCounter pc = new PerformanceCounter("System", "System Up Time");

pc.NextValue();

TimeSpan ts = TimeSpan.FromSeconds(pc.NextValue() );

MessageBox.Show ("Uptime: " + ts.Days + " days, " + ts.Hours + " hours, " +

ts.Minutes + " minutes");
}
*/

#include "core.h"




QString System::Windows()
{
    #ifdef Q_WS_WIN    
    m_os_type = "windows";
    m_os_base = "windows";
    switch(QSysInfo::windowsVersion())
    {
    case QSysInfo::WV_WINDOWS7: return "Windows Seven";
    case QSysInfo::WV_VISTA: return "Windows Vista";
    case QSysInfo::WV_2003: return "Windows 2003";
    case QSysInfo::WV_2000: return "Windows 2000";
    case QSysInfo::WV_XP: return "Windows XP";
    default: return "Windows";
    }
    #endif
    return QString("Windows");
}
