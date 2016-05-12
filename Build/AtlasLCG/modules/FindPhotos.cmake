# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindPhotos.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Module finding Photos in the LCG release. Defines:
#  - PHOTOS_FOUND
#  - PHOTOS_INCLUDE_DIR
#  - PHOTOS_INCLUDE_DIRS
#  - PHOTOS_LIBRARIES
#  - PHOTOS_LIBRARY_DIRS
#
# Can be steered by PHOTOS_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Photos
   INCLUDE_SUFFIXES include INCLUDE_NAMES ph_hepevt.inc
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS photos )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Photos DEFAULT_MSG PHOTOS_INCLUDE_DIR
   PHOTOS_LIBRARIES )
mark_as_advanced( PHOTOS_FOUND PHOTOS_INCLUDE_DIR PHOTOS_INCLUDE_DIRS
   PHOTOS_LIBRARIES PHOTOS_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( photos )
