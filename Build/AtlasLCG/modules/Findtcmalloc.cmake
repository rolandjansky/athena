# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findtcmalloc.cmake 718732 2016-01-20 12:30:12Z krasznaa $
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

# tcmalloc is picked up from AtlasExternals, so let's not set up an LCG
# RPM dependency for it.
