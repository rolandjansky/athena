# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindRivet.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Module finding Rivet in the LCG release. Defines:
#  - RIVET_FOUND
#  - RIVET_INCLUDE_DIR
#  - RIVET_INCLUDE_DIRS
#  - RIVET_LIBRARIES
#  - RIVET_LIBRARY_DIRS
#
# Can be steered by RIVET_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Rivet
   INCLUDE_SUFFIXES include INCLUDE_NAMES Rivet/Rivet.hh
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS Rivet )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Rivet DEFAULT_MSG RIVET_INCLUDE_DIR
   RIVET_LIBRARIES )
mark_as_advanced( RIVET_FOUND RIVET_INCLUDE_DIR RIVET_INCLUDE_DIRS
   RIVET_LIBRARIES RIVET_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( rivet )
