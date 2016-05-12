# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindJimmy.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Module finding Jimmy in the LCG release. Defines:
#  - JIMMY_FOUND
#  - JIMMY_INCLUDE_DIR
#  - JIMMY_INCLUDE_DIRS
#  - JIMMY_LIBRARIES
#  - JIMMY_LIBRARY_DIRS
#
# Can be steered by JIMMY_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Jimmy
   INCLUDE_SUFFIXES include INCLUDE_NAMES jimmy.inc
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS jimmy )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Jimmy DEFAULT_MSG JIMMY_INCLUDE_DIR
   JIMMY_LIBRARIES )
mark_as_advanced( JIMMY_FOUND JIMMY_INCLUDE_DIR JIMMY_INCLUDE_DIRS
   JIMMY_LIBRARIES JIMMY_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( jimmy )
