# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindHerwig3.cmake 742916 2016-04-26 11:28:14Z krasznaa $
#
# Module finding Herwig++3 in the LCG release. Defines:
#  - HERWIG3_FOUND
#  - HERWIG3_INCLUDE_DIR
#  - HERWIG3_INCLUDE_DIRS
#  - HERWIG3_LIBRARIES
#  - HERWIG3_LIBRARY_DIRS
#
# Can be steered by HERWIG3_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Herwig3
   INCLUDE_SUFFIXES include
   INCLUDE_NAMES Herwig/Analysis/EventShapes.h
   LIBRARY_SUFFIXES lib/Herwig
   COMPULSORY_COMPONENTS Herwig.so )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Herwig3 DEFAULT_MSG HERWIG3_INCLUDE_DIR
   HERWIG3_LIBRARIES )
mark_as_advanced( HERWIG3_FOUND HERWIG3_INCLUDE_DIR HERWIG3_INCLUDE_DIRS
   HERWIG3_LIBRARIES HERWIG3_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( herwig3 )
