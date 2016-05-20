# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findyampl.cmake 725956 2016-02-24 16:35:37Z krasznaa $
#
# Locate the yampl external package.
#
# Defines:
#  YAMPL_FOUND
#  YAMPL_INCLUDE_DIR
#  YAMPL_INCLUDE_DIRS
#  YAMPL_<component>_FOUND
#  YAMPL_<component>_LIBRARY
#  YAMPL_LIBRARIES
#  YAMPL_LIBRARY_DIRS
#
# The user can set YAMPL_ROOT to guide the script.
#

# Include the helper code:
include( AtlasInternals )

# Declare the module:
atlas_external_module( NAME yampl
   INCLUDE_SUFFIXES include INCLUDE_NAMES yampl/Channel.h
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS yampl )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( yampl DEFAULT_MSG YAMPL_INCLUDE_DIRS
   YAMPL_LIBRARIES )
mark_as_advanced( YAMPL_FOUND YAMPL_INCLUDE_DIR YAMPL_INCLUDE_DIRS
   YAMPL_LIBRARIES YAMPL_LIBRARY_DIRS )
