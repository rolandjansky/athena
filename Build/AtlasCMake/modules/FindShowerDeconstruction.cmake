# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindShowerDeconstruction.cmake 711151 2015-11-27 12:00:55Z krasznaa $
#
# - Locate the SHOWERDECONSTRUCTION External
# Defines:
#
#  SHOWERDECONSTRUCTION_FOUND
#  SHOWERDECONSTRUCTION_INCLUDE_DIR
#  SHOWERDECONSTRUCTION_INCLUDE_DIRS
#  SHOWERDECONSTRUCTION_<component>_FOUND
#  SHOWERDECONSTRUCTION_<component>_LIBRARY
#  SHOWERDECONSTRUCTION_LIBRARIES
#  SHOWERDECONSTRUCTION_LIBRARY_DIRS
#
# Can be steered by SHOWERDECONSTRUCTION_ROOT.
#

# Include the helper code:
include( AtlasInternals )

# Declare the module:
atlas_external_module( NAME ShowerDeconstruction
   INCLUDE_SUFFIXES include INCLUDE_NAMES Deconstruct.h
   LIBRARY_SUFFIXES lib COMPULSORY_COMPONENTS Deconstruction )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( ShowerDeconstruction DEFAULT_MSG
   SHOWERDECONSTRUCTION_INCLUDE_DIR SHOWERDECONSTRUCTION_LIBRARIES )
mark_as_advanced( SHOWERDECONSTRUCTION_FOUND SHOWERDECONSTRUCTION_INCLUDE_DIR
   SHOWERDECONSTRUCTION_INCLUDE_DIRS SHOWERDECONSTRUCTION_LIBRARIES
   SHOWERDECONSTRUCTION_LIBRARY_DIRS )
