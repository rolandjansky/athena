# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findpytools.cmake 722366 2016-02-08 10:06:10Z krasznaa $
#
# Sets:
#  PYTOOLS_BINARY_PATH
#  PYTOOLS_PYTHON_PATH
#
# Can be steered by PYTOOLS_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# If it was already found, let's be quiet:
if( PYTOOLS_FOUND )
   set( pytools_FIND_QUIETLY TRUE )
endif()

# Find the binary path:
find_path( PYTOOLS_BINARY_PATH ipython
   PATH_SUFFIXES bin PATHS ${PYTOOLS_ROOT} )

# Find the python path:
find_path( PYTOOLS_PYTHON_PATH mock.py
   PATH_SUFFIXES lib/python2.7/site-packages
   PATHS ${PYTOOLS_ROOT} )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( pytools DEFAULT_MSG
   PYTOOLS_PYTHON_PATH PYTOOLS_BINARY_PATH )

# Set up the RPM dependency:
lcg_need_rpm( pytools )
