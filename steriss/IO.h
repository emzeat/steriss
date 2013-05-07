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


#ifndef IO_H_
#define IO_H_

#include "Types.h"
#include "CadFormat.h"

#include <vector>
#include <list>
#include <string>

class IO {
public:
    IO(const std::list<std::string>& input_stems);
    ~IO();

    size_t imageCount() const;
    const cv::Size& imageSize() const;
    const int imageType() const;

    CrackData readImages(size_t from, size_t to) const ;
    void writeImages(CrackData, size_t from, size_t to);

  private:

    void write_binary(const std::string& path, size_t index, const cv::Mat &image) const;
    void write_overlay(const std::string& path, size_t index, const cv::Mat &image, const cv::Vec3b &color) const;

    std::vector<std::string> _files;
    std::string _origin;
    cv::Size _image_size;
    int _image_type;

    CadFormatPtr _cad_output;
};


#endif /* IO_H_ */
