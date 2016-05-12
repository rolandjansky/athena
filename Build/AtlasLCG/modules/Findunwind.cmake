# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findunwind.cmake 718760 2016-01-20 14:00:18Z krasznaa $
#
# Defines:
#
#  UNWIND_FOUND
#  UNWIND_INCLUDE_DIR
#  UNWIND_INCLUDE_DIRS
#  UNWIND_<component>_FOUND
#  UNWIND_<component>_LIBRARY
#  UNWIND_LIBRARIES
#  UNWIND_LIBRARY_DIRS
#
# Can be steered using LIBUNWIND_ROOT. This module is a duplicate of
# Findlibunwind.cmake. Only put here because Gaudi expects to find this
# external with this name. But since the LCG release consistently refers
# to this external as "libunwind", I don't want to change the ATLAS
# naming convention...
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME unwind
   INCLUDE_SUFFIXES include INCLUDE_NAMES unwind.h
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS unwind
   SEARCH_PATHS ${LIBUNWIND_ROOT} )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( unwind DEFAULT_MSG UNWIND_INCLUDE_DIR
   UNWIND_LIBRARIES )
mark_as_advanced( UNWIND_FOUND UNWIND_INCLUDE_DIR UNWIND_INCLUDE_DIRS
   UNWIND_LIBRARIES UNWIND_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( libunwind FOUND_NAME UNWIND VERSION_NAME LIBUNWIND )
