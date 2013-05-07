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


#include "Steriss.h"
#include "IO.h"
#include "Types.h"
#include "Algorithm.h"
#include "Threshold3d.h"
#include "JobSplitter.h"

#include <boost/bind.hpp>
#include <gflags/gflags.h>
#include <exception>

DEFINE_string(algorithm, "plain", "Defines the algorithm to use, currently valid choices are: plain, cube");
DEFINE_int32(median_size, 3, "The aperture linear size used for median preprocessing. Must be odd and greater than 1: 3,5,7,...");

Steriss::Steriss() {
  sDEBUG("Steriss instanciated");

  // choose the selected algorithm
  if( FLAGS_algorithm == "plain" ) {
    sINFO("'plain' was selected as algorithm");
    _algorithm.reset( new Threshold2d() );
  } else if( FLAGS_algorithm == "cube" ) {
    sINFO("'cube' was selected as algorithm");
    _algorithm.reset( new Threshold3d() );
  } else {
    sERROR("'%s' is an unkown algorithm", %FLAGS_algorithm );
  }
}

int Steriss::exec () {
  try {
    std::list<std::string> valid_stems;
    valid_stems.push_back(".tiff");
    valid_stems.push_back(".tif");
    valid_stems.push_back(".bmp");
    valid_stems.push_back(".pgm");
    IO input_images( valid_stems );
    JobSplitter jobs( input_images );

    while( jobs.hasNext() ) {
      // read images
      _current_data = jobs.nextJob();
      sINFO(">> Running job %i of %i", %jobs.currentJob() %jobs.jobCt());

      // do the processing
      preprocess_data ();
      transfer_data ();
      process_data ();

      // write images
      input_images.writeImages( _current_data, jobs.currentStartIndex(), jobs.currentEndIndex() );

      // make sure to release the memory for the next turn
      _current_data.reset();
    }

    sINFO("All done. Now it's up to you - I'll have a break...");
    return EXIT_SUCCESS;
  } catch(std::exception& e) {
    return EXIT_FAILURE;
  }
}

void Steriss::preprocess_data () {
  sINFO("Preprossing raw image data");

  sDEBUG("Smoothing using a median filter");
  _current_data.ImageStack->apply ( boost::bind( Algorithm::median, _1, FLAGS_median_size ) );
  // sDEBUG("Increasing contrast by adding a laplacian filter");
  // _current_data.ImageStack->apply ( boost::bind( Algorithm::laplacian, _1, FLAGS_laplace_size ) );
}

void Steriss::transfer_data () {
  sINFO("Preparing data for analysis");

  // span the image to use entire data range
  if( _current_data.ImageStack->at (0).type() == CV_8U ) {
    sDEBUG("Image type is CV_8U / PGM");
    Algorithm::stretchToFillDomain<uint8_t> ( _current_data );
  } else if( _current_data.ImageStack->at(0).type() == CV_16U ) {
    sDEBUG("Image type is CV_16U / TIFF,BMP");
    Algorithm::stretchToFillDomain<uint16_t> ( _current_data );
  } else {
    sWARNING("Incompatible Image Type");
    throw std::exception();
  }
}

void Steriss::process_data () {
  sINFO("Processing data");
  sDEBUG("Initializing output images");

  int rows = _current_data.ImageStack->front ().rows;
  int cols = _current_data.ImageStack->front ().cols;
  for(size_t i = 0; i < _current_data.ImageStack->size (); ++i)
    // the output image is always a pgm file, 8bit unsigned
    _current_data.ImageStack->operator [] (i) = cv::Mat( rows, cols, CV_8U );

  // apply the selected algorithm
  _current_data.Cube->apply ( boost::bind( &CrackAlgorithm::apply, _algorithm, boost::ref(_current_data), _1, _2, _3 ) );
}
