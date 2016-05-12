# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindPhotospp.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Module finding Photos++ in the LCG release. Defines:
#  - PHOTOSPP_FOUND
#  - PHOTOSPP_INCLUDE_DIR
#  - PHOTOSPP_INCLUDE_DIRS
#  - PHOTOSPP_LIBRARIES
#  - PHOTOSPP_LIBRARY_DIRS
#
# Can be steered by PHOTOSPP_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Photospp
   INCLUDE_SUFFIXES include INCLUDE_NAMES Photos/Photos.h
   LIBRARY_SUFFIXES lib
   DEFAULT_COMPONENTS PhotosCxxInterface Photospp )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Photospp DEFAULT_MSG PHOTOSPP_INCLUDE_DIR
   PHOTOSPP_LIBRARIES )
mark_as_advanced( PHOTOSPP_FOUND PHOTOSPP_INCLUDE_DIR PHOTOSPP_INCLUDE_DIRS
   PHOTOSPP_LIBRARIES PHOTOSPP_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( photos++ FOUND_NAME PHOTOSPP )
