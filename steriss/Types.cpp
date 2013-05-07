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


#include "Types.h"

uchar Color::BLACK_SOLID = std::numeric_limits<uchar>::min();
uchar Color::WHITE_CRACK = std::numeric_limits<uchar>::max();
cv::Vec3b Color::GREEN = cv::Vec3b(0,255,0);


CrackImageStack::CrackImageStack(const std::string & origin)
    : BaseType(), _origin(origin) {
}

CrackImageStack::CrackImageStack(const std::string & origin, size_t i)
    : BaseType(i), _origin(origin) {

}

void CrackImageStack::apply (const ImageFunction &cfunc) {

    xdispatch::global_queue ().apply (boost::bind(&CrackImageStack::apply_handler, this, boost::ref(cfunc), _1 ), this->size ());
}


void CrackImageStack::apply_handler (const ImageFunction &cfunc, size_t i) {
    cfunc( this->operator [] (i) );
}

std::string& CrackImageStack::origin(){
    return _origin;
}

CrackImageStackPtr CrackImageStack::clone () {
    CrackImageStackPtr cs_ptr( new CrackImageStack(this->origin (), this->size ()) );

    for(size_t i = 0; i < this->size (); ++i)
        (*cs_ptr)[i] = this->operator [] (i).clone();

    return cs_ptr;
}
