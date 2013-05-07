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


#include "Stl.h"

#include "Debug.h"

#include <iomanip>
#include <boost/filesystem.hpp>
#include <xdispatch/dispatch>
#include <boost/bind.hpp>


StlAscii::StlAscii(const std::string& directory)
  : CadFormat( directory ) {

}

StlAscii::~StlAscii(){

}

void StlAscii::open_output(std::ofstream &output, const std::string &path) {

  output.open( path.c_str(), std::ios::trunc | std::ios::out );
}

void StlAscii::on_open() {
  sDEBUG("Writing STL file header");

  output() << "solid vcg" << std::endl;
}

void StlAscii::on_close() {
  sDEBUG("Writing STL file footer");

  output() << "endsolid vcg";
}

void StlAscii::write_voxel(size_t x, size_t y, size_t z) {

  if( current_data().ImageStack->pixelAt <uchar>(z,x,y) == Color::BLACK_SOLID )
    return;

  const size_t max_x = current_data().Cube->cols();
  const size_t max_y = current_data().Cube->rows();
  const size_t max_z = current_data().Cube->height();
  float x_f = x, y_f = y, z_f = z;

  /// normal in -x
  if( x == 0 || current_data().ImageStack->pixelAt <uchar>(z,x-1,y) == Color::BLACK_SOLID ) {
    write_queue().sync([&]{
      output()
          << "  facet normal  -1.000000e+00  0.000000e+00  0.000000e+00" << std::endl
          << "    outer loop" << std::endl
          << "      vertex " << (x_f - 0.5) << " " << (y_f - 0.5) << " " << (z_f - 0.5)
          << "      vertex " << (x_f - 0.5) << " " << (y_f - 0.5) << " " << (z_f + 0.5)
          << "      vertex " << (x_f - 0.5) << " " << (y_f + 0.5) << " " << (z_f + 0.5)
          << "    endloop" << std::endl
          << "  endfacet" << std::endl
          << "  facet normal  -1.000000e+00  0.000000e+00  0.000000e+00" << std::endl
          << "    outer loop" << std::endl
          << "      vertex " << (x_f - 0.5) << " " << (y_f - 0.5) << " " << (z_f - 0.5)
          << "      vertex " << (x_f - 0.5) << " " << (y_f + 0.5) << " " << (z_f - 0.5)
          << "      vertex " << (x_f - 0.5) << " " << (y_f + 0.5) << " " << (z_f + 0.5)
          << "    endloop" << std::endl
          << "  endfacet" << std::endl;
    });
  }

  /// normal in x
  if( x+1 == max_x || current_data().ImageStack->pixelAt <uchar>(z,x+1,y) == Color::BLACK_SOLID ) {
    write_queue().sync([&]{
      output()
          << "  facet normal  1.000000e+00  0.000000e+00  0.000000e+00" << std::endl
          << "    outer loop" << std::endl
          << "      vertex " << (x_f + 0.5) << " " << (y_f - 0.5) << " " << (z_f - 0.5)
          << "      vertex " << (x_f + 0.5) << " " << (y_f - 0.5) << " " << (z_f + 0.5)
          << "      vertex " << (x_f + 0.5) << " " << (y_f + 0.5) << " " << (z_f + 0.5)
          << "    endloop" << std::endl
          << "  endfacet" << std::endl
          << "  facet normal  1.000000e+00  0.000000e+00  0.000000e+00" << std::endl
          << "    outer loop" << std::endl
          << "      vertex " << (x_f + 0.5) << " " << (y_f - 0.5) << " " << (z_f - 0.5)
          << "      vertex " << (x_f + 0.5) << " " << (y_f + 0.5) << " " << (z_f - 0.5)
          << "      vertex " << (x_f + 0.5) << " " << (y_f + 0.5) << " " << (z_f + 0.5)
          << "    endloop" << std::endl
          << "  endfacet" << std::endl;
    });
  }

  /// normal in -y
  if( y == 0 || current_data().ImageStack->pixelAt <uchar>(z,x,y-1) == Color::BLACK_SOLID ) {
    write_queue().sync([&]{
      output()
        << "  facet normal  0.000000e+00  -1.000000e+00  0.000000e+00" << std::endl
        << "    outer loop" << std::endl
        << "      vertex " << (x_f - 0.5) << " " << (y_f - 0.5) << " " << (z_f - 0.5)
        << "      vertex " << (x_f - 0.5) << " " << (y_f - 0.5) << " " << (z_f + 0.5)
        << "      vertex " << (x_f + 0.5) << " " << (y_f - 0.5) << " " << (z_f + 0.5)
        << "    endloop" << std::endl
        << "  endfacet" << std::endl
        << "  facet normal  0.000000e+00  -1.000000e+00  0.000000e+00" << std::endl
        << "    outer loop" << std::endl
        << "      vertex " << (x_f - 0.5) << " " << (y_f - 0.5) << " " << (z_f - 0.5)
        << "      vertex " << (x_f + 0.5) << " " << (y_f - 0.5) << " " << (z_f - 0.5)
        << "      vertex " << (x_f + 0.5) << " " << (y_f - 0.5) << " " << (z_f + 0.5)
        << "    endloop" << std::endl
        << "  endfacet" << std::endl;
    });
  }

  /// normal in y
  if( y+1 == max_y || current_data().ImageStack->pixelAt <uchar>(z,x,y+1) == Color::BLACK_SOLID ) {
    write_queue().sync([&]{
      output()
        << "  facet normal  0.000000e+00  1.000000e+00  0.000000e+00" << std::endl
        << "    outer loop" << std::endl
        << "      vertex " << (x_f - 0.5) << " " << (y_f + 0.5) << " " << (z_f - 0.5)
        << "      vertex " << (x_f - 0.5) << " " << (y_f + 0.5) << " " << (z_f + 0.5)
        << "      vertex " << (x_f + 0.5) << " " << (y_f + 0.5) << " " << (z_f + 0.5)
        << "    endloop" << std::endl
        << "  endfacet" << std::endl
        << "  facet normal  0.000000e+00  1.000000e+00  0.000000e+00" << std::endl
        << "    outer loop" << std::endl
        << "      vertex " << (x_f - 0.5) << " " << (y_f + 0.5) << " " << (z_f - 0.5)
        << "      vertex " << (x_f + 0.5) << " " << (y_f + 0.5) << " " << (z_f - 0.5)
        << "      vertex " << (x_f + 0.5) << " " << (y_f + 0.5) << " " << (z_f + 0.5)
        << "    endloop" << std::endl
        << "  endfacet" << std::endl;
    });
  }

  /// normal in -z
  if( z == 0 || current_data().ImageStack->pixelAt <uchar>(z-1,x,y) == Color::BLACK_SOLID ) {
    write_queue().sync([&]{
      output()
        << "  facet normal  0.000000e+00  0.000000e+00  -1.000000e+00" << std::endl
        << "    outer loop" << std::endl
        << "      vertex " << (x_f - 0.5) << " " << (y_f - 0.5) << " " << (z_f - 0.5)
        << "      vertex " << (x_f - 0.5) << " " << (y_f + 0.5) << " " << (z_f - 0.5)
        << "      vertex " << (x_f + 0.5) << " " << (y_f + 0.5) << " " << (z_f - 0.5)
        << "    endloop" << std::endl
        << "  endfacet" << std::endl
        << "  facet normal  0.000000e+00  0.000000e+00  -1.000000e+00" << std::endl
        << "    outer loop" << std::endl
        << "      vertex " << (x_f - 0.5) << " " << (y_f - 0.5) << " " << (z_f - 0.5)
        << "      vertex " << (x_f + 0.5) << " " << (y_f - 0.5) << " " << (z_f - 0.5)
        << "      vertex " << (x_f + 0.5) << " " << (y_f + 0.5) << " " << (z_f - 0.5)
        << "    endloop" << std::endl
        << "  endfacet" << std::endl;
    });
  }

  /// normal in z
  if( z+1 == max_z || current_data().ImageStack->pixelAt <uchar>(z+1,x,y) == Color::BLACK_SOLID ) {
    write_queue().sync([&]{
      output()
        << "  facet normal  0.000000e+00  0.000000e+00  1.000000e+00" << std::endl
        << "    outer loop" << std::endl
        << "      vertex " << (x_f - 0.5) << " " << (y_f - 0.5) << " " << (z_f + 0.5)
        << "      vertex " << (x_f - 0.5) << " " << (y_f + 0.5) << " " << (z_f + 0.5)
        << "      vertex " << (x_f + 0.5) << " " << (y_f + 0.5) << " " << (z_f + 0.5)
        << "    endloop" << std::endl
        << "  endfacet" << std::endl
        << "  facet normal  0.000000e+00  0.000000e+00  1.000000e+00" << std::endl
        << "    outer loop" << std::endl
        << "      vertex " << (x_f - 0.5) << " " << (y_f - 0.5) << " " << (z_f + 0.5)
        << "      vertex " << (x_f + 0.5) << " " << (y_f - 0.5) << " " << (z_f + 0.5)
        << "      vertex " << (x_f + 0.5) << " " << (y_f + 0.5) << " " << (z_f + 0.5)
        << "    endloop" << std::endl
        << "  endfacet" << std::endl;
    });
  }

}
