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

#ifndef STERISS_H
#define STERISS_H

#include "Debug.h"
#include "Types.h"
#include "CrackAlgorithm.h"

class Steriss
{
public:
    Steriss();

    int exec();

private:
    CrackData _current_data;
    CrackAlgorithmPtr _algorithm;

    void preprocess_data();
    void transfer_data();
    void process_data();
};

#endif // STERISS_H
