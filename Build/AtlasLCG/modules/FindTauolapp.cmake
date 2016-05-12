# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindTauolapp.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Module finding Tauola++ in the LCG release. Defines:
#  - TAUOLAPP_FOUND
#  - TAUOLAPP_INCLUDE_DIR
#  - TAUOLAPP_INCLUDE_DIRS
#  - TAUOLAPP_LIBRARIES
#  - TAUOLAPP_LIBRARY_DIRS
#
# Can be steered by TAUOLAPP_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Tauolapp
   INCLUDE_SUFFIXES include INCLUDE_NAMES Tauola/Tauola.h
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS TauolaCxxInterface )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Tauolapp DEFAULT_MSG TAUOLAPP_INCLUDE_DIR
   TAUOLAPP_LIBRARIES )
mark_as_advanced( TAUOLAPP_FOUND TAUOLAPP_INCLUDE_DIR TAUOLAPP_INCLUDE_DIRS
   TAUOLAPP_LIBRARIES TAUOLAPP_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( tauola++ FOUND_NAME TAUOLAPP )
