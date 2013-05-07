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


#include "Debug.h"

#include <gflags/gflags.h>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <xdispatch/dispatch>
#include <fstream>

DEFINE_bool (verbose, false, "Enables verbose output");
DEFINE_string(logfile, "", "Configure a file to be used as logfile here. Note that this will supress all output and truncate the file");

using namespace boost::posix_time;

static xdispatch::once file_init_once;
static std::ofstream log_file;
static void open_log_file() {
  if( not FLAGS_logfile.empty() ) {
    log_file.open ( FLAGS_logfile.c_str(), std::ios::out | std::ios::trunc );
    std::cout.rdbuf( log_file.rdbuf () );
  }

  time_facet * facet = new time_facet("%H:%M:%S");
  std::cout.imbue(std::locale(std::cout.getloc(), facet));
}

void Debug::print (Debug::Level l, const std::string &msg) {

  file_init_once( boost::bind( open_log_file ) );

  if( not FLAGS_verbose && l == Debug::S_DEBUG )
    return;

  ptime now = second_clock::local_time ();

  synchronized {
    switch( l ) {
      case Debug::S_ERROR:
        std::cout << "ERROR ";
        break;
      case Debug::S_WARNING:
        std::cout << "WARN  ";
        break;
      case Debug::S_DEBUG:
        std::cout << "Debug ";
        break;
      case Debug::S_INFO:
        std::cout << "Info  ";
        break;
    }

    std::cout << now << " " << msg << std::endl << std::flush;
  }
}
