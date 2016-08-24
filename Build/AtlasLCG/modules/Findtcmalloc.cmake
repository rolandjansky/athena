# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findtcmalloc.cmake 769797 2016-08-24 10:03:31Z krasznaa $
#
# - Locate tcmalloc library
# Defines:
#
#  TCMALLOC_FOUND
#  TCMALLOC_INCLUDE_DIR
#  TCMALLOC_INCLUDE_DIRS
#  TCMALLOC_tcmalloc_LIBRARY
#  TCMALLOC_profiler_LIBRARY
#  TCMALLOC_LIBRARIES
#  TCMALLOC_LIBRARY_DIRS
#  TCMALLOC_BINARY_PATH
#  PPROF_EXECUTABLE
#
# Can be steered using TCMALLOC_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME tcmalloc
   INCLUDE_SUFFIXES include
   INCLUDE_NAMES gperftools/tcmalloc.h google/tcmalloc.h
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS tcmalloc profiler )

# Find the pprof executable:
find_program( PPROF_EXECUTABLE NAMES pprof
   PATH_SUFFIXES bin PATHS ${TCMALLOC_ROOT} )
get_filename_component( TCMALLOC_BINARY_PATH ${PPROF_EXECUTABLE} PATH )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( tcmalloc DEFAULT_MSG
   TCMALLOC_INCLUDE_DIR TCMALLOC_LIBRARIES )
mark_as_advanced( TCMALLOC_FOUND TCMALLOC_INCLUDE_DIR TCMALLOC_INCLUDE_DIRS
   TCMALLOC_LIBRARIES TCMALLOC_LIBRARY_DIRS PPROF_EXECUTABLE
   TCMALLOC_BINARY_PATH )

# Set up the RPM dependency:
lcg_need_rpm( gperftools FOUND_NAME TCMALLOC VERSION_NAME GPERFTOOLS )
