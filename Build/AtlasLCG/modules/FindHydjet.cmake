# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindHydjet.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Module finding Hydjet in the LCG release. Defines:
#  - HYDJET_FOUND
#  - HYDJET_INCLUDE_DIR
#  - HYDJET_INCLUDE_DIRS
#  - HYDJET_LIBRARIES
#  - HYDJET_LIBRARY_DIRS
#
# Can be steered by HYDJET_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Hydjet
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS hydjet )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Hydjet DEFAULT_MSG HYDJET_LIBRARY_DIRS
   HYDJET_LIBRARIES )
mark_as_advanced( HYDJET_FOUND HYDJET_LIBRARIES HYDJET_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( hydjet )
