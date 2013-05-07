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

#ifndef JOB_SPLITTER_H_
#define JOB_SPLITTER_H_

#include "IO.h"

class JobSplitter {

  public:
    JobSplitter(const IO& io);

    CrackData nextJob();
    bool hasNext();

    inline int currentJob() const {
      return _current_job;
    }

    inline int jobCt() const {
      return _job_ct;
    }

    size_t currentStartIndex() const;
    size_t currentEndIndex() const;

  private:
    const IO& _io;
    bool _started;
    int _job_ct;
    int _images_per_job;
    int _current_job;

    static const int overlap = 1;

};

#endif /* JOB_SPLITTER_H_ */
