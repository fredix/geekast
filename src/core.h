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

#ifndef CORE_H
#define CORE_H

#include <QTime>
#include <QTest>
#include <QSysInfo>
#include <QString>
#include <QDebug>
#include <QDialog>
#include <QUuid>
#include <QStack>
#include <QFile>
#include <QxtJSON>


extern "C" {
#include "sigar.h"
#include "sigar_format.h"
}

//#include <iostream>


#ifdef WIN32
#define EOL "\r\n"
#else
#define EOL "\n"
#endif



QT_BEGIN_NAMESPACE
class QSysInfo;
//class QString;
QT_END_NAMESPACE


 enum dataName {
  name,
  version,
  arch,
  machine,
  description,
  patch_level,
  vendor,
  vendor_version,
  vendor_name,
  vendor_code_name
};

typedef QMap<QString, dataName> StringToEnumMap;
typedef long unsigned int Lu;         // shortcut for printf format
typedef long long unsigned int Llu;   // ditto
typedef long long int Lld;            // ditto
const int DEFAULT_PAUSE = 500;

const double KiB = 1024.;
const double MiB = KiB * 1024;
const double GiB = MiB * 1024;


const int DEFAULT_NET_STAT_FLAGS = SIGAR_NETCONN_CLIENT|SIGAR_NETCONN_SERVER|SIGAR_NETCONN_TCP;





//! [0]
class System
{
    // Q_OBJECT


public:
    System();
    ~System();
    QStringList infos();
    void setVisible(bool visible);
    QString os_name;
    QString m_os_type;
    QString m_os_base;

protected:


private:
    StringToEnumMap stringToEnum;
    QString Windows();
    QString Linux();
    QString MacOS();
    QStringList getProcess();

};




class SysInfo
{

public:
    SysInfo();
    ~SysInfo();
    QString name;
    QString version;
    QString arch;
    QString machine;
    QString description;
    QString patch_level;
    QString vendor;
    QString vendor_version;
    QString vendor_name;
    QString vendor_code_name;
};

class LoadAvg
{
public:
    LoadAvg();
    ~LoadAvg();
    QString loadavg0;
    QString loadavg1;
    QString loadavg2;
private:
    System system;
};



class Uptime
{
public:
    Uptime();
    ~Uptime();
    QString time;
    QString days;
};


class Cpu
{
public:
    Cpu();
    ~Cpu();    
    QStringList cpus;
    QString vendor;
    QString model;
    QString mhz;
    QString cache_size;
    QString number;
    QString total_cores;
    QString total_sockets;
    QString cores_per_socket;

};



class Mem
{
public:
    Mem();
    ~Mem();
    QString ram;
    QString total;
    QString used;
    QString free;
    QString actual_free;
    QString actual_used;
    QString actual_free_percent;
    QString actual_used_percent;
};

class Swap
{
public:
    Swap();
    ~Swap();
    QString total;
    QString used;
    QString free;
    QString page_in;
    QString page_out;
};


class NetInfo
{
public:
    NetInfo();
    ~NetInfo();
    QString hostname;
    QString default_gateway;
    QString domain_name;
    QString primary_dns;
    QString secondary_dns;
    QString primary_interface;
    QString primary_addr;
};


class NetStat
{
public:
    NetStat(QString ifname);    
    ~NetStat();
    QString tcp_established;
    QString tcp_listen;
    QString tcp_time_wait;
    QString tcp_close_wait;
    QString tcp_idle;
    QString rx_rate;
    QString tx_rate;
private:
    QTime m_time;
};


class CpuStat
{
public:
    CpuStat();    
    ~CpuStat();
    QString user;
    QString sys;
    QString nice;
    QString idle;
    QString wait;
    QString irq;
    QString soft_irq;
    QString stolen;
    QString combined;
    QString total;

};

class FileSystem
{
public:
    FileSystem();
    ~FileSystem();
    QString dir_name;
    QString devname;
    QString type_name;
    QString sys_type_name;
    QString type;
    QString flags;
    QString total;
    QString free;
    QString used;
    QString avail;
    QString files;
    QString disk_read;
    QString disk_write;
    QString disk_write_byte;
    QString disk_read_byte;
    QString disk_queues;
    QString use_percent;
};




class Process
{
public:
    Process();
    ~Process();

public:
    QString pid;
    QString state_name;
    QString state_state;
    QString state_ppid;
    QString state_tty;
    QString state_priority;
    QString state_nice;
    QString state_processor;
    QString state_threads;
    QString mem_size;
    QString mem_resident;
    QString mem_share;
    QString mem_minor_faults;
    QString mem_major_faults;
    QString mem_page_faults;
    QString time_start_time;
    QString time_user;
    QString time_sys;
    QString time_total;
};


class ProcList
{
public:
    ProcList();
    ~ProcList();

    QStack <Process> stack;
};




class Datas
{

  public:
    Datas();
    ~Datas();
    void Populate(QVariantMap *a_datas);
    bool activated_hardware;
    bool activated_uptime;
    bool activated_memory;
    bool activated_load;
    bool activated_process;
    bool activated_network;
    bool activated_cpu;
    bool activated_fs;
    bool public_host;


  protected:
    QVariantMap *m_root;
    QVariantMap *m_profil;
    QVariantMap *m_public;
    QVariantMap *m_uuid;
    QVariantMap *m_device;

    QVariantMap *m_activated_network;
    QVariantMap *m_activated_uptime;
    QVariantMap *m_activated_load;
    QVariantMap *m_activated_hardware;
    QVariantMap *m_activated_memory;
    QVariantMap *m_activated_cpu;
    QVariantMap *m_activated_process;

    private:
        System system;
        SysInfo *sysinfo;
        LoadAvg *loadavg;
        Uptime *uptime;
        Cpu *cpu;
        CpuStat *cpustat;
        Mem *mem;
        Swap *swap;
        NetInfo *netinfo;
        NetStat *netstat;
        ProcList *proclist;
};


//! [0]


#endif // CORE_H
