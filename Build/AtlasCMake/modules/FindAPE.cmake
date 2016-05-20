# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindAPE.cmake 711151 2015-11-27 12:00:55Z krasznaa $
#
# - Locate and install the APE External
# Defines:
#
#  APE_FOUND
#  APE_INCLUDE_DIR
#  APE_INCLUDE_DIRS (not cached)
#  APE_LIBRARIES
#
# Can be steered using APE_ROOT.
#

# Include the helper code:
include( AtlasInternals )

# Declare the module:
atlas_external_module( NAME APE
   INCLUDE_SUFFIXES include INCLUDE_NAMES APE/BufferAccessor.hpp
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS APEContainer )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( APE DEFAULT_MSG APE_INCLUDE_DIRS
   APE_LIBRARIES )
mark_as_advanced( APE_FOUND APE_INCLUDE_DIR APE_INCLUDE_DIRS
   APE_LIBRARIES APE_LIBRARY_DIRS )
