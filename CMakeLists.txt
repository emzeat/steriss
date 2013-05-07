#
# Copyright (c) 2012 Marius Zwicker
# All rights reserved.
# 
# @LICENSE_HEADER_START:GPL@
# 
# This file is part of Steriss.
# 
# Steriss is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# Steriss is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#  
# You should have received a copy of the GNU General Public License
# along with Steriss. If not, see <http://www.gnu.org/licenses/>.
# 
# @LICENSE_HEADER_END:GPL@
#
cmake_minimum_required(VERSION 2.8)

# project name
project(Steriss)

# includes
include(build/global.cmake)

# We need to ouput everything into the same directory
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "Library output path")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "Executable output path")
message("-- Setting binary output path: ${CMAKE_BINARY_DIR}/bin")

# externals
# mz_find_include_library <name>  SYS <version> SRC <directory> <include_dir> <target>
find_package( OpenCV COMPONENTS core imgproc highgui REQUIRED )
mz_find_include_library( xdispatch
        SYS 0.6
        SRC external/xdispatch external/xdispatch/include xdispatch
)
if(NOT APPLE)
 set(DISPATCH_LIBRARIES dispatch)
endif()
mz_add_library( gflags external/gflags )
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package( Boost 1.46.1 COMPONENTS filesystem system date_time REQUIRED )
find_package(Qt4 COMPONENTS QtGui QtMain QtCore)
include(${QT_USE_FILE})

if(WIN32)
    include_directories(external/gflags/src/windows)
else()
    include_directories(external/gflags/src)
endif()
include_directories(
    ${Boost_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
    ${XDISPATCH_INCLUDE_DIRS}
    ${QT_QTCORE_INCLUDE_DIR}
    ${QT_QTGUI_INCLUDE_DIR}
    ${QT_INCLUDE_DIR}
)
link_directories(
	${Boost_LIBRARY_DIRS}
)

# targets
file(GLOB steriss_src steriss/*.h steriss/*.cpp)
add_executable( steriss ${steriss_src} )
if(APPLE)
elseif(UNIX)
    set(LIBS statgrab)
endif()
target_link_libraries( steriss ${OpenCV_LIBS} ${XDISPATCH_LIBRARIES} ${DISPATCH_LIBRARIES} gflags ${Boost_LIBRARIES} ${LIBS} )
mz_target_props( steriss )


file(GLOB steriss_gui_src steriss_gui/*.h steriss_gui/*.cpp)
file(GLOB steriss_ui steriss_gui/*.ui)
file(GLOB steriss_res steriss_gui/*.qrc)
file(GLOB steriss_ts steriss_gui/*.ts)
qt4_wrap_ui(UI_H ${steriss_ui})
qt4_add_resources(RES ${steriss_res})
qt4_add_translation(LANG ${steriss_ts})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
mz_auto_moc(MOC ${steriss_gui_src})
add_executable( steriss_gui WIN32 ${steriss_gui_src} ${steriss_ui} ${steriss_res} ${steriss_ts} ${MOC} ${UI_H} ${RES} ${LANG})
target_link_libraries( steriss_gui ${QT_LIBRARIES} ${XDISPATCH_LIBRARIES} ${DISPATCH_LIBRARIES} gflags ${Boost_LIBRARIES} )
mz_target_props( steriss_gui )
