# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findpyanalysis.cmake 723053 2016-02-10 11:00:16Z krasznaa $
#
# Sets:
#  PYANALYSIS_BINARY_PATH
#  PYANALYSIS_PYTHON_PATH
#
# Can be steered by PYANALYSIS_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# If it was already found, let's be quiet:
if( PYANALYSIS_FOUND )
   set( pyanalysis_FIND_QUIETLY TRUE )
endif()

# Find the binary path:
find_path( PYANALYSIS_BINARY_PATH isympy
   PATH_SUFFIXES bin PATHS ${PYANALYSIS_ROOT} )

# Find the python path:
find_path( PYANALYSIS_PYTHON_PATH numpy/__init__.py
   PATH_SUFFIXES lib/python2.7/site-packages
   PATHS ${PYANALYSIS_ROOT} )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( pyanalysis DEFAULT_MSG
   PYANALYSIS_PYTHON_PATH PYANALYSIS_BINARY_PATH )

# Set up the RPM dependency:
lcg_need_rpm( pyanalysis )
