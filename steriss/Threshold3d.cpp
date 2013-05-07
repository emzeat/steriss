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


#include "Threshold3d.h"
#include "Algorithm.h"
#include "Debug.h"

#include <gflags/gflags.h>
#include <list>

DEFINE_double(threshold, 117.6, "the threshold to use, valid range is 0.0:1000.0");
//DEFINE_int32(radius, 5, "the radius (in px) to consider around pixels below the threshold");
//DEFINE_double(sigma, 2.0, "the standard deviation used for weighting the pixels around sub-threshold pixels");
DEFINE_double(border_threshold, 235.3, "will be considered a crack whenever below the value and next to the crack");
DECLARE_bool(overlay);

inline static bool is_crack( const CrackCube& cube, size_t x, size_t y, size_t z ) {
    return cube[x][y][z] < FLAGS_threshold;
}

typedef std::list<CrackCube::Scalar> NeighbourList;


static void collect_neighbourhood( const CrackCube& cube, size_t x, size_t y, size_t z, bool search_3d, NeighbourList& neighbours ) {

    for(size_t zz = search_3d ? z-1 : z; zz <= z+1; ++zz) {
        for(size_t xx = x-1; xx <= x+1; ++xx) {
            for(size_t yy = y-1; yy <= y+1; ++yy) {
                if( xx > 0 && xx < cube.cols ()-1
                        && yy > 0 && yy < cube.rows ()-1
                        && zz > 0 && zz < cube.height ()-1 )
                    neighbours.push_back ( cube[xx][yy][zz] );
            }
        }

        if( not search_3d ) break;
    }

}

static bool is_crack_neighbour( const CrackCube& cube, size_t x, size_t y, size_t z, bool search_3d = true ) {

    if( cube[x][y][z] > FLAGS_border_threshold )
        return false;

    NeighbourList nb_list;
    collect_neighbourhood ( cube, x, y, z, search_3d, nb_list );

    for(auto it = nb_list.begin (); it != nb_list.end (); ++it)
        if( *it < FLAGS_threshold )
            return true;

    return false;
}


void Threshold2d::apply(CrackData& c_data, int x, int y, int z) const {
    //sDEBUG("(%i,%i,%i) Using threshold=%i border_threshold=%i", %x %y %z %FLAGS_threshold %FLAGS_border_threshold );

    uchar& output_px = c_data.ImageStack->pixelAt <uchar>(z,x,y);
    const CrackCube& cube = *(c_data.Cube);

    if( is_crack (cube, x, y, z) || is_crack_neighbour (cube, x, y, z, false) ) {
      output_px = Color::WHITE_CRACK;
    } else {
      output_px = Color::BLACK_SOLID;
    }
}


void Threshold3d::apply(CrackData &c_data, int x, int y, int z) const {
    //sDEBUG("(%i,%i,%i) Using threshold=%i border_threshold=%i", %x %y %z %FLAGS_threshold %FLAGS_border_threshold );

    uchar& output_px = c_data.ImageStack->pixelAt <uchar>(z,x,y);
    const CrackCube& cube = *(c_data.Cube);

    if( is_crack (cube, x, y, z) || is_crack_neighbour (cube, x, y, z, true) )
        output_px = Color::WHITE_CRACK;
    else
        output_px = Color::BLACK_SOLID;
}
