# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Finddcap.cmake 724137 2016-02-16 09:01:57Z krasznaa $
#
# Defines:
#  - DCAP_FOUND
#  - DCAP_INCLUDE_DIRS
#  - DCAP_LIBRARY_DIRS
#  - DCAP_<component>_FOUND
#  - DCAP_<component>_LIBRARY
#  - DCAP_LIBRARIES
#
# Can be steered by DCAP_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME dcap
   INCLUDE_SUFFIXES include INCLUDE_NAMES dcap.h
   LIBRARY_SUFFIXES lib lib64
   DEFAULT_COMPONENTS dcap )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( dcap DEFAULT_MSG DCAP_INCLUDE_DIR
   DCAP_LIBRARIES )
mark_as_advanced( DCAP_FOUND DCAP_INCLUDE_DIR DCAP_INCLUDE_DIRS
   DCAP_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( dcap )
