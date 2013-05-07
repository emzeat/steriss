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


#include "CadFormat.h"

#include "Debug.h"

#include <iomanip>
#include <boost/filesystem.hpp>
#include <xdispatch/dispatch>
#include <boost/bind.hpp>

CadFormat::CadFormat(const std::string &directory)
  : _active( not directory.empty() ), _write_queue("de.mlba-team.steriss.stl"), _directory(directory) {

}

CadFormat::~CadFormat() {

}

void CadFormat::open() {
  if ( active() ) {

    boost::filesystem::path output_path( _directory );
    boost::filesystem::create_directories( output_path );

    output_path /= "_voxel." + format_stem();
    sINFO("Preparing STL output to %s", %output_path);

    open_output( _output, output_path.string() );
    on_open();

  } else {
    sDEBUG("Skipping %s output as requested (see flag 'voxelfile')", %format_name());
  }
}

void CadFormat::close() {

}

void CadFormat::writeData(const CrackData &cd) {
  if( active() ) {
    _cd = cd;
    sDEBUG("Writing some %s data", %format_name() );

    _cd.Cube->apply ( boost::bind( &CadFormat::write_voxel, this, _1, _2, _3 ) );
  }
}
