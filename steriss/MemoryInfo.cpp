/*
* Copyright (c) 2012 Marius Zwicker
* All rights reserved.
* 
* @LICENSE_HEADER_START:GPL@
* 
* This file is part of Steriss.
* 
* Steriss is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* Steriss is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*  
* You should have received a copy of the GNU General Public License
* along with Steriss. If not, see <http://www.gnu.org/licenses/>.
* 
* @LICENSE_HEADER_END:GPL@
*/


#include "MemoryInfo.h"

#ifdef _WIN32

/* Reduces build time by omitting extra system headers */
# define WIN32_LEAN_AND_MEAN
# include <windows.h>


uint64_t MemoryInfo::physicalMemory() {
  MEMORYSTATUSEX mem_stat;
  mem_stat.dwLength = sizeof (mem_stat);

  GlobalMemoryStatusEx (&mem_stat);

  return mem_stat.ullTotalPhys / bytesPerKilobyte;
}

uint64_t MemoryInfo::availablePhysicalMemory() {
  MEMORYSTATUSEX mem_stat;
  mem_stat.dwLength = sizeof (mem_stat);

  GlobalMemoryStatusEx (&mem_stat);

  return mem_stat.ullAvailPhys / bytesPerKilobyte;
}

#elif defined(__linux__)

#include <statgrab.h>

uint64_t MemoryInfo::physicalMemory() {
  sg_mem_stats* mem_stat_ptr = sg_get_mem_stats();

  return mem_stat_ptr->total / bytesPerKilobyte;
}

uint64_t MemoryInfo::availablePhysicalMemory() {
  sg_mem_stats* mem_stat_ptr = sg_get_mem_stats();

  return mem_stat_ptr->free / bytesPerKilobyte;
}

#elif defined(__APPLE__)

#include <exception>
#include <sys/sysctl.h>

uint64_t MemoryInfo::physicalMemory() {
  int64_t value = 0;
  size_t length = sizeof(value);
  int mib [] = { CTL_HW, HW_MEMSIZE };

  if(-1 == sysctl(mib, 2, &value, &length, NULL, 0))
      return 0; // error

  return value / bytesPerKilobyte;
}

uint64_t MemoryInfo::availablePhysicalMemory() {
  // FIXME: This is ALL system memory, not the free one
  return physicalMemory();
}

#else
# error "Unsupported operating system, implement me"
#endif
