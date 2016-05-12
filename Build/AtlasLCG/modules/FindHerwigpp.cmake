# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindHerwigpp.cmake 742916 2016-04-26 11:28:14Z krasznaa $
#
# Module finding Herwig++ in the LCG release. Defines:
#  - HERWIGPP_FOUND
#  - HERWIGPP_INCLUDE_DIR
#  - HERWIGPP_INCLUDE_DIRS
#  - HERWIGPP_LIBRARIES
#  - HERWIGPP_LIBRARY_DIRS
#
# Can be steered by HERWIGPP_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Herwigpp
   INCLUDE_SUFFIXES include
   INCLUDE_NAMES Herwig++/Analysis/EventShapes.h
   LIBRARY_SUFFIXES lib/Herwig++
   COMPULSORY_COMPONENTS Herwig.so )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Herwigpp DEFAULT_MSG HERWIGPP_INCLUDE_DIR
   HERWIGPP_LIBRARIES )
mark_as_advanced( HERWIGPP_FOUND HERWIGPP_INCLUDE_DIR HERWIGPP_INCLUDE_DIRS
   HERWIGPP_LIBRARIES HERWIGPP_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( herwig++ FOUND_NAME HERWIGPP )
