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


#include "JobSplitter.h"

#include "MemoryInfo.h"
#include "Debug.h"

JobSplitter::JobSplitter(const IO& io)
  : _io(io), _started(false), _current_job(0) {

  uint64_t image_depth = 0;
  switch( _io.imageType() ){
    case CV_8U:
      image_depth = sizeof(uint8_t);
      break;
    case CV_16U:
      image_depth = sizeof(uint16_t);
      break;

  }

  // the memory we may use (in KB)
  uint64_t system_memory = MemoryInfo::availablePhysicalMemory();
  // the size of a single image (in KB)
  uint64_t image_size = image_depth * _io.imageSize().width * _io.imageSize().height / MemoryInfo::bytesPerKilobyte;
  // the size of a single level within the cube (in KB)
  uint64_t level_size = sizeof(CrackCube::Scalar) * _io.imageSize().width * _io.imageSize().height / MemoryInfo::bytesPerKilobyte;
  // the size needed for storing an image plus its corresponding level (in KB)
  uint64_t data_size = image_size + level_size;

  sINFO("Found %.3f GB of available system memory", %(system_memory / (double)MemoryInfo::kilobytesPerGigabyte) );
  sINFO("Loading all data would need %.3f GB of memory", %(data_size*_io.imageCount() / (double)MemoryInfo::kilobytesPerGigabyte) );

  _job_ct = data_size*_io.imageCount() / system_memory + 1; // +1 corresponds to ceil()
  _images_per_job = _io.imageCount() / _job_ct - 2*overlap;

  // in KB
  size_t size_per_job = ((_images_per_job + 2*overlap)*data_size);
  sINFO("Dividing into %i jobs (#images=%i) using %.3f GB", %_job_ct %_images_per_job %(size_per_job / (double)MemoryInfo::kilobytesPerGigabyte) );
}

CrackData JobSplitter::nextJob() {
  if( _started )
    ++_current_job;

  sDEBUG("Reading images %i to %i", %currentStartIndex() %currentEndIndex() );
  _started = true;

  return _io.readImages(currentStartIndex(), currentEndIndex());
}

bool JobSplitter::hasNext() {
  return _started == false || _current_job < _job_ct;
}

size_t JobSplitter::currentStartIndex() const {
  return std::max( _current_job * _images_per_job - overlap, 0 );
}

size_t JobSplitter::currentEndIndex() const {
  return std::min( (_current_job+1) * _images_per_job + overlap, (int)_io.imageCount() );
}
