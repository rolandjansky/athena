# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindCORAL.cmake 721147 2016-02-01 15:24:46Z krasznaa $
#
# - Try to find CORAL
# Defines:
#
#  CORAL_FOUND
#  CORAL_INCLUDE_DIR
#  CORAL_INCLUDE_DIRS
#  CORAL_<component>_LIBRARY
#  CORAL_<component>_FOUND
#  CORAL_LIBRARIES
#  CORAL_LIBRARY_DIRS
#  CORAL_PYTHON_PATH
#  CORAL_BINARY_PATH
#
# Can be steered by CORAL_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME CORAL
   INCLUDE_SUFFIXES include INCLUDE_NAMES RelationalAccess/ConnectionService.h
   LIBRARY_SUFFIXES lib LIBRARY_PREFIX lcg_
   COMPULSORY_COMPONENTS CoralBase )

# Find the python directory:
find_path( CORAL_PYTHON_PATH coral.py
   PATH_SUFFIXES python PATHS ${CORAL_ROOT} )
set( CORAL_PYTHON_PATH ${CORAL_PYTHON_PATH}
   ${CORAL_LIBRARY_DIRS} )

# Find executables:
find_program( CORAL_coralServer_EXECUTABLE coralServer
   PATH_SUFFIXES bin PATHS ${CORAL_ROOT} )
mark_as_advanced( CORAL_coralServer_EXECUTABLE )
if( CORAL_coralServer_EXECUTABLE )
   get_filename_component( CORAL_BINARY_PATH ${CORAL_coralServer_EXECUTABLE}
      PATH )
endif()

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( CORAL DEFAULT_MSG CORAL_INCLUDE_DIR
   CORAL_LIBRARIES CORAL_PYTHON_PATH )
mark_as_advanced( CORAL_FOUND CORAL_INCLUDE_DIR CORAL_INCLUDE_DIRS
   CORAL_LIBRARIES CORAL_LIBRARY_DIRS CORAL_PYTHON_PATH )

# Set up the RPM dependency:
lcg_need_rpm( CORAL )
