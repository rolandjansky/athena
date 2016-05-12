# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findgfal.cmake 724137 2016-02-16 09:01:57Z krasznaa $
#
# Defines:
#  - GFAL_FOUND
#  - GFAL_INCLUDE_DIRS
#  - GFAL_LIBRARY_DIRS
#  - GFAL_<component>_FOUND
#  - GFAL_<component>_LIBRARY
#  - GFAL_LIBRARIES
#
# Can be steered by GFAL_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME gfal
   INCLUDE_SUFFIXES include INCLUDE_NAMES gfal_api.h
   LIBRARY_SUFFIXES lib lib64
   DEFAULT_COMPONENTS gfal )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( gfal DEFAULT_MSG GFAL_INCLUDE_DIR
   GFAL_LIBRARIES )
mark_as_advanced( GFAL_FOUND GFAL_INCLUDE_DIR GFAL_INCLUDE_DIRS
   GFAL_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( gfal )
