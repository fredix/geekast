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

// check for lib sigar usage : http://www.hypertable.org/doxygen/_system_info_8cc_source.html


#include "core.h"

/*
extern "C" {
#include <poll.h>
#include <curses.h>
#include <term.h>
#include <sigar-1.6.3/include/sigar.h>
#include <sigar-1.6.3/include/sigar_format.h>
}
*/



Process::Process()
{

}

Process::~Process()
{

}

/*
 * ProcList CLASS
 */

ProcList::ProcList()
{
    int status;
    sigar_t *sigar;
    sigar_proc_list_t proclist;
    unsigned long i;

    sigar_open(&sigar);
    status = sigar_proc_list_get(sigar, &proclist);

    if (status != SIGAR_OK) {
        printf("get proc list error: %d (%s)\n",
               status, sigar_strerror(sigar, status));
        exit(1);
    }

    for (i=0; i<proclist.number; i++) {
        sigar_pid_t pid = proclist.data[i];        
        sigar_proc_mem_t proc_mem;
        sigar_proc_time_t proc_time;
        sigar_proc_state_t proc_state;


        status = sigar_proc_mem_get(sigar, pid, &proc_mem);
        if (status != SIGAR_OK) {
#ifdef DEBUG
            printf("error: %d (%s) proc_mem(%d)\n",
                   status, sigar_strerror(sigar, status), pid);
#endif
            continue;
        }

        status = sigar_proc_state_get(sigar, pid, &proc_state);
        if (status != SIGAR_OK) {
#ifdef DEBUG
            printf("error: %d (%s) proc_state(%d)\n",
                   status, sigar_strerror(sigar, status), pid);
#endif
            continue;
        }

        status = sigar_proc_time_get(sigar, pid, &proc_time);
        if (status != SIGAR_OK) {
#ifdef DEBUG
            printf("error: %d (%s) proc_time(%d)\n",
                   status, sigar_strerror(sigar, status), pid);
#endif
            continue;
        }

//        printf("%d %s\n", (long)pid, pstate.name);
        //qDebug() << "name " << pstate.name << " processor " << pstate.processor;

        //qDebug()  << lpid.setNum((long)pid);
         // datas << QString("PID : %1 , %2").arg(lpid.setNum((long)pid), pstate.name);

        Process proc;

        //proc = new(Process);

        proc.pid.setNum((long)pid);
        proc.state_name = proc_state.name;
        proc.state_state = proc_state.state;
        proc.state_ppid.setNum(proc_state.ppid);
        //proc.state_tty = proc_state.tty;
        proc.state_priority.setNum(proc_state.priority);
        proc.state_nice.setNum(proc_state.nice);
        proc.state_processor.setNum(proc_state.processor);
        proc.state_threads.setNum(proc_state.threads);
        proc.mem_size.setNum(proc_mem.size);
        proc.mem_resident.setNum(proc_mem.resident);
        proc.mem_share.setNum(proc_mem.share);
        proc.mem_minor_faults.setNum(proc_mem.minor_faults);
        proc.mem_major_faults.setNum(proc_mem.major_faults);
        proc.mem_page_faults.setNum(proc_mem.page_faults);
        proc.time_start_time.setNum(proc_time.start_time);

        qDebug() << "proc_state.name : " << proc_state.name << " : proc_time.start_time : " << proc_time.start_time;

        proc.time_user.setNum(proc_time.user);
        proc.time_sys.setNum(proc_time.sys);
        proc.time_total.setNum(proc_time.total);

        stack.push(proc);
        //delete(proc);

    }
/* TEST OK
    while (!stack.isEmpty())
    {
        qDebug() << "stack length : " << stack.count();
        ok = .pop();
        qDebug() << "name : " << ok->state_name << " " << " pid : " << ok->pid;
        delete(ok);
    }
*/

    sigar_proc_list_destroy(sigar, &proclist);
    sigar_close(sigar);
}



ProcList::~ProcList()
{}





/*
* FileSystem CLASS
*/


FileSystem::FileSystem()
{
    int status;
    sigar_t *sigar;
    sigar_file_system_list_t fslist;

    sigar_open(&sigar);
    status = sigar_file_system_list_get(sigar, &fslist);


    if (status != SIGAR_OK) {
        printf("get net info error: %d (%s)\n",
               status, sigar_strerror(sigar, status));
        exit(1);
    }
}

/*
* NetInfo CLASS
*/


NetInfo::NetInfo()
{
    int status;
    sigar_t *sigar;
    sigar_net_info_t ni;
    sigar_net_interface_config_t ifc;
    char addrbuf[SIGAR_INET6_ADDRSTRLEN];

    sigar_open(&sigar);    
    status = sigar_net_info_get(sigar, &ni);    


    if (status != SIGAR_OK) {
        printf("get net info error: %d (%s)\n",
               status, sigar_strerror(sigar, status));
        exit(1);
    }

    hostname = ni.host_name;
    default_gateway = ni.default_gateway;
    domain_name = ni.domain_name;
    primary_dns = ni.primary_dns;
    secondary_dns = ni.secondary_dns;


    status = sigar_net_interface_config_primary_get(sigar, &ifc);
    if (status != SIGAR_OK) {
        printf("get net info error: %d (%s)\n",
               status, sigar_strerror(sigar, status));
        exit(1);
    }


    if (sigar_net_interface_config_primary_get(sigar, &ifc) == SIGAR_OK) {
        primary_interface = ifc.name;
         sigar_net_address_to_string(sigar, &ifc.address, addrbuf);
         primary_addr = addrbuf;
       }
       if (primary_addr.isEmpty() )
         primary_addr = "127.0.0.1";

    qDebug () << ifc.name << " " << ifc.type << " " << primary_addr;
    sigar_close(sigar);
}



NetInfo::~NetInfo()
{}

/*
* NetStat CLASS
*/





NetStat::NetStat(QString ifname)
{    
    int status;
    QString tmp;
    sigar_t *sigar;
    sigar_net_stat_t s;
    sigar_net_interface_stat_t curr, curr2;

    sigar_open(&sigar);
    status = sigar_net_stat_get(sigar, &s, DEFAULT_NET_STAT_FLAGS);


    if (status != SIGAR_OK) {
        printf("get net start info error: %d (%s)\n",
               status, sigar_strerror(sigar, status));
        exit(1);
    }

/*
    s.all_inbound_total;
    s.all_outbound_total;
    s.tcp_inbound_total;
    s.tcp_outbound_total;
    s.tcp_states;*/



    tcp_established = s.tcp_states[SIGAR_TCP_ESTABLISHED];
    tcp_listen = s.tcp_states[SIGAR_TCP_LISTEN];
    tcp_time_wait = s.tcp_states[SIGAR_TCP_TIME_WAIT];
    tcp_close_wait = s.tcp_states[SIGAR_TCP_CLOSE_WAIT];
    tcp_idle = s.tcp_states[SIGAR_TCP_IDLE];


    status = sigar_net_interface_stat_get(sigar, ifname.toAscii(), &curr);
    if (status != SIGAR_OK) {
        printf("get stat net info error: %d (%s)\n",
               status, sigar_strerror(sigar, status));
        exit(1);
    }

    // printf("curr_net_stat: rx_bytes=%llu, tx_bytes=%llu \n", (Llu)curr.rx_bytes, (Llu)curr.tx_bytes);


    m_time.start();
    //poll(0, 0, DEFAULT_PAUSE);
     QTest::qSleep(1000);
    status = sigar_net_interface_stat_get(sigar, ifname.toAscii(), &curr2);
    if (status != SIGAR_OK) {
        printf("get stat net info error: %d (%s)\n",
               status, sigar_strerror(sigar, status));
        exit(1);
    }
    int elapsed = m_time.elapsed();


    //rx_bytes.setNum(curr.rx_bytes, 10);
    //tx_bytes.setNum(curr.tx_bytes, 10);

    //double tmp = (curr2.rx_bytes - curr.rx_bytes) / KiB;

    //qDebug() << "rx tmp " << tmp;

    rx_rate.setNum(((curr2.rx_bytes - curr.rx_bytes) / KiB), 'g');

    //rx_rate = (curr2.rx_bytes - curr.rx_bytes) / KiB;

    //tmp = "";


    //tmp = (curr2.tx_bytes - curr.tx_bytes) / KiB;
    //qDebug() << "tx tmp " << tmp;
    tx_rate.setNum(((curr2.tx_bytes - curr.tx_bytes) / KiB), 'g');

    //tx_rate = (curr2.tx_bytes - curr.tx_bytes) / KiB;

    //tmp = "";


    qDebug() << "tx2 rate : " << curr2.tx_bytes << " tx rate : " << curr.tx_bytes << " rate : " << (curr2.tx_bytes - curr.tx_bytes) / KiB;

    sigar_close(sigar);
}


NetStat::~NetStat()
{

}


/*
* Mem CLASS
*/


Mem::Mem()
{
    int status;
    sigar_t *sigar;
    sigar_mem_t mem;

    sigar_open(&sigar);
    status = sigar_mem_get(sigar, &mem);


    if (status != SIGAR_OK) {
        printf("get mem info error: %d (%s)\n",
               status, sigar_strerror(sigar, status));
        exit(1);
    }

    ram.setNum(mem.ram);
    total.setNum(mem.total / MiB, 'g');
    used.setNum(mem.used / MiB, 'g');
    free.setNum(mem.free / MiB, 'g');
    actual_free.setNum(mem.actual_free / MiB, 'g');
    actual_used.setNum(mem.actual_used / MiB, 'g');
    actual_free_percent.setNum(mem.free_percent, 'g');
    actual_used_percent.setNum(mem.used_percent, 'g');


    qDebug() << "mem.actual_used : " << mem.actual_used;
    qDebug() << "mem.used_percent : " << mem.used_percent;
    qDebug() << "actual_used_percent : " << actual_used_percent;

    sigar_close(sigar);
}

Mem::~Mem()
{

}

Swap::Swap()
{
    int status;
    sigar_t *sigar;
    sigar_swap_t swap;

    sigar_open(&sigar);
    status = sigar_swap_get(sigar, &swap);

    if (status != SIGAR_OK) {
        printf("get swap info error: %d (%s)\n",
               status, sigar_strerror(sigar, status));
        exit(1);
    }

    total.setNum(swap.total / MiB, 'g');
    used.setNum(swap.used / MiB, 'g');
    free.setNum(swap.free / MiB, 'g');
    page_in.setNum(swap.page_in);
    page_out.setNum(swap.page_out);

    sigar_close(sigar);
}



Swap::~Swap()
{

}

/*
* SysInfo CLASS
*/


SysInfo::SysInfo()
{
    int status;
    sigar_t *sigar;
    sigar_sys_info_t sysinfo;

    sigar_open(&sigar);
    status = sigar_sys_info_get(sigar, &sysinfo);


    if (status != SIGAR_OK) {
        printf("get sys info error: %d (%s)\n",
               status, sigar_strerror(sigar, status));
        exit(1);
    }

    name = sysinfo.name;
    version = sysinfo.version;
    arch = sysinfo.arch;
    machine = sysinfo.machine;
    description = sysinfo.description;
    patch_level = sysinfo.patch_level;
    vendor = sysinfo.vendor;
    vendor_version = sysinfo.vendor_version;
    vendor_name = sysinfo.vendor_name;
    vendor_code_name = sysinfo.vendor_code_name;

    sigar_close(sigar);
}


SysInfo::~SysInfo()
{

}


/*
* LoadAvg CLASS
*/


LoadAvg::LoadAvg()
{
    // loadavg doest not exist on windows. Test to not segfault
    if (system.os_name.indexOf("windows", 0, Qt::CaseInsensitive) == -1) {
        QString load;
        int status;
        sigar_t *sigar;
        sigar_loadavg_t loadavg;

        sigar_open(&sigar);
        status = sigar_loadavg_get(sigar, &loadavg);

        if (status != SIGAR_OK) {
            printf("get load avg : %d (%s)\n",
                   status, sigar_strerror(sigar, status));
            exit(1);
        }

        qDebug() << "loadavg.loadavg[0] : " << loadavg.loadavg[0];
        qDebug() << "loadavg.loadavg[1] : " << loadavg.loadavg[1];
        qDebug() << "loadavg.loadavg[2] : " << loadavg.loadavg[2];


/*
        int i_loadavg0 = loadavg.loadavg[0] * 100.00 + 0.5;
        int i_loadavg1 = loadavg.loadavg[1] * 100.00 + 0.5;
        int i_loadavg2 = loadavg.loadavg[2] * 100.00 + 0.5;


        loadavg0 = load.setNum(i_loadavg0);
        loadavg1 = load.setNum(i_loadavg1);
        loadavg2 = load.setNum(i_loadavg2);
*/

        loadavg0 = load.setNum(loadavg.loadavg[0], 'g');
        loadavg1 = load.setNum(loadavg.loadavg[1], 'g');
        loadavg2 = load.setNum(loadavg.loadavg[2], 'g');




        sigar_close(sigar);
    }
}

LoadAvg::~LoadAvg()
{
}



/*
* Uptime CLASS
*/


Uptime::Uptime() {
    QString cpu;
    int status;
    char b[256];
    sigar_t *sigar;
    sigar_uptime_t uptime;

    sigar_open(&sigar);
    status = sigar_uptime_get(sigar, &uptime);
    status = sigar_uptime_string(sigar, &uptime, b, sizeof(b));


    if (status != SIGAR_OK) {
        printf("get uptime error: %d (%s)\n",
               status, sigar_strerror(sigar, status));
        exit(1);
    }

    this->time = cpu.setNum(uptime.uptime, 'g');
    //this->time = uptime.uptime;
    this->days = b;


    qDebug() << "uptime.uptime : " << uptime.uptime;
    qDebug() << "time : " << this->time;

    //printf("uptime time %f\n", uptime.uptime);

    //printf("uptime days = %s time %f\n", b, uptime.uptime);


    sigar_close(sigar);
}



Uptime::~Uptime()
{

}


/*
* Cpu CLASS
*/



Cpu::Cpu() {    
    int status, i;
    sigar_t *sigar;
    sigar_cpu_info_list_t cpulist;

    sigar_open(&sigar);
    status = sigar_cpu_info_list_get(sigar, &cpulist);

    if (status != SIGAR_OK) {
        printf("cpu_list error: %d (%s)\n",
               status, sigar_strerror(sigar, status));
        exit(1);
    }

    /*
    for (i=0; i<cpulist.number; i++) {
        sigar_cpu_info_t cpu = cpulist.data[i];
        cpus << cpu.vendor;
        cpus << cpu.model;
        cpus << cpu.mhz;
        cpus << cpu.cache_size;
    }*/

    sigar_cpu_info_t cpu = cpulist.data[0];

    vendor = cpu.vendor;
    model = cpu.model;
    mhz.setNum(cpu.mhz);
    cache_size.setNum(cpu.cache_size);
    number.setNum(cpulist.number);
    total_cores.setNum(cpu.total_cores);
    total_sockets.setNum(cpu.total_sockets);
    cores_per_socket.setNum(cpu.cores_per_socket);


    sigar_cpu_info_list_destroy(sigar, &cpulist);
    sigar_close(sigar);
}


Cpu::~Cpu()
{

}


CpuStat::CpuStat()
{
    int status, i;
    sigar_t *sigar;
    sigar_cpu_t _prev_cpu, cpu;
    sigar_cpu_perc_t cpu_perc;

    sigar_open(&sigar);
    status = sigar_cpu_get(sigar, &_prev_cpu);

    if (status != SIGAR_OK) {
        printf("cpu stat error: %d (%s)\n",
               status, sigar_strerror(sigar, status));
        exit(1);
    }

    QTest::qSleep(1000);

    status = sigar_cpu_get(sigar, &cpu);

    if (status != SIGAR_OK) {
        printf("cpu stat error: %d (%s)\n",
               status, sigar_strerror(sigar, status));
        exit(1);
    }

    sigar_cpu_perc_calculate(&_prev_cpu, &cpu, &cpu_perc);



    user.setNum(cpu_perc.user * 100.);
    sys.setNum(cpu_perc.sys * 100.);
    nice.setNum(cpu_perc.nice * 100.);
    idle.setNum(cpu_perc.idle * 100.);
    wait.setNum(cpu_perc.wait * 100.);
    irq.setNum(cpu_perc.irq * 100.);
    soft_irq.setNum(cpu_perc.soft_irq * 100.);
    stolen.setNum(cpu_perc.stolen * 100.);
    combined.setNum(cpu_perc.combined * 100.);

    total.setNum(cpu.total);

    sigar_close(sigar);
}


CpuStat::~CpuStat()
{

}


/*
* SYSTEM CLASS
*/


System::System()
{    
    os_name = "unknown OS";

    qDebug() << QUuid::createUuid ();


    #ifdef Q_WS_X11
    os_name = System::Linux();
    #endif

    #ifdef Q_WS_MAC
    os_name = System::MacOS();

    #endif

    #ifdef Q_WS_QWS
    os_name = "Embedded Linux";
    #endif

    #ifdef Q_WS_WIN
    os_name = System::Windows();
    #endif

}



System::~System()
{

}






/*
* DATAS CLASS
*/


Datas::Datas()
{
    sysinfo = new SysInfo();
    loadavg = NULL;
    uptime = NULL;
    cpu = NULL;
    cpustat = NULL;
    mem = NULL;
    swap = NULL;
    netinfo = NULL;
    netstat = NULL;
    proclist = NULL;


    m_doc = new QDomDocument;
    m_root = new QDomElement(m_doc->createElement("host"));
    m_profil = new QDomElement(m_doc->createElement("profil"));
    m_public = new QDomElement(m_doc->createElement("public"));
    m_uuid = new QDomElement(m_doc->createElement("uuid"));
    m_device = new QDomElement(m_doc->createElement("device"));


    m_activated_network = new QDomElement(m_doc->createElement("activated_network"));
    m_activated_uptime = new QDomElement(m_doc->createElement("activated_uptime"));
    m_activated_load = new QDomElement(m_doc->createElement("activated_load"));
    m_activated_hardware = new QDomElement(m_doc->createElement("activated_hardware"));
    m_activated_memory = new QDomElement(m_doc->createElement("activated_memory"));
    m_activated_cpu = new QDomElement(m_doc->createElement("activated_cpu"));
    m_activated_process = new QDomElement(m_doc->createElement("activated_process"));





/*
    QDomNode xmlNode = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.insertBefore(xmlNode, doc.firstChild());
*/


}




Datas::~Datas()
{

    qDebug() << "DELETE Datas CLASS";

    delete (m_profil);
    delete (m_public);
    delete (m_uuid);
    delete (m_device);

    delete m_activated_network;
    delete m_activated_uptime;
    delete m_activated_load;
    delete m_activated_hardware;
    delete m_activated_memory;
    delete m_activated_cpu;
    delete m_activated_process;

    delete m_root;
    delete m_doc;

    if (sysinfo) delete sysinfo;
    if (loadavg) delete loadavg;
    if (uptime) delete uptime;
    if (cpu) delete cpu;
    if (cpustat) delete cpustat;
    if (mem) delete mem;
    if (swap) delete swap;
    if (netinfo) delete netinfo;
    if (netstat) delete netstat;
    if (proclist) delete proclist;
}


void Datas::Populate(QByteArray *a_datas)
{
    qDebug() << "populate";


    m_doc->appendChild(*m_root);

    m_root->appendChild(*m_profil);
    QDomText text_profil = m_doc->createTextNode("default");
    m_profil->appendChild(text_profil);

    m_root->appendChild(*m_public);
    QDomText text_public = m_doc->createTextNode(public_host?"true":"false");
    m_public->appendChild(text_public);

    m_root->appendChild(*m_device);
    QDomText text_device = m_doc->createTextNode("desktop");
    m_device->appendChild(text_device);


    // SYSINFO
    QDomElement m_name = m_doc->createElement("name");
    QDomElement m_version = m_doc->createElement("version");
    QDomElement m_architecture = m_doc->createElement("architecture");
    QDomElement m_machine = m_doc->createElement("machine");
    QDomElement m_description = m_doc->createElement("description");
    QDomElement m_patch_level = m_doc->createElement("patch_level");
    QDomElement m_vendor = m_doc->createElement("vendor");
    QDomElement m_vendor_version = m_doc->createElement("vendor_version");
    QDomElement m_vendor_name = m_doc->createElement("vendor_name");
    QDomElement m_vendor_code_name = m_doc->createElement("vendor_code_name");
    QDomElement m_os_type = m_doc->createElement("os_type");
    QDomElement m_os_base = m_doc->createElement("os_base");


    m_root->appendChild(m_name);
    QDomText text_name = m_doc->createTextNode(sysinfo->name);
    m_name.appendChild(text_name);


    m_root->appendChild(m_version);
    QDomText text_version = m_doc->createTextNode(sysinfo->version);
    m_version.appendChild(text_version);

    m_root->appendChild(m_architecture);
    QDomText text_architecture = m_doc->createTextNode(sysinfo->arch);
    m_architecture.appendChild(text_architecture);

    m_root->appendChild(m_machine);
    QDomText text_machine = m_doc->createTextNode(sysinfo->machine);
    m_machine.appendChild(text_machine);

    m_root->appendChild(m_description);
    QDomText text_description = m_doc->createTextNode(sysinfo->description);
    m_description.appendChild(text_description);


    m_root->appendChild(m_patch_level);
    QDomText text_patch_level = m_doc->createTextNode(sysinfo->patch_level);
    m_patch_level.appendChild(text_patch_level);

    m_root->appendChild(m_vendor);
    QDomText text_vendor = m_doc->createTextNode(sysinfo->vendor);
    m_vendor.appendChild(text_vendor);

    m_root->appendChild(m_vendor_name);
    QDomText text_vendor_name = m_doc->createTextNode(sysinfo->vendor_name);
    m_vendor_name.appendChild(text_vendor_name);


    m_root->appendChild(m_vendor_version);
    QDomText text_vendor_version = m_doc->createTextNode(sysinfo->vendor_version);
    m_vendor_version.appendChild(text_vendor_version);


    m_root->appendChild(m_vendor_code_name);
    QDomText text_vendor_code_name = m_doc->createTextNode(sysinfo->vendor_code_name);
    m_vendor_code_name.appendChild(text_vendor_code_name);


    m_root->appendChild(m_os_type);
    QDomText text_os_type = m_doc->createTextNode(system.m_os_type);
    m_os_type.appendChild(text_os_type);

    m_root->appendChild(m_os_base);
    QDomText text_os_base = m_doc->createTextNode(system.m_os_base);
    m_os_base.appendChild(text_os_base);



    if (activated_network)
    {
        QDomElement m_hostname = m_doc->createElement("hostname");
        QDomElement m_domain_name = m_doc->createElement("domain_name");
        QDomElement m_default_gateway = m_doc->createElement("default_gateway");
        QDomElement m_primary_dns = m_doc->createElement("primary_dns");
        QDomElement m_secondary_dns = m_doc->createElement("secondary_dns");
        QDomElement m_primary_interface = m_doc->createElement("primary_interface");
        QDomElement m_primary_addr = m_doc->createElement("primary_addr");
        QDomElement m_rx_rate = m_doc->createElement("rx_rate");
        QDomElement m_tx_rate = m_doc->createElement("tx_rate");



        netinfo = new NetInfo();
        netstat = new NetStat(netinfo->primary_interface);

        m_root->appendChild(*m_activated_network);
        QDomText text_activated_network = m_doc->createTextNode("true");
        m_activated_network->appendChild(text_activated_network);


        QDomElement m_network = m_doc->createElement("network");
        m_root->appendChild(m_network);


        m_network.appendChild(m_hostname);
        qDebug() << "populate OK : " << netinfo->hostname;

        QDomText text_hostname = m_doc->createTextNode(netinfo->hostname);
        m_hostname.appendChild(text_hostname);


        m_network.appendChild(m_domain_name);
        QDomText text_domain_name = m_doc->createTextNode(netinfo->domain_name);
        m_domain_name.appendChild(text_domain_name);



        m_network.appendChild(m_default_gateway);
        QDomText text_default_gateway = m_doc->createTextNode(netinfo->default_gateway);
        m_default_gateway.appendChild(text_default_gateway);


        m_network.appendChild(m_primary_dns);
        QDomText text_primary_dns = m_doc->createTextNode(netinfo->primary_dns);
        m_primary_dns.appendChild(text_primary_dns);



        m_network.appendChild(m_secondary_dns);
        QDomText text_secondary_dns = m_doc->createTextNode(netinfo->secondary_dns);
        m_secondary_dns.appendChild(text_secondary_dns);


        m_network.appendChild(m_primary_interface);
        QDomText text_primary_interface = m_doc->createTextNode(netinfo->primary_interface);
        m_primary_interface.appendChild(text_primary_interface);


        m_network.appendChild(m_primary_addr);
        QDomText text_primary_addr = m_doc->createTextNode(netinfo->primary_addr);
        m_primary_addr.appendChild(text_primary_addr);


        m_network.appendChild(m_rx_rate);
        QDomText text_rx_rate = m_doc->createTextNode(netstat->rx_rate);
        m_rx_rate.appendChild(text_rx_rate);


        m_network.appendChild(m_tx_rate);
        QDomText text_tx_rate = m_doc->createTextNode(netstat->tx_rate);
        m_tx_rate.appendChild(text_tx_rate);
    }
    else
        {
            m_root->appendChild(*m_activated_network);
            QDomText text_activated_network = m_doc->createTextNode("false");
            m_activated_network->appendChild(text_activated_network);
        }


    if (activated_uptime)
    {
        QDomElement m_time = m_doc->createElement("time");
        QDomElement m_days = m_doc->createElement("days");


        uptime = new Uptime();

        m_root->appendChild(*m_activated_uptime);
        QDomText text_activated_uptime = m_doc->createTextNode("true");
        m_activated_uptime->appendChild(text_activated_uptime);


        QDomElement m_uptime = m_doc->createElement("uptime");
        m_root->appendChild(m_uptime);


        m_uptime.appendChild(m_time);
        QDomText text_time = m_doc->createTextNode(uptime->time);
        m_time.appendChild(text_time);

        m_uptime.appendChild(m_days);
        QDomText text_days = m_doc->createTextNode(uptime->days);
        m_days.appendChild(text_days);
    } else
    {
        m_root->appendChild(*m_activated_uptime);
        QDomText text_activated_uptime = m_doc->createTextNode("false");
        m_activated_uptime->appendChild(text_activated_uptime);
    }


//    m_shell = new QDomElement(m_doc->createElement("shell"));
//    m_user = new QDomElement(m_doc->createElement("user"));


    if (activated_load)
    {
       QDomElement m_loadavg0 = m_doc->createElement("loadavg0");
       QDomElement m_loadavg1 = m_doc->createElement("loadavg1");
       QDomElement m_loadavg2 = m_doc->createElement("loadavg2");


       loadavg = new LoadAvg();

       m_root->appendChild(*m_activated_load);
       QDomText text_activated_load = m_doc->createTextNode("true");
       m_activated_load->appendChild(text_activated_load);


       QDomElement m_load = m_doc->createElement("load");
       m_root->appendChild(m_load);


       m_load.appendChild(m_loadavg0);
       QDomText text_loadavg0 = m_doc->createTextNode(loadavg->loadavg0);
       m_loadavg0.appendChild(text_loadavg0);

       m_load.appendChild(m_loadavg1);
       QDomText text_loadavg1 = m_doc->createTextNode(loadavg->loadavg1);
       m_loadavg1.appendChild(text_loadavg1);

       m_load.appendChild(m_loadavg2);
       QDomText text_loadavg2 = m_doc->createTextNode(loadavg->loadavg2);
       m_loadavg2.appendChild(text_loadavg2);
   } else
   {
       m_root->appendChild(*m_activated_load);
       QDomText text_activated_load = m_doc->createTextNode("false");
       m_activated_load->appendChild(text_activated_load);
   }


    if (activated_hardware)
    {
        QDomElement m_cpu_vendor = m_doc->createElement("vendor");
        QDomElement m_cpu_model = m_doc->createElement("model");
        QDomElement m_cpu_mhz = m_doc->createElement("mhz");
        QDomElement m_cpu_cache_size = m_doc->createElement("cache_size");
        QDomElement m_cpu_number = m_doc->createElement("number");
        QDomElement m_cpu_total_cores = m_doc->createElement("total_cores");
        QDomElement m_cpu_total_sockets = m_doc->createElement("total_sockets");
        QDomElement m_cpu_cores_per_socket = m_doc->createElement("cores_per_socket");


        cpu = new Cpu();


        m_root->appendChild(*m_activated_hardware);
        QDomText text_activated_hardware = m_doc->createTextNode("true");
        m_activated_hardware->appendChild(text_activated_hardware);


        QDomElement m_cpu_hardware = m_doc->createElement("cpu_hardware");
        m_root->appendChild(m_cpu_hardware);


        m_cpu_hardware.appendChild(m_cpu_vendor);
        QDomText text_cpu_vendor = m_doc->createTextNode(cpu->vendor);
        m_cpu_vendor.appendChild(text_cpu_vendor);


        m_cpu_hardware.appendChild(m_cpu_model);
        QDomText text_cpu_model = m_doc->createTextNode(cpu->model);
        m_cpu_model.appendChild(text_cpu_model);


        m_cpu_hardware.appendChild(m_cpu_mhz);
        QDomText text_cpu_mhz = m_doc->createTextNode(cpu->mhz);
        m_cpu_mhz.appendChild(text_cpu_mhz);


        m_cpu_hardware.appendChild(m_cpu_cache_size);
        QDomText text_cpu_cache_size = m_doc->createTextNode(cpu->cache_size);
        m_cpu_cache_size.appendChild(text_cpu_cache_size);


        m_cpu_hardware.appendChild(m_cpu_number);
        QDomText text_cpu_number = m_doc->createTextNode(cpu->number);
        m_cpu_number.appendChild(text_cpu_number);


        m_cpu_hardware.appendChild(m_cpu_total_cores);
        QDomText text_cpu_total_cores = m_doc->createTextNode(cpu->total_cores);
        m_cpu_total_cores.appendChild(text_cpu_total_cores);


        m_cpu_hardware.appendChild(m_cpu_total_sockets);
        QDomText text_cpu_total_sockets = m_doc->createTextNode(cpu->total_sockets);
        m_cpu_total_sockets.appendChild(text_cpu_total_sockets);


        m_cpu_hardware.appendChild(m_cpu_cores_per_socket);
        QDomText text_cpu_cores_per_socket = m_doc->createTextNode(cpu->cores_per_socket);
        m_cpu_cores_per_socket.appendChild(text_cpu_cores_per_socket);
    }
    else
    {
        m_root->appendChild(*m_activated_hardware);
        QDomText text_activated_hardware = m_doc->createTextNode("false");
        m_activated_hardware->appendChild(text_activated_hardware);
    }





    if (activated_memory)
    {
        QDomElement m_mem_ram = m_doc->createElement("mem_ram");
        QDomElement m_mem_total = m_doc->createElement("mem_total");
        QDomElement m_mem_used = m_doc->createElement("mem_used");
        QDomElement m_mem_free = m_doc->createElement("mem_free");
        QDomElement m_mem_actual_free = m_doc->createElement("mem_actual_free");
        QDomElement m_mem_actual_used = m_doc->createElement("mem_actual_used");
        QDomElement m_mem_actual_free_percent = m_doc->createElement("mem_actual_free_percent");
        QDomElement m_mem_actual_used_percent = m_doc->createElement("mem_actual_used_percent");

        QDomElement m_swap_total = m_doc->createElement("swap_total");
        QDomElement m_swap_used = m_doc->createElement("swap_used");
        QDomElement m_swap_free = m_doc->createElement("swap_free");
        QDomElement m_swap_page_in = m_doc->createElement("swap_page_in");
        QDomElement m_swap_page_out = m_doc->createElement("swap_page_out");



        mem = new Mem();
        swap = new Swap();

        m_root->appendChild(*m_activated_memory);
        QDomText text_activated_memory = m_doc->createTextNode("true");
        m_activated_memory->appendChild(text_activated_memory);



        QDomElement m_memory = m_doc->createElement("memory");
        m_root->appendChild(m_memory);



        m_memory.appendChild(m_mem_ram);
        QDomText text_mem_ram = m_doc->createTextNode(mem->ram);
        m_mem_ram.appendChild(text_mem_ram);


        m_memory.appendChild(m_mem_total);
        QDomText text_mem_total = m_doc->createTextNode(mem->total);
        m_mem_total.appendChild(text_mem_total);

        m_memory.appendChild(m_mem_used);
        QDomText text_mem_used = m_doc->createTextNode(mem->used);
        m_mem_used.appendChild(text_mem_used);

        m_memory.appendChild(m_mem_free);
        QDomText text_mem_free = m_doc->createTextNode(mem->free);
        m_mem_free.appendChild(text_mem_free);

        m_memory.appendChild(m_mem_actual_free);
        QDomText text_mem_actual_free = m_doc->createTextNode(mem->actual_free);
        m_mem_actual_free.appendChild(text_mem_actual_free);

        m_memory.appendChild(m_mem_actual_used);
        QDomText text_mem_actual_used = m_doc->createTextNode(mem->actual_used);
        m_mem_actual_used.appendChild(text_mem_actual_used);


        m_memory.appendChild(m_mem_actual_free_percent);
        QDomText text_mem_actual_free_percent = m_doc->createTextNode(mem->actual_free_percent);
        m_mem_actual_free_percent.appendChild(text_mem_actual_free_percent);

        m_memory.appendChild(m_mem_actual_used_percent);
        QDomText text_mem_actual_used_percent = m_doc->createTextNode(mem->actual_used_percent);
        m_mem_actual_used_percent.appendChild(text_mem_actual_used_percent);


        m_memory.appendChild(m_swap_total);
        QDomText text_swap_total = m_doc->createTextNode(swap->total);
        m_swap_total.appendChild(text_swap_total);

        m_memory.appendChild(m_swap_used);
        QDomText text_swap_used = m_doc->createTextNode(swap->used);
        m_swap_used.appendChild(text_swap_used);

        m_memory.appendChild(m_swap_free);
        QDomText text_swap_free = m_doc->createTextNode(swap->free);
        m_swap_free.appendChild(text_swap_free);

        m_memory.appendChild(m_swap_page_in);
        QDomText text_swap_page_in = m_doc->createTextNode(swap->page_in);
        m_swap_page_in.appendChild(text_swap_page_in);

        m_memory.appendChild(m_swap_page_out);
        QDomText text_swap_page_out = m_doc->createTextNode(swap->page_out);
        m_swap_page_out.appendChild(text_swap_page_out);
    } else
    {
        m_root->appendChild(*m_activated_memory);
        QDomText text_activated_memory = m_doc->createTextNode("false");
        m_activated_memory->appendChild(text_activated_memory);
    }






    if (activated_cpu)
    {
        QDomElement m_cpu_user = m_doc->createElement("user");
        QDomElement m_cpu_sys = m_doc->createElement("sys");
        QDomElement m_cpu_nice = m_doc->createElement("nice");
        QDomElement m_cpu_idle = m_doc->createElement("idle");
        QDomElement m_cpu_wait = m_doc->createElement("wait");
        QDomElement m_cpu_irq = m_doc->createElement("irq");
        QDomElement m_cpu_soft_irq = m_doc->createElement("soft_irq");
        QDomElement m_cpu_stolen = m_doc->createElement("stolen");
        QDomElement m_cpu_combined = m_doc->createElement("combined");
        QDomElement m_cpu_total = m_doc->createElement("total");


        cpustat = new CpuStat();

        m_root->appendChild(*m_activated_cpu);
        QDomText text_activated_cpu = m_doc->createTextNode("true");
        m_activated_cpu->appendChild(text_activated_cpu);


        QDomElement m_cpu_usage = m_doc->createElement("cpu_usage");
        m_root->appendChild(m_cpu_usage);


        m_cpu_usage.appendChild(m_cpu_user);
        QDomText text_cpu_user = m_doc->createTextNode(cpustat->user);
        m_cpu_user.appendChild(text_cpu_user);



        m_cpu_usage.appendChild(m_cpu_sys);
        QDomText text_cpu_sys = m_doc->createTextNode(cpustat->sys);
        m_cpu_sys.appendChild(text_cpu_sys);



        m_cpu_usage.appendChild(m_cpu_nice);
        QDomText text_cpu_nice = m_doc->createTextNode(cpustat->nice);
        m_cpu_nice.appendChild(text_cpu_nice);


        m_cpu_usage.appendChild(m_cpu_idle);
        QDomText text_cpu_idle = m_doc->createTextNode(cpustat->idle);
        m_cpu_idle.appendChild(text_cpu_idle);

        m_cpu_usage.appendChild(m_cpu_wait);
        QDomText text_cpu_wait = m_doc->createTextNode(cpustat->wait);
        m_cpu_wait.appendChild(text_cpu_wait);

        m_cpu_usage.appendChild(m_cpu_irq);
        QDomText text_cpu_irq = m_doc->createTextNode(cpustat->irq);
        m_cpu_irq.appendChild(text_cpu_irq);

        m_cpu_usage.appendChild(m_cpu_soft_irq);
        QDomText text_cpu_soft_irq = m_doc->createTextNode(cpustat->soft_irq);
        m_cpu_soft_irq.appendChild(text_cpu_soft_irq);


        m_cpu_usage.appendChild(m_cpu_stolen);
        QDomText text_cpu_stolen = m_doc->createTextNode(cpustat->stolen);
        m_cpu_stolen.appendChild(text_cpu_stolen);

        m_cpu_usage.appendChild(m_cpu_combined);
        QDomText text_cpu_combined = m_doc->createTextNode(cpustat->combined);
        m_cpu_combined.appendChild(text_cpu_combined);


        m_cpu_usage.appendChild(m_cpu_total);
        QDomText text_cpu_total = m_doc->createTextNode(cpustat->total);
        m_cpu_total.appendChild(text_cpu_total);
    }
    else
    {
        m_root->appendChild(*m_activated_cpu);
        QDomText text_activated_cpu = m_doc->createTextNode("false");
        m_activated_cpu->appendChild(text_activated_cpu);
    }





    /*
    for (int i = 0; i < cpu.cpus.size(); ++i)
    {
        m_cpu = new QDomElement(m_doc->createElement(QString("cpu%1").arg(tmp.setNum(i))));

        m_root->appendChild(*m_cpu);
        QDomText text_cpu = m_doc->createTextNode(cpu.cpus.at(i));
        m_cpu->appendChild(text_cpu);
    }*/


    // CPU

    // MEM


    // CPUSTAT




    // PROCESS
    if (activated_process)
    {
        proclist = new ProcList();

        m_root->appendChild(*m_activated_process);
        QDomText text_activated_process = m_doc->createTextNode("true");
        m_activated_process->appendChild(text_activated_process);


        QDomElement m_process = m_doc->createElement("process");
        m_root->appendChild(m_process);

        QDomElement m_process_number = m_doc->createElement("process_number");
        m_process.appendChild(m_process_number);

        QString counter;
        counter.setNum(proclist->stack.count());

        QDomText text_process_number = m_doc->createTextNode(counter);
        m_process_number.appendChild(text_process_number);


        while (!proclist->stack.isEmpty())
        {
          //  qDebug() << "stack length : " << proclist->stack.count();                    
            Process procs = proclist->stack.pop();
          //  qDebug() << "name : " << procs->state_name << " " << " pid : " << procs->pid;


           QDomElement m_processus = m_doc->createElement("processus");
           m_process.appendChild(m_processus);


           QDomElement m_pid = m_doc->createElement("pid");
           m_processus.appendChild(m_pid);
           QDomText text_pid = m_doc->createTextNode(procs.pid);
           m_pid.appendChild(text_pid);


           QDomElement m_state_name = m_doc->createElement("state_name");
           m_processus.appendChild(m_state_name);
           QDomText text_state_name = m_doc->createTextNode(procs.state_name);
           m_state_name.appendChild(text_state_name);


           QDomElement m_state_state = m_doc->createElement("state_state");
           m_processus.appendChild(m_state_state);
           QDomText text_state_state = m_doc->createTextNode(procs.state_state);
           m_state_state.appendChild(text_state_state);


           QDomElement m_state_ppid = m_doc->createElement("state_ppid");
           m_processus.appendChild(m_state_ppid);
           QDomText text_state_ppid = m_doc->createTextNode(procs.state_ppid);
           m_state_ppid.appendChild(text_state_ppid);

/*
           m_state_tty = new QDomElement(m_doc->createElement("state_tty"));
           m_processus.appendChild(*m_state_tty);
           QDomText text_state_tty = m_doc->createTextNode(procs->state_tty);
           m_state_tty->appendChild(text_state_tty);
*/

           QDomElement m_state_priority = m_doc->createElement("state_priority");
           m_processus.appendChild(m_state_priority);
           QDomText text_state_priority = m_doc->createTextNode(procs.state_priority);
           m_state_priority.appendChild(text_state_priority);


           QDomElement m_state_nice = m_doc->createElement("state_nice");
           m_processus.appendChild(m_state_nice);
           QDomText text_state_nice = m_doc->createTextNode(procs.state_nice);
           m_state_nice.appendChild(text_state_nice);


           QDomElement m_state_processor = m_doc->createElement("state_processor");
           m_processus.appendChild(m_state_processor);
           QDomText text_state_processor = m_doc->createTextNode(procs.state_processor);
           m_state_processor.appendChild(text_state_processor);


           QDomElement m_state_threads = m_doc->createElement("state_threads");
           m_processus.appendChild(m_state_threads);
           QDomText text_state_threads = m_doc->createTextNode(procs.state_threads);
           m_state_threads.appendChild(text_state_threads);


           QDomElement m_mem_size = m_doc->createElement("mem_size");
           m_processus.appendChild(m_mem_size);
           QDomText text_mem_size = m_doc->createTextNode(procs.mem_size);
           m_mem_size.appendChild(text_mem_size);


           QDomElement m_mem_resident = m_doc->createElement("mem_resident");
           m_processus.appendChild(m_mem_resident);
           QDomText text_mem_resident = m_doc->createTextNode(procs.mem_resident);
           m_mem_resident.appendChild(text_mem_resident);


           QDomElement m_mem_share = m_doc->createElement("mem_share");
           m_processus.appendChild(m_mem_share);
           QDomText text_mem_share = m_doc->createTextNode(procs.mem_share);
           m_mem_share.appendChild(text_mem_share);


           QDomElement m_mem_minor_faults = m_doc->createElement("mem_minor_faults");
           m_processus.appendChild(m_mem_minor_faults);
           QDomText text_mem_minor_faults = m_doc->createTextNode(procs.mem_minor_faults);
           m_mem_minor_faults.appendChild(text_mem_minor_faults);


           QDomElement m_mem_major_faults = m_doc->createElement("mem_major_faults");
           m_processus.appendChild(m_mem_major_faults);
           QDomText text_mem_major_faults = m_doc->createTextNode(procs.mem_major_faults);
           m_mem_major_faults.appendChild(text_mem_major_faults);


           QDomElement m_mem_page_faults = m_doc->createElement("mem_page_faults");
           m_processus.appendChild(m_mem_page_faults);
           QDomText text_mem_page_faults = m_doc->createTextNode(procs.mem_page_faults);
           m_mem_page_faults.appendChild(text_mem_page_faults);


           QDomElement m_time_start_time = m_doc->createElement("time_start_time");
           m_processus.appendChild(m_time_start_time);
           QDomText text_time_start_time = m_doc->createTextNode(procs.time_start_time);
           m_time_start_time.appendChild(text_time_start_time);


           QDomElement m_time_user = m_doc->createElement("time_user");
           m_processus.appendChild(m_time_user);
           QDomText text_time_user = m_doc->createTextNode(procs.time_user);
           m_time_user.appendChild(text_time_user);


           QDomElement m_time_sys = m_doc->createElement("time_sys");
           m_processus.appendChild(m_time_sys);
           QDomText text_time_sys = m_doc->createTextNode(procs.time_sys);
           m_time_sys.appendChild(text_time_sys);


           QDomElement m_time_total = m_doc->createElement("time_total");
           m_processus.appendChild(m_time_total);
           QDomText text_time_total = m_doc->createTextNode(procs.time_total);
           m_time_total.appendChild(text_time_total);
        }

    } else
    {
        m_root->appendChild(*m_activated_process);
        QDomText text_activated_process = m_doc->createTextNode("false");
        m_activated_process->appendChild(text_activated_process);
    }



    // *******************************************************


    QTextStream out(a_datas);
    m_doc->save(out, 0);
    qDebug() << "doc : " << m_doc->toString(0);
}
