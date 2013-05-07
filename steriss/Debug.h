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

#ifndef DEBUG_H
#define DEBUG_H

#include <boost/format.hpp>
#include <ciso646>

class Debug {

public:
    enum Level {
        S_WARNING,
        S_INFO,
        S_DEBUG,
        S_ERROR
    };

    static void print(Level, const std::string&);

};

#define _STERISS_IDENT "[STERISS]  "

#define sERROR(F, ...) { Debug::print( Debug::S_ERROR, boost::str( boost::format(_STERISS_IDENT F) __VA_ARGS__ ) ); throw std::exception(); }
#define sWARNING(F, ...) Debug::print( Debug::S_WARNING, boost::str( boost::format(_STERISS_IDENT F) __VA_ARGS__ ) )
#define sDEBUG(F, ...) Debug::print( Debug::S_DEBUG, boost::str( boost::format(_STERISS_IDENT F) __VA_ARGS__ ) )
#define sINFO(F, ...) Debug::print( Debug::S_INFO, boost::str( boost::format(_STERISS_IDENT F) __VA_ARGS__ ) )

#endif // DEBUG_H
