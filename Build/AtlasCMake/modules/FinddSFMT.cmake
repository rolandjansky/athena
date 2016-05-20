# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FinddSFMT.cmake 725783 2016-02-23 17:15:28Z krasznaa $
#
# - Locate and install the dSFMT External
# Defines:
#
#  DSFMT_FOUND
#  DSFMT_INCLUDE_DIR
#  DSFMT_INCLUDE_DIRS (not cached)
#  DSFMT_LIBRARIES
#
# Can be steered using DSFMT_ROOT.
#

# Include the helper code:
include( AtlasInternals )

# Declare the module:
atlas_external_module( NAME dSFMT
   INCLUDE_SUFFIXES include INCLUDE_NAMES dSFMT.h
   LIBRARY_SUFFIXES ${ATLAS_PLATFORM} lib
   LIBRARY_PREFIX "dSFMT-"
   COMPULSORY_COMPONENTS std sse2 )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( dSFMT DEFAULT_MSG DSFMT_INCLUDE_DIR
   DSFMT_LIBRARIES )
mark_as_advanced( DSFMT_FOUND DSFMT_INCLUDE_DIR DSFMT_INCLUDE_DIRS
   DSFMT_LIBRARIES DSFMT_LIBRARY_DIRS )

# Code using this external needs the following definition:
add_definitions( -DDSFMT_MEXP=19937 )
