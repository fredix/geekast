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

#include "core.h"



QString System::MacOS()
{   
    #ifdef Q_WS_MAC
    m_os_type = "osx";
    m_os_base = "unix";
    switch(QSysInfo::macVersion())
    {
    case QSysInfo::MV_SNOWLEOPARD: return "Mac OS X SNOW LEOPARD";
    case QSysInfo::MV_LEOPARD: return "Mac OS X LEOPARD";
    case QSysInfo::MV_TIGER: return "Mac OS X TIGER";
    case QSysInfo::MV_PANTHER: return "Mac OS X PANTHER";
    default: return "Mac OS X";
    }
    #endif
}


