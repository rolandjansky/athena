# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findsrm_ifce.cmake 724137 2016-02-16 09:01:57Z krasznaa $
#
# Defines:
#  - SRM_IFCE_FOUND
#  - SRM_IFCE_INCLUDE_DIRS
#  - SRM_IFCE_LIBRARY_DIRS
#  - SRM_IFCE_<component>_FOUND
#  - SRM_IFCE_<component>_LIBRARY
#  - SRM_IFCE_LIBRARIES
#
# Can be steered by SRM_IFCE_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME srm_ifce
   INCLUDE_SUFFIXES include INCLUDE_NAMES gfal_srm_ifce.h
   LIBRARY_SUFFIXES lib lib64
   DEFAULT_COMPONENTS gfal_srm_ifce )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( srm_ifce DEFAULT_MSG SRM_IFCE_INCLUDE_DIR
   SRM_IFCE_LIBRARIES )
mark_as_advanced( SRM_IFCE_FOUND SRM_IFCE_INCLUDE_DIR SRM_IFCE_INCLUDE_DIRS
   SRM_IFCE_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( srm_ifce )
