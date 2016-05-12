# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindDavix.cmake 724137 2016-02-16 09:01:57Z krasznaa $
#
# Defines:
#  - DAVIX_FOUND
#  - DAVIX_INCLUDE_DIRS
#  - DAVIX_LIBRARY_DIRS
#  - DAVIX_<component>_FOUND
#  - DAVIX_<component>_LIBRARY
#  - DAVIX_LIBRARIES
#
# Can be steered by DAVIX_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Davix
   INCLUDE_SUFFIXES include INCLUDE_NAMES davix/davix.hpp
   LIBRARY_SUFFIXES lib lib64
   DEFAULT_COMPONENTS davix )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Davix DEFAULT_MSG DAVIX_INCLUDE_DIR
   DAVIX_LIBRARIES )
mark_as_advanced( DAVIX_FOUND DAVIX_INCLUDE_DIR DAVIX_INCLUDE_DIRS
   DAVIX_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( Davix )
