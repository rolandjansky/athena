# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Finddpm.cmake 724137 2016-02-16 09:01:57Z krasznaa $
#
# - Locate dpm library
# Defines:
#  - DPM_FOUND
#  - DPM_INCLUDE_DIR
#  - DPM_INCLUDE_DIRS
#  - DPM_<component>_FOUND
#  - DPM_<component>_LIBRARY
#  - DPM_LIBRARIES
#  - DPM_LIBRARY_DIR
#  - DPM_LIBRARY_DIRS
#
# The script can be steered by DPM_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME dpm
   INCLUDE_SUFFIXES include INCLUDE_NAMES dpm/dpm_api.h
   LIBRARY_SUFFIXES lib64
   COMPULSORY_COMPONENTS dpm )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( dpm DEFAULT_MSG DPM_INCLUDE_DIR
   DPM_LIBRARIES )
mark_as_advanced( DPM_FOUND DPM_INCLUDE_DIR DPM_INCLUDE_DIRS
   DPM_LIBRARIES DPM_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( dpm )
