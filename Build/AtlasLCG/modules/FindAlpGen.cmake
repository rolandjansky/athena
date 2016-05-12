# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindAlpGen.cmake 727419 2016-03-03 12:36:58Z krasznaa $
#
# Module finding AlpGen in the LCG release. Defines:
#  - ALPGEN_FOUND
#  - ALPGEN_LIBRARIES
#  - ALPGEN_LIBRARY_DIRS
#
# Can be steered by ALPGEN_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME AlpGen
   INCLUDE_SUFFIXES share/sources INCLUDE_NAMES 2Qlib/2Q.inc 4Qlib/4Q.inc
   LIBRARY_SUFFIXES lib
   DEFAULT_COMPONENTS alpgen )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( AlpGen DEFAULT_MSG ALPGEN_INCLUDE_DIR
   ALPGEN_LIBRARIES )
mark_as_advanced( ALPGEN_FOUND ALPGEN_LIBRARIES ALPGEN_LIBRARY_DIRS )

# Set additional environment variable(s):
find_path( ALPGENPATH NAMES 2Qlib/2Q.inc 4Qlib/4Q.inc
   PATHS ${ALPGEN_ROOT} PATH_SUFFIXES share/sources )
if( ALPGENPATH )
   set( ALPGEN_ENVIRONMENT
      SET ALPGENPATH ${ALPGENPATH} )
endif()

# Set up the RPM dependency:
lcg_need_rpm( alpgen )
