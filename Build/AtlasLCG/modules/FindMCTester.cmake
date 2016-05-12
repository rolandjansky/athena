# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindMCTester.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Module finding MCTester in the LCG release. Defines:
#  - MCTESTER_FOUND
#  - MCTESTER_INCLUDE_DIR
#  - MCTESTER_INCLUDE_DIRS
#  - MCTESTER_<component>_FOUND
#  - MCTESTER_<component>_LIBRARY
#  - MCTESTER_LIBRARIES
#  - MCTESTER_LIBRARY_DIRS
#
# Can be steered by MCTESTER_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME MCTester
   INCLUDE_SUFFIXES include INCLUDE_NAMES HEPEVTParticle.H
   LIBRARY_SUFFIXES lib
   DEFAULT_COMPONENTS MCTester HEPEvent HepMCEvent )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( MCTester DEFAULT_MSG MCTESTER_INCLUDE_DIR
   MCTESTER_LIBRARIES )
mark_as_advanced( MCTESTER_FOUND MCTESTER_INCLUDE_DIR MCTESTER_INCLUDE_DIRS
   MCTESTER_LIBRARIES MCTESTER_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( mctester )
