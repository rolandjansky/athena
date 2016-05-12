# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindTauola.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Module finding Tauola in the LCG release. Defines:
#  - TAUOLA_FOUND
#  - TAUOLA_INCLUDE_DIR
#  - TAUOLA_INCLUDE_DIRS
#  - TAUOLA_LIBRARIES
#  - TAUOLA_LIBRARY_DIRS
#
# Can be steered by TAUOLA_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Tauola
   INCLUDE_SUFFIXES include INCLUDE_NAMES hepevt.inc
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS tauola )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Tauola DEFAULT_MSG TAUOLA_INCLUDE_DIR
   TAUOLA_LIBRARIES )
mark_as_advanced( TAUOLA_FOUND TAUOLA_INCLUDE_DIR TAUOLA_INCLUDE_DIRS
   TAUOLA_LIBRARIES TAUOLA_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( tauola )
