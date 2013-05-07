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


#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <boost/function.hpp>

#include "Types.h"

class Algorithm {
public:

    inline static void localOperation(cv::Mat& image, const boost::function<void(int,int)> operation) {
        for(int x = 0; x < image.rows; ++x)
            for(int y = 0; y < image.cols; ++y)
                operation(x,y);
    }

    static void median(cv::Mat& image, int ksize) {
        cv::medianBlur ( image, image, ksize );
    }

    static void laplacian(cv::Mat& image, int ksize) {
        cv::Mat laplace_out = image.clone ();
        cv::Laplacian ( image, laplace_out, image.type (), ksize, 1, 0, cv::BORDER_DEFAULT );

        image += laplace_out;
    }

    template <typename _ImageType>
    static void stretchToFillDomain(CrackData& data) {

        int rows = data.ImageStack->front ().rows;
        int cols = data.ImageStack->front ().cols;
        size_t height = data.ImageStack->size ();

        data.Cube.reset ( new CrackCube( rows, cols, height ) );

        xdispatch::global_queue ().apply ([&](size_t z){
          cv::Mat& image = data.ImageStack->operator [] (z);

          CrackCube::Scalar min = CrackCube::maxValue (), max = CrackCube::minValue ();
          for(int x = 0; x < image.rows; ++x)
			for(int y = 0; y < image.cols; ++y){
              min = std::min( min, (CrackCube::Scalar)image.at<_ImageType>(x,y) );
              max = std::max( max, (CrackCube::Scalar)image.at<_ImageType>(x,y) );
          }

          CrackCube::Scalar span = std::abs ( max - min );
          for(int x = 0; x < image.rows; ++x)
			for(int y = 0; y < image.cols; ++y){
             (*(data.Cube))[x][y][z] = ((CrackCube::Scalar)image.at<_ImageType>(x,y)) * CrackCube::maxValue () / span - min;
          }
        }, height);
    }

};

#endif /* ALGORITHM_H_ */
