# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindTBB.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Defines:
#
#  TBB_FOUND
#  TBB_INCLUDE_DIR
#  TBB_INCLUDE_DIRS
#  TBB_LIBRARIES
#  TBB_LIBRARY_DIRS
#
# Can be steered using TBB_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME TBB
   INCLUDE_SUFFIXES include INCLUDE_NAMES tbb/tbb.h
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS tbb
   EXTRA_OPTIONS NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( TBB DEFAULT_MSG
   TBB_INCLUDE_DIR TBB_LIBRARIES )
mark_as_advanced( TBB_FOUND TBB_INCLUDE_DIR TBB_INCLUDE_DIRS
   TBB_LIBRARIES TBB_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( tbb )
