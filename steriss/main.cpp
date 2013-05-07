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


#include <gflags/gflags.h>
#include <limits>

#include "Steriss.h"

int main(int argc, char **argv) {

    // gflag config
    google::SetVersionString ("0.4\n(c) 2012 Marius Zwicker - All rights reserved");
    std::string usage_str = "Program for detecting cracks in REM images. Sample usage:\n";
    usage_str += std::string(argv[0]) + " -input /dir1 -output /dir2 -threshold 0.3";
    google::SetUsageMessage (usage_str );

    // parse custom gflags
    google::ParseCommandLineFlags( &argc, &argv, true );

    // Create a steriss object
    Steriss s;

    // and run it
    return s.exec ();
}

