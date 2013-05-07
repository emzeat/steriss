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


#ifndef TYPES_H_
#define TYPES_H_

#include <vector>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/multi_array.hpp>
#include <xdispatch/dispatch>

struct Color {
    static uchar BLACK_SOLID;
    static uchar WHITE_CRACK;
    static cv::Vec3b GREEN;
};

struct CrackImageStack;
template <typename _Scalar, int _dimension> struct _CrackCube;
typedef _CrackCube<float, 3> CrackCube;

typedef boost::shared_ptr<CrackImageStack> CrackImageStackPtr;
typedef boost::shared_ptr<CrackCube> CrackCubePtr;

struct CrackData {
    CrackData(const CrackImageStackPtr& cis )
        : ImageStack( cis ), Cube() {}
    CrackData()
        : ImageStack(), Cube() {}

    CrackImageStackPtr ImageStack;
    CrackCubePtr Cube;

    inline void reset() {
      ImageStack.reset();
      Cube.reset();
    }
};

struct CrackImageStack : public std::vector<cv::Mat> {

    CrackImageStack(const std::string&);
    CrackImageStack(const std::string &, size_t);

    typedef std::vector<cv::Mat> BaseType;
    typedef boost::function<void(cv::Mat&)> ImageFunction;

    std::string& origin();
    void apply(const ImageFunction& );
    CrackImageStackPtr clone();

    enum ImageType {
      BINARY_IMAGE,
      OVERLAY_IMAGE
    };

    template<typename _T> _T& pixelAt (int z, int x, int y) {
        return this->operator [] (z).at<_T>(x,y);
    }

    inline ImageType& imageType(){
      return _type;
    }

private:
    void apply_handler(const ImageFunction&, size_t);

    std::string _origin;
    ImageType _type;

};

template <typename _Scalar, int _dimension = 3>
struct _CrackCube : public boost::multi_array< _Scalar, _dimension > {

    typedef _Scalar Scalar;
    typedef boost::multi_array< _Scalar, _dimension > BaseType;
    typedef _CrackCube<_Scalar, _dimension> Type;
    typedef boost::function<void(int,int,int)> CubeFunction;
    enum {
        Dimension = _dimension
    };

    inline static Scalar maxValue(){ return (Scalar)1000.; }
    inline static Scalar minValue(){ return (Scalar)0.; }

    _CrackCube(size_t x, size_t y, size_t z)
        : BaseType(boost::extents[x][y][z]), _height(z), _rows(y), _cols(x) {
        assert(x > 0 && y > 0 && z > 0);
    }

    void apply(const CubeFunction& cfunc ) {
        xdispatch::global_queue ().apply (boost::bind( &Type::apply_handler, this, boost::ref(cfunc), _1 ), _height );
    }

    inline size_t height() const { return _height; }
    inline size_t rows() const { return _rows; }
    inline size_t cols() const { return _cols; }

private:
    void apply_handler(const CubeFunction& cfunc, size_t z) {
        for(size_t x = 0; x < _cols; ++x )
            for(size_t y = 0; y < _rows; ++y )
                cfunc( x, y, z );
    }

    size_t _height, _rows, _cols;
};

#endif /* TYPES_H_ */
