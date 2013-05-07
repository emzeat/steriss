Steriss (v0.4)
==============

Steriss is a small utility created for automated segmentation
of cracks within images obtained using computer tomography.


Building
--------

Steriss has several dependencies:
 * a modern C++ compiler (gcc 4.7+ or vs 2010)
 * CMake 2.8.8 or later
 * XDispatch 0.7.x (install to external/xdispatch)
 * gflags 2.0 (install to svn:external/gflags)
 * boost 1.48+
 * opencv 2.4
 * Qt 4.8+
 
Make sure your development environment is set up properly.
Enter a shell and execute one of the generate scripts found within
the 'build' directory or generate the needed project files directly
from a unix compatible shell:

    cmake -G "Unix Makefiles" && make all


Usage
-----

Please see the file manual.pdf (German!) for details.


License
-------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.