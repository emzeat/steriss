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

#ifndef MEMORY_INFO_H_
#define MEMORY_INFO_H_

#include <stdint.h>

class MemoryInfo {

  public:

    static const uint64_t bytesPerKilobyte = 1024;
    static const uint64_t bytesPerMegabyte = 1024*1024;
    static const uint64_t bytesPerGigabyte = 1024*1024*1024;
    static const uint64_t kilobytesPerMegabyte = 1024;
    static const uint64_t kilobytesPerGigabyte = 1024*1024;

    /**
     * @returns The amount of actual physical memory in kilobytes
     */
    static uint64_t physicalMemory();
    /**
     * @return The amount of phyiscal memory currently available in kilobytes
     */
    static uint64_t availablePhysicalMemory();
};

#endif /* MEMORY_INFO_H_ */
