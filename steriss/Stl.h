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

#ifndef STL_H_
#define STL_H_

#include "CadFormat.h"

class StlAscii : public CadFormat {
  public:

    StlAscii(const std::string& directory);
    ~StlAscii();

  protected:
    std::string format_name() const { return "STL"; }
    std::string format_stem() const { return "stl"; }

    void open_output(std::ofstream &, const std::string &path);
    void on_open();
    void on_close();
    void write_voxel(size_t x, size_t y, size_t z);


};

#endif /* STL_H_ */
