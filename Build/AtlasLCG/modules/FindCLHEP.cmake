# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindCLHEP.cmake 741751 2016-04-20 18:57:53Z krasznaa $
#
# Try to find CLHEP
#
# Defines:
#
#  CLHEP_FOUND
#  CLHEP_VERSION
#  CLHEP_INCLUDE_DIR
#  CLHEP_INCLUDE_DIRS
#  CLHEP_<component>_LIBRARY
#  CLHEP_<component>_FOUND
#  CLHEP_LIBRARIES
#  CLHEP_LIBRARY_DIRS
#
# Can be steered by CLHEP_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# First just find the headers:
lcg_external_module( NAME CLHEP
   INCLUDE_SUFFIXES include INCLUDE_NAMES CLHEP/ClhepVersion.h )

# Extract a version for the found path:
if( CLHEP_INCLUDE_DIR )
   file( READ "${CLHEP_INCLUDE_DIR}/CLHEP/ClhepVersion.h"
      _CLHEP_VERSION_H_CONTENTS )
   set( _clhep_regex
      ".*static std::string String\\(\\) { return \"([^\"]*)\".*" )
   if( "${_CLHEP_VERSION_H_CONTENTS}" MATCHES ${_clhep_regex} )
      set( CLHEP_FOUND_VERSION ${CMAKE_MATCH_1} CACHE INTERNAL
         "Detected version of CLHEP" )
   endif()
endif()

# Now find the libraries:
lcg_external_module( NAME CLHEP
   DEFAULT_COMPONENTS CLHEP
   LIBRARY_SUFFIXES lib )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( CLHEP REQUIRED_VARS
   CLHEP_INCLUDE_DIR CLHEP_LIBRARIES
   VERSION_VAR CLHEP_FOUND_VERSION )
mark_as_advanced( CLHEP_FOUND CLHEP_INCLUDE_DIR CLHEP_INCLUDE_DIRS
   CLHEP_LIBRARIES CLHEP_LIBRARY_DIRS )

# In order to build against CLHEP, some extra definitions are needed:
set( CLHEP_DEFINITIONS -DCLHEP_MAX_MIN_DEFINED
   -DCLHEP_ABS_DEFINED -DCLHEP_SQR_DEFINED )
add_definitions( ${CLHEP_DEFINITIONS} )

# Set up the RPM dependency:
lcg_need_rpm( CLHEP )
