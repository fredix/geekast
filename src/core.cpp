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

    m_root = new QVariantMap;
    m_profil = new QVariantMap();
    m_public = new QVariantMap();
    m_uuid = new QVariantMap();
    m_device = new QVariantMap();


    m_activated_network = new QVariantMap();
    m_activated_uptime = new QVariantMap();
    m_activated_load = new QVariantMap();
    m_activated_hardware = new QVariantMap();
    m_activated_memory = new QVariantMap();
    m_activated_cpu = new QVariantMap();
    m_activated_process = new QVariantMap();

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
    qDebug() << "Datas::Populate";

    m_root->insert("profil", "default");
    m_root->insert("public", public_host?"true":"false");
    m_root->insert("device", "desktop");


    // SYSINFO
    QVariantMap data;
    data.insert("name", sysinfo->name);
    data.insert("version", sysinfo->version);
    data.insert("architecture", sysinfo->arch);
    data.insert("machine", sysinfo->machine);
    data.insert("description", sysinfo->description);
    data.insert("patch_level", sysinfo->patch_level);
    data.insert("vendor", sysinfo->vendor);
    data.insert("vendor_name", sysinfo->vendor_name);
    data.insert("vendor_version", sysinfo->vendor_version);
    data.insert("vendor_code_name", sysinfo->vendor_code_name);
    data.insert("os_type", system.m_os_type);
    data.insert("os_base", system.m_os_base);

    m_root->insert("sysinfo", data);


    if (activated_network)
    {
        QVariantMap data;


        netinfo = new NetInfo();
        netstat = new NetStat(netinfo->primary_interface);


        data.insert("activated", "true");
        data.insert("hostname", netinfo->hostname);
        data.insert("domain_name", netinfo->domain_name);
        data.insert("default_gateway", netinfo->default_gateway);
        data.insert("primary_dns", netinfo->primary_dns);
        data.insert("secondary_dns", netinfo->secondary_dns);
        data.insert("primary_interface", netinfo->primary_interface);
        data.insert("primary_addr", netinfo->primary_addr);
        data.insert("rx_rate", netstat->rx_rate);
        data.insert("tx_rate", netstat->tx_rate);


        m_root->insert("network", data);
    }
    else
    {
        m_activated_network->insert("activated", "false");
        m_root->insert("network", *m_activated_network);
    }


    if (activated_uptime)
    {
        QVariantMap data;

        uptime = new Uptime();

        data.insert("activated", "true");
        data.insert("time", uptime->time);
        data.insert("days", uptime->days);

        m_root->insert("uptime", data);
    }
    else
    {
        m_activated_uptime->insert("activated", "false");
        m_root->insert("uptime", *m_activated_uptime);
    }



    if (activated_load)
    {
        QVariantMap data;

       loadavg = new LoadAvg();

       data.insert("activated", "true");
       data.insert("loadavg0", loadavg->loadavg0);
       data.insert("loadavg1", loadavg->loadavg1);
       data.insert("loadavg2", loadavg->loadavg2);

       m_root->insert("load", data);

   } else
   {
       m_activated_load->insert("activated", "false");
       m_root->insert("load", *m_activated_load);
   }


    if (activated_hardware)
    {
        QVariantMap data;

        cpu = new Cpu();

        data.insert("activated", "true");
        data.insert("vendor", cpu->vendor);
        data.insert("model", cpu->model);
        data.insert("mhz", cpu->mhz);
        data.insert("cache_size", cpu->cache_size);
        data.insert("number", cpu->number);
        data.insert("total_cores", cpu->total_cores);
        data.insert("total_sockets", cpu->total_sockets);
        data.insert("cores_per_socket", cpu->cores_per_socket);

        m_root->insert("cpu_hardware", data);
    }
    else
    {
        m_activated_hardware->insert("activated", "false");
        m_root->insert("cpu_hardware", *m_activated_hardware);

    }


    if (activated_memory)
    {
        QVariantMap data;


        mem = new Mem();
        swap = new Swap();

        data.insert("activated", "true");


        data.insert("mem_ram", mem->ram);
        data.insert("mem_total", mem->ram);
        data.insert("mem_used", mem->ram);
        data.insert("mem_free", mem->ram);
        data.insert("mem_actual_free", mem->ram);
        data.insert("mem_actual_used", mem->ram);
        data.insert("mem_actual_free_percent", mem->ram);
        data.insert("mem_actual_used_percent", mem->ram);
        data.insert("swap_total", mem->ram);
        data.insert("swap_used", mem->ram);
        data.insert("swap_free", mem->ram);
        data.insert("swap_page_in", mem->ram);
        data.insert("m_swap_page_out", mem->ram);


        m_root->insert("memory", data);
    }
    else
    {
        m_activated_memory->insert("activated", "false");
        m_root->insert("memory", *m_activated_memory);
    }


    if (activated_cpu)
    {
        QVariantMap data;

        cpustat = new CpuStat();

        data.insert("activated", "true");
        data.insert("user", cpustat->user);
        data.insert("sys", cpustat->sys);
        data.insert("nice", cpustat->nice);
        data.insert("idle", cpustat->idle);
        data.insert("wait", cpustat->wait);
        data.insert("irq", cpustat->irq);
        data.insert("soft_irq", cpustat->soft_irq);
        data.insert("stolen", cpustat->stolen);
        data.insert("combined", cpustat->combined);
        data.insert("total", cpustat->total);

        m_root->insert("cpu_usage", data);
    }
    else
    {
        m_activated_cpu->insert("activated", "false");
        m_root->insert("cpu_usage", *m_activated_cpu);
    }





    // CPU

    // MEM


    // CPUSTAT




    // PROCESS
    if (activated_process)
    {
        QVariantMap data;

        proclist = new ProcList();

        QString counter;
        counter.setNum(proclist->stack.count());

        data.insert("activated", "true");
        data.insert("process_number", counter);


        QVariantList processus_list;

        while (!proclist->stack.isEmpty())
        {
          //  qDebug() << "stack length : " << proclist->stack.count();
            Process procs = proclist->stack.pop();
          //  qDebug() << "name : " << procs->state_name << " " << " pid : " << procs->pid;


           QVariantMap m_processus;
           //m_process.appendChild(m_processus);

           m_processus.insert("pid", procs.pid);
           m_processus.insert("state_name", procs.state_name);
           m_processus.insert("state_state", procs.state_state);
           m_processus.insert("sate_ppid", procs.state_ppid);

/*
           m_state_tty = new QDomElement(m_doc->createElement("state_tty"));
           m_processus.appendChild(*m_state_tty);
           QDomText text_state_tty = m_doc->createTextNode(procs->state_tty);
           m_state_tty->appendChild(text_state_tty);
*/

           m_processus.insert("state_priority", procs.state_priority);
           m_processus.insert("state_nice", procs.state_nice);
           m_processus.insert("state_processor", procs.state_processor);
           m_processus.insert("state_theads", procs.state_threads);
           m_processus.insert("mem_size", procs.mem_size);
           m_processus.insert("mem_resident", procs.mem_resident);
           m_processus.insert("mem_share", procs.mem_share);
           m_processus.insert("mem_minor_faults", procs.mem_minor_faults);
           m_processus.insert("mem_major_faults", procs.mem_major_faults);
           m_processus.insert("mem_page_faults", procs.mem_page_faults);
           m_processus.insert("time_start_time", procs.time_start_time);
           m_processus.insert("time_user", procs.time_user);
           m_processus.insert("time_sys", procs.time_sys);
           m_processus.insert("time_total", procs.time_total);

           processus_list << m_processus;
        }
        data.insert("processus", processus_list);
        m_root->insert("process", data);
    }
    else
    {
        m_activated_process->insert("activated", "false");
        m_root->insert("process", *m_activated_process);
    }


    QFile file("/tmp/geekast.json");
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);   // we will serialize the data into the file

    QJson::Serializer serializer;
    QByteArray json = serializer.serialize(*m_root);
    out << json;

    *a_datas = json;
}

