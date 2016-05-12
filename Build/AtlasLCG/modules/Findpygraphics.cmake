# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findpygraphics.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# - Simple module to add the pygraphics directory to the python path
#
# It also provides the functions gen_pyqt_resource and gen_pyqt_uic.
#
# The following variables are defined:
#
#   PYGRAPHICS_FOUND
#   PYGRAPHICS_PYTHON_PATH
#   PYGRAPHICS_BINARY_PATH
#
# Commands:
#
#   pyrcc_cmd
#   pyuic_cmd
#
# Can be steered by PYGRAPHICS_ROOT.
#

# First off, we need Python itself:
find_package( PythonInterp QUIET REQUIRED )

# Find the executable:
find_program( pyrcc_EXECUTABLE pyrcc4
   PATH_SUFFIXES bin PATHS ${PYGRAPHICS_ROOT} )

# Set the binary path:
get_filename_component( PYGRAPHICS_BINARY_PATH ${pyrcc_EXECUTABLE} PATH )

# Find the python path:
find_path( PYGRAPHICS_PYTHON_PATH pydot.py
   PATH_SUFFIXES lib/python2.7/site-packages
   PATHS ${PYGRAPHICS_ROOT} )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( pygraphics DEFAULT_MSG PYGRAPHICS_PYTHON_PATH
   pyrcc_EXECUTABLE )
mark_as_advanced( PYGRAPHICS_FOUND PYGRAPHICS_BINARY_PATH
   PYGRAPHICS_PYTHON_PATH )

# Set up the RPM dependency:
lcg_need_rpm( pygraphics )
