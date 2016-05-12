# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindXrootd.cmake 730931 2016-03-18 08:30:37Z krasznaa $
#
# - Locate Xrootd library
# Defines:
#
#  XROOTD_FOUND
#  XROOTD_INCLUDE_DIR
#  XROOTD_INCLUDE_DIRS
#  XROOTD_<component>_LIBRARY
#  XROOTD_<component>_FOUND
#  XROOTD_LIBRARIES
#  XROOTD_LIBRARY_DIRS
#
# Can be steered by XROOTD_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Xrootd
   INCLUDE_SUFFIXES include INCLUDE_NAMES xrootd/XrdVersion.hh
   LIBRARY_SUFFIXES lib64 LIBRARY_PREFIX Xrd
   DEFAULT_COMPONENTS Utils
   EXTRA_OPTIONS NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Xrootd DEFAULT_MSG XROOTD_INCLUDE_DIR
   XROOTD_LIBRARIES )
mark_as_advanced( XROOTD_FOUND XROOTD_INCLUDE_DIR XROOTD_INCLUDE_DIRS
   XROOTD_LIBRARIES XROOTD_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( xrootd )
