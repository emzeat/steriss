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



#include "IO.h"
#include "Debug.h"
#include "Stl.h"

#include <opencv2/highgui/highgui.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <gflags/gflags.h>


DEFINE_string(input, "", "Input directory containing the PGM files");
DEFINE_string(output, "", "Output directory where the created PGM files will be written to");
DEFINE_string(output_stem, "pgm", "The filetype of the output images");
DEFINE_bool(voxelfile, false, "Enable to generate an STL file with the 3D object");
DEFINE_int64(overlay_color, 000000000, "Set a color overlay in here");

IO::IO(const std::list<std::string> &input_stems) {

  if( FLAGS_input.empty() )
    sERROR("You need to set an input directory using the '-input' flag");

  if( FLAGS_output.empty() )
    sERROR("You need to set an output directory using the '-output' flag");

  boost::filesystem::path input_path( FLAGS_input );
  if(not boost::filesystem::exists( input_path) )
    sERROR("The given input path '%s' does not exist", %FLAGS_input);

  _origin = input_path.stem ().string();
  sINFO("Reading images from directory '%s'", %_origin );

  boost::filesystem::directory_iterator it( input_path );
  boost::filesystem::directory_iterator end;

  std::list<std::string> files;
  while(it != end){
    boost::filesystem::directory_entry file = *it;
    std::string file_type = file.path ().extension ().string();

    auto find_result = std::find( input_stems.begin(), input_stems.end(), file_type );
    if( find_result != input_stems.end() )
      files.push_back ( file.path ().string() );
    else
      sWARNING("Skipping file %s", %file.path ().filename () );
    it++;
  }
  files.sort ();

  _files = std::vector<std::string>( files.size() );
  size_t i = 0;
  BOOST_FOREACH( std::string& f, files )
    _files[i++] = f;

  if( _files.size() > 0 ){
    cv::Mat color_img = cv::imread ( _files[0] );
    cv::Mat gray_img;
    cv::cvtColor( color_img, gray_img, CV_RGB2GRAY );
    _image_size.width = gray_img.rows;
    _image_size.height = gray_img.cols;
    _image_type = gray_img.type();
  } else {
    sERROR("No files in input directory, aborting");
  }

  sINFO("Found %i files with dimensions %ix%i px", %_files.size() %_image_size.width %_image_size.height);

  _cad_output.reset( new StlAscii( FLAGS_voxelfile ? FLAGS_output : "" ) );
  _cad_output->open();
}

IO::~IO() {
  _cad_output->close();
}

size_t IO::imageCount() const {
  return _files.size();
}

const cv::Size& IO::imageSize() const {
  return _image_size;
}

const int IO::imageType() const {
  return _image_type;
}

CrackData IO::readImages (size_t from, size_t to) const {
  assert( from < to );

  CrackImageStackPtr cm_ptr(new CrackImageStack(_origin) );
  cm_ptr->resize ( to - from );

  size_t output_i = 0;
  for( size_t i = from; i < to; ++i ){
    boost::filesystem::path file( _files[i] );

    sDEBUG("Reading %s", %file.filename () );
    cv::Mat color_img = cv::imread ( file.string() );
    cv::cvtColor ( color_img, (*cm_ptr)[output_i], CV_RGB2GRAY );

    ++output_i;
  }

  return CrackData( cm_ptr );
}

void IO::writeImages (CrackData cm_ptr, size_t from, size_t to) {

  boost::filesystem::path output_path( FLAGS_output );
  boost::filesystem::create_directories( output_path );

  sINFO("Writing images to '%s'", %output_path.stem () );

  cv::Vec3b overlay_color;
  bool do_overlay = false;
  if( FLAGS_overlay_color == 0 )
    do_overlay = false;
  else {
    do_overlay = true;
    uchar r = (uchar)(FLAGS_overlay_color / 1000000 % 1000);
    uchar g = (uchar)(FLAGS_overlay_color / 1000 % 1000);
    uchar b = (uchar)(FLAGS_overlay_color % 1000);
    overlay_color = cv::Vec3b(b,g,r);

    sDEBUG("Result will be stored as overlay with color (%u,%u,%u)", %r %g %b);
  }

  sDEBUG("Using output stem %s", %FLAGS_output_stem);

  size_t index = from + 1;
  for(size_t i = 0; i < cm_ptr.ImageStack->size (); ++i) {
    boost::filesystem::path image_path = output_path;
    image_path /= boost::str(boost::format("%s_%04i.%s") %cm_ptr.ImageStack->origin () %index %FLAGS_output_stem );

    if( do_overlay )
      write_overlay( image_path.string(), index-1, cm_ptr.ImageStack->at(i), overlay_color );
    else
      write_binary( image_path.string(), index-1, cm_ptr.ImageStack->at(i) );
    sDEBUG("Writing %s", %image_path.filename ());

    ++index;
  }

  _cad_output->writeData( cm_ptr );
}

void IO::write_binary(const std::string &path, size_t index, const cv::Mat &image) const {
  cv::imwrite( path, image );
}

void IO::write_overlay(const std::string &path, size_t index, const cv::Mat &image, const cv::Vec3b& color) const {

  boost::filesystem::path original_file( _files[index] );
  cv::Mat output_image = cv::imread ( original_file.string() );

  for(int x = 0; x < output_image.cols; ++x) {
    for(int y = 0; y < output_image.rows; ++y) {
      if( image.at<uchar>(y,x) == Color::WHITE_CRACK )
        output_image.at<cv::Vec3b>(y,x) = color;
    }
  }

  cv::imwrite( path, output_image );
}
