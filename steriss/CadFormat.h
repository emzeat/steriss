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

#ifndef CAD_FORMAT_H_
#define CAD_FORMAT_H_

#include "Types.h"

#include <fstream>
#include <iostream>
#include <xdispatch/dispatch>

class CadFormat {
  public:

    CadFormat(const std::string& directory);
    virtual ~CadFormat();

    virtual void open();
    virtual void close();
    virtual void writeData(const CrackData&);

  protected:
    virtual void write_voxel(size_t x, size_t y, size_t z) = 0;
    virtual void open_output(std::ofstream&, const std::string& path) = 0;
    virtual void on_open(){}
    virtual void on_close(){}

    virtual std::string format_name() const = 0;
    virtual std::string format_stem() const = 0;

    inline CrackData& current_data(){
      return _cd;
    }

    inline bool active() const {
      return _active;
    }

    inline xdispatch::queue& write_queue() {
      return _write_queue;
    }

    inline std::ofstream& output() {
      return _output;
    }

  private:
    CrackData _cd;
    bool _active;
    xdispatch::queue _write_queue;
    std::ofstream _output;
    std::string _directory;

};

typedef boost::shared_ptr<CadFormat> CadFormatPtr;

#endif /* CAD_FORMAT_H_ */
