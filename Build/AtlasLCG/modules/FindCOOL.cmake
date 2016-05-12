# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindCOOL.cmake 723149 2016-02-10 16:26:22Z krasznaa $
#
# - Try to find COOL
# Defines:
#
#  COOL_FOUND
#  COOL_INCLUDE_DIR
#  COOL_INCLUDE_DIRS
#  COOL_<component>_LIBRARY
#  COOL_<component>_FOUND
#  COOL_LIBRARIES
#  COOL_LIBRARY_DIRS
#  COOL_PYTHON_PATH
#  COOL_BINARY_PATH
#
# The script can be guided using COOL_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME COOL
   INCLUDE_SUFFIXES include INCLUDE_NAMES CoolKernel/IDatabase.h
   LIBRARY_SUFFIXES lib LIBRARY_PREFIX lcg_
   COMPULSORY_COMPONENTS CoolKernel CoolApplication )

# Find the python path:
find_path( COOL_PYTHON_PATH PyCool/__init__.py
   PATH_SUFFIXES python PATHS ${COOL_ROOT} )

# Find an executable:
find_program( COOL_ReplicateDB_EXECUTABLE coolReplicateDB
   PATH_SUFFIXES bin PATHS ${COOL_ROOT} )
mark_as_advanced( COOL_ReplicateDB_EXECUTABLE )
if( COOL_ReplicateDB_EXECUTABLE )
   get_filename_component( COOL_BINARY_PATH ${COOL_ReplicateDB_EXECUTABLE}
      PATH )
endif()

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( COOL DEFAULT_MSG COOL_INCLUDE_DIR
   COOL_LIBRARIES COOL_PYTHON_PATH )
mark_as_advanced( COOL_FOUND COOL_INCLUDE_DIR COOL_INCLUDE_DIRS
   COOL_LIBRARIES COOL_LIBRARY_DIRS COOL_PYTHON_PATH COOL_BINARY_PATH )

# Additional environment variable(s):
set( COOL_ENVIRONMENT
   SET COOL_DISABLE_CORALCONNECTIONPOOLCLEANUP "YES" )

# Set up the RPM dependency:
lcg_need_rpm( COOL )
