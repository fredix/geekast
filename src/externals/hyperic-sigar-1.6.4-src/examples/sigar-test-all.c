/* Copyright (C) 2007 MySQL AB
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

/**
 * a test app which calls all sigar methods once and prints their output
 *
 * send your comments to
 * - sigar-dev@hyperic.org
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "sigar.h"

#ifdef WIN32
#define EOL "\r\n"
#else
#define EOL "\n"
#endif

/* on 32bit we need %Lu, on 64bit it is %lu */
#if SIZEOF_SIZE_T == 8
# define F_U64 "%lu"
#else
# define F_U64 "%Lu"
#endif

#define TEST(name) \
	static int name(sigar_t *t) 

TEST(test_sigar_mem_get) {
	sigar_mem_t mem;

	assert(SIGAR_OK == sigar_mem_get(t, &mem));

	fprintf(stderr, "%s.%d: " EOL
			"  mem.ram = "F_U64", " EOL
			"  mem.total = "F_U64", " EOL
			"  mem.used = "F_U64", " EOL
			"  mem.free = "F_U64", " EOL
			"  mem.actual_free = "F_U64", " EOL
			"  mem.actual_used = "F_U64"" EOL, 
			__FILE__, __LINE__, 
			mem.ram,
			mem.total,
			mem.used, 
			mem.free,
			mem.actual_free,
			mem.actual_used);

	return 0;
}

TEST(test_sigar_pid_get) {
	sigar_pid_t pid;

	pid = sigar_pid_get(t);

	fprintf(stderr, "%s.%d: " EOL
			"  pid = %d" EOL, 
			__FILE__, __LINE__,
			pid);

	return 0;
}

TEST(test_sigar_swap_get) {
	sigar_swap_t swap;

	assert(SIGAR_OK == sigar_swap_get(t, &swap));
	
	fprintf(stderr, "%s.%d: " EOL
			"  swap.total = "F_U64", " EOL
			"  swap.used = "F_U64", " EOL
			"  swap.free = "F_U64"" EOL, 
			__FILE__, __LINE__,
			swap.total,
			swap.used,
			swap.free);

	return 0;
}

TEST(test_sigar_cpu_get) {
	sigar_cpu_t cpu;

	assert(SIGAR_OK == sigar_cpu_get(t, &cpu));
	
	fprintf(stderr, "%s.%d: " EOL
			"  cpu.user = "F_U64", " EOL
			"  cpu.sys = "F_U64", " EOL
			"  cpu.nice = "F_U64", " EOL
			"  cpu.idle = "F_U64", " EOL
			"  cpu.wait = "F_U64", " EOL
			"  cpu.total = "F_U64"" EOL, 
			__FILE__, __LINE__,
			cpu.user,
			cpu.sys,
			cpu.nice,
			cpu.idle,
			cpu.wait,
			cpu.total
			);

	return 0;
}

TEST(test_sigar_cpu_list_get) {
	sigar_cpu_list_t cpulist;
	size_t i;

	assert(SIGAR_OK == sigar_cpu_list_get(t, &cpulist));

	fprintf(stderr, "%s.%d: " EOL,
			__FILE__, __LINE__, __FUNCTION__);

	for (i = 0; i < cpulist.number; i++) {
		sigar_cpu_t cpu = cpulist.data[i];

		fprintf(stderr, 
			"[%zd] " EOL
			"  cpu.user = "F_U64", " EOL
			"  cpu.sys = "F_U64", " EOL
			"  cpu.nice = "F_U64", " EOL
			"  cpu.idle = "F_U64", " EOL
			"  cpu.wait = "F_U64", " EOL
			"  cpu.total = "F_U64"" EOL, 
			i,
			cpu.user,
			cpu.sys,
			cpu.nice,
			cpu.idle,
			cpu.wait,
			cpu.total
			);
	}

	sigar_cpu_list_destroy(t, &cpulist);

	return 0;
}

TEST(test_sigar_cpu_info_get) {
	sigar_cpu_info_list_t cpuinfo;
	size_t i;

	assert(SIGAR_OK == sigar_cpu_info_list_get(t, &cpuinfo));

	for (i = 0; i < cpuinfo.number; i++) {
		sigar_cpu_info_t info = cpuinfo.data[i];

		fprintf(stderr, "%s.%d: " EOL
			"[%zd] " EOL
			"  info.vendor = %s, " EOL
			"  info.model = %s, " EOL
			"  info.mhz = %d, " EOL
			"  info.cache_size = "F_U64"" EOL,
			__FILE__, __LINE__,
			i,
			info.vendor,
			info.model,
			info.mhz,
			info.cache_size
			);
	}

	sigar_cpu_info_list_destroy(t, &cpuinfo);

	return 0;
}

TEST(test_sigar_uptime_get) {
	sigar_uptime_t uptime;

	assert(SIGAR_OK == sigar_uptime_get(t, &uptime));

	fprintf(stderr, "%s.%d: " EOL
			"  uptime = %f" EOL, 
			__FILE__, __LINE__,
			uptime.uptime);

	return 0;
}

TEST(test_sigar_loadavg_get) {
	sigar_loadavg_t loadavg;

	if (SIGAR_OK == sigar_loadavg_get(t, &loadavg)) {

		fprintf(stderr, "%s.%d: " EOL
			"  loadavg.0 = %.2f" EOL 
			"  loadavg.1 = %.2f" EOL 
			"  loadavg.2 = %.2f" EOL, 
			__FILE__, __LINE__,
			loadavg.loadavg[0],
			loadavg.loadavg[1],
			loadavg.loadavg[2]
			);
	}

	return 0;
}


TEST(test_sigar_uptime_string) {
	sigar_uptime_t uptime;
	char b[256];

	assert(SIGAR_OK == sigar_uptime_string(t, &uptime, b, sizeof(b)));

	fprintf(stderr, "%s.%d: " EOL
			"  uptime = %f" EOL
			"  uptime.string = %s" EOL, 
			__FILE__, __LINE__,
			uptime.uptime,
			b);

	return 0;
}

TEST(test_sigar_resource_limit_get) {
	sigar_resource_limit_t rlimit;

	assert(SIGAR_OK == sigar_resource_limit_get(t, &rlimit));

	fprintf(stderr, "%s.%d: " EOL
			"  rlimit.cpu_cur = "F_U64"" EOL
			"  rlimit.cpu_max = "F_U64"" EOL
			"  rlimit.file_size_cur = "F_U64"" EOL
			"  rlimit.file_size_max = "F_U64"" EOL
			"  rlimit.pipe_size_cur = "F_U64"" EOL
			"  rlimit.pipe_size_max = "F_U64"" EOL
			"  rlimit.data_cur = "F_U64"" EOL
			"  rlimit.data_max = "F_U64"" EOL
			"  rlimit.stack_cur = "F_U64"" EOL
			"  rlimit.stack_max = "F_U64"" EOL
			"  rlimit.core_cur = "F_U64"" EOL
			"  rlimit.core_max = "F_U64"" EOL
			"  rlimit.memory_cur = "F_U64"" EOL
			"  rlimit.memory_max = "F_U64"" EOL
			"  rlimit.processes_cur = "F_U64"" EOL
			"  rlimit.processes_max = "F_U64"" EOL
			"  rlimit.open_files_cur = "F_U64"" EOL
			"  rlimit.open_files_max = "F_U64"" EOL
			"  rlimit.virtual_memory_cur = "F_U64"" EOL
			"  rlimit.virtual_memory_max = "F_U64"" EOL,
			__FILE__, __LINE__,
			rlimit.cpu_cur,
			rlimit.cpu_max,
			rlimit.file_size_cur,
			rlimit.file_size_max,
			rlimit.pipe_size_cur,
			rlimit.pipe_size_max,
			rlimit.data_cur,
			rlimit.data_max,
			rlimit.stack_cur,
			rlimit.stack_max,
			rlimit.core_cur,
			rlimit.core_max,
			rlimit.memory_cur,
			rlimit.memory_max,
			rlimit.processes_cur,
			rlimit.processes_max,
			rlimit.open_files_cur,
			rlimit.open_files_max,
			rlimit.virtual_memory_cur,
			rlimit.virtual_memory_max
			);

	return 0;
}

TEST(test_sigar_proc_stat_get) {
	sigar_proc_stat_t proc_stat;

	assert(SIGAR_OK == sigar_proc_stat_get(t, &proc_stat));

	fprintf(stderr, "%s.%d: " EOL
			"  proc.total = "F_U64"" EOL, 
			__FILE__, __LINE__,
			proc_stat.total);

	return 0;
}

TEST(test_sigar_proc_list_get) {
	sigar_proc_list_t proclist;
	size_t i;

	assert(SIGAR_OK == sigar_proc_list_get(t, &proclist));

	fprintf(stderr, "%s.%d: " EOL,
			__FILE__, __LINE__, __FUNCTION__);

	for (i = 0; i < proclist.number; i++) {
		sigar_pid_t pid = proclist.data[i];
		sigar_proc_mem_t proc_mem;
		sigar_proc_time_t proc_time;
		sigar_proc_state_t proc_state;
		int ret;

		if (SIGAR_OK != (ret = sigar_proc_mem_get(t, pid, &proc_mem))) {
			fprintf(stderr, "%s.%d: sigar_proc_mem_get(%d) failed with: %d" EOL,
					__FILE__, __LINE__,
					pid, ret
					);
			continue;
		}
		if (SIGAR_OK != sigar_proc_time_get(t, pid, &proc_time)) {
			continue;
		}
		assert(SIGAR_OK == sigar_proc_state_get(t, pid, &proc_state));

		/* only show mysqld stats */
		if (0 != strcmp("mysqld", proc_state.name)) continue;

		fprintf(stderr, 
			"[%zd] " EOL
			"  proc.pid = %d" EOL

			"  proc_state.name = %s" EOL
			"  proc_state.state = %c" EOL
			"  proc_state.ppid = %d" EOL
			"  proc_state.tty = 0x%x" EOL
			"  proc_state.priority = %d" EOL
			"  proc_state.nice = %d" EOL
			"  proc_state.processor = %d" EOL
			"  proc_state.threads = "F_U64"" EOL
			
			"  proc_mem.size = "F_U64"" EOL
			"  proc_mem.resident = "F_U64"" EOL
			"  proc_mem.share = "F_U64"" EOL
			"  proc_mem.minor_faults = "F_U64"" EOL
			"  proc_mem.major_faults = "F_U64"" EOL
			"  proc_mem.page_faults = "F_U64"" EOL

			"  proc_time.start_time = "F_U64"" EOL
			"  proc_time.user = "F_U64"" EOL
			"  proc_time.sys = "F_U64"" EOL
			"  proc_time.total = "F_U64"" EOL,
			i,
			pid,

			proc_state.name,
			proc_state.state,
			proc_state.ppid,
			proc_state.tty,
			proc_state.priority,
			proc_state.nice,
			proc_state.processor,
			proc_state.threads,
			
			proc_mem.size,
			proc_mem.resident,
			proc_mem.share,
			proc_mem.minor_faults,
			proc_mem.major_faults,
			proc_mem.page_faults,
			
			proc_time.start_time,
			proc_time.user,
			proc_time.sys,
			proc_time.total

			);
	}

	sigar_proc_list_destroy(t, &proclist);

	return 0;
}

TEST(test_sigar_file_system_list_get) {
	sigar_file_system_list_t fslist;
	size_t i;

	assert(SIGAR_OK == sigar_file_system_list_get(t, &fslist));

	fprintf(stderr, "%s.%d: (items = %ld)" EOL,
			__FILE__, __LINE__,
			fslist.number);

	for (i = 0; i < fslist.number; i++) {
		sigar_file_system_t fs = fslist.data[i];
		sigar_file_system_usage_t fsusage;
		int r;

		if (SIGAR_OK != (r = sigar_file_system_usage_get(t, fs.dir_name, &fsusage))) {
			fprintf(stderr, "%s.%d: " EOL
					"sigar_file_system_usage_get(%s) failed with: %s (%d) " EOL,
					__FILE__, __LINE__,
					fs.dir_name, strerror(errno), errno);
			continue;
		}

		fprintf(stderr, 
			"[%zd] " EOL  
			"  fs.dirname = %s" EOL
			"  fs.devname = %s" EOL
			"  fs.typename = %s" EOL
			"  fs.sys-type-name = %s" EOL
			"  fs.type = %d" EOL
			"  fs.flags = 0x%lx" EOL
			"  fsusage.total = "F_U64"" EOL
			"  fsusage.free = "F_U64"" EOL
			"  fsusage.used = "F_U64"" EOL
			"  fsusage.avail = "F_U64"" EOL
			"  fsusage.files = "F_U64"" EOL
			"  fsusage.disk_reads = "F_U64"" EOL
			"  fsusage.disk_writes = "F_U64"" EOL
			"  fsusage.disk_write_bytes = "F_U64"" EOL
			"  fsusage.disk_read_bytes = "F_U64"" EOL
			"  fsusage.disk_queue = "F_U64"" EOL
			"  fsusage.use_percent = %2f" EOL,

			i,
			fs.dir_name,
			fs.dev_name,
			fs.type_name,
			fs.sys_type_name,
			fs.type,
			fs.flags,

			fsusage.total,
			fsusage.free,
			fsusage.used,
			fsusage.avail,
			fsusage.files,
			fsusage.disk_reads,
			fsusage.disk_writes,
			fsusage.disk_write_bytes,
			fsusage.disk_read_bytes,
			fsusage.disk_queue,
			fsusage.use_percent
			);
	}

	sigar_file_system_list_destroy(t, &fslist);

	return 0;
}

TEST(test_sigar_sys_info_get) {
	sigar_sys_info_t sysinfo;

	assert(SIGAR_OK == sigar_sys_info_get(t, &sysinfo));

	fprintf(stderr, "%s.%d: " EOL
			"  sysinfo.names = %s" EOL
			"  sysinfo.versions = %s" EOL
			"  sysinfo.archs = %s" EOL
			"  sysinfo.machines = %s" EOL
			"  sysinfo.descriptions = %s" EOL
			"  sysinfo.patch_levels = %s" EOL
			"  sysinfo.vendors = %s" EOL
			"  sysinfo.vendor_versions = %s" EOL
			"  sysinfo.vendor_names = %s" EOL
			"  sysinfo.vendor_code_name = %s" EOL,
			__FILE__, __LINE__,
			sysinfo.name,
			sysinfo.version,
			sysinfo.arch,
			sysinfo.machine,
			sysinfo.description,
			sysinfo.patch_level,
			sysinfo.vendor,
			sysinfo.vendor_version,
			sysinfo.vendor_name,
			sysinfo.vendor_code_name
			);

	return 0;
}



int main() {
	sigar_t *t;
	
	assert(SIGAR_OK == sigar_open(&t));

	assert(0 == test_sigar_pid_get(t));
	assert(0 == test_sigar_mem_get(t));
	assert(0 == test_sigar_swap_get(t));
	assert(0 == test_sigar_cpu_get(t));
	assert(0 == test_sigar_cpu_list_get(t));
	assert(0 == test_sigar_cpu_info_get(t));
	assert(0 == test_sigar_uptime_get(t));
	assert(0 == test_sigar_uptime_string(t));
	assert(0 == test_sigar_loadavg_get(t));
	assert(0 == test_sigar_resource_limit_get(t));
	assert(0 == test_sigar_proc_stat_get(t));
	assert(0 == test_sigar_proc_list_get(t));
	assert(0 == test_sigar_file_system_list_get(t));
	assert(0 == test_sigar_sys_info_get(t));

	sigar_close(t);

	return 0;
}
