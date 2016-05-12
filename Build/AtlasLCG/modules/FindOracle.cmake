# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindOracle.cmake 723149 2016-02-10 16:26:22Z krasznaa $
#
# - Locate Oracle library
# Defines:
#
#  ORACLE_FOUND
#  ORACLE_INCLUDE_DIR
#  ORACLE_INCLUDE_DIRS
#  ORACLE_LIBRARY
#  ORACLE_LIBRARIES
#  ORACLE_<component>_LIBRARY
#  ORACLE_<component>_FOUND
#  ORACLE_LIBRARY_DIRS
#  ORACLE_BINARY_PATH
#  SQLPLUS_EXECUTABLE
#
# The script can be steered by ORACLE_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Oracle
   INCLUDE_SUFFIXES include INCLUDE_NAMES oci.h
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS clntsh occi )

# Find some executable(s):
find_program( SQLPLUS_EXECUTABLE NAMES sqlplus
   PATH_SUFFIXES bin PATHS ${ORACLE_ROOT} )
if( SQLPLUS_EXECUTABLE )
   get_filename_component( ORACLE_BINARY_PATH ${SQLPLUS_EXECUTABLE}
      PATH )
endif()

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Oracle DEFAULT_MSG ORACLE_INCLUDE_DIR
   ORACLE_LIBRARIES )
mark_as_advanced( ORACLE_FOUND ORACLE_INCLUDE_DIR ORACLE_INCLUDE_DIRS
   ORACLE_LIBRARIES ORACLE_LIBRARY_DIRS SQLPLUS_EXECUTABLE
   ORACLE_BINARY_PATH )

# Set additional environment variables:
set( ORACLE_ENVIRONMENT
   SET ORA_FPU_PRECISION "EXTENDED" )

# Set up the RPM dependency:
lcg_need_rpm( oracle )
