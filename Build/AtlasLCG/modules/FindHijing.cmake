# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindHijing.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Module finding Hijing in the LCG release. Defines:
#  - HIJING_FOUND
#  - HIJING_LIBRARIES
#  - HIJING_LIBRARY_DIRS
#
# Can be steered by HIJING_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Hijing
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS hijing )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Hijing DEFAULT_MSG HIJING_LIBRARY_DIRS
   HIJING_LIBRARIES )
mark_as_advanced( HIJING_FOUND HIJING_LIBRARIES HIJING_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( hijing )
