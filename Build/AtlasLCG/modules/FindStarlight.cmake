# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindStarlight.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Module finding Starlight in the LCG release. Defines:
#  - STARLIGHT_FOUND
#  - STARLIGHT_INCLUDE_DIR
#  - STARLIGHT_INCLUDE_DIRS
#  - STARLIGHT_LIBRARIES
#  - STARLIGHT_LIBRARY_DIRS
#
# Can be steered by STARLIGHT_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Starlight
   INCLUDE_SUFFIXES include INCLUDE_NAMES starlight.h
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS Starlib )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Starlight DEFAULT_MSG STARLIGHT_INCLUDE_DIR
   STARLIGHT_LIBRARIES )
mark_as_advanced( STARLIGHT_FOUND STARLIGHT_INCLUDE_DIR STARLIGHT_INCLUDE_DIRS
   STARLIGHT_LIBRARIES STARLIGHT_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( starlight )
