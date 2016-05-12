# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findxapian.cmake 724137 2016-02-16 09:01:57Z krasznaa $
#
# Defines:
#  - XAPIAN_FOUND
#  - XAPIAN_INCLUDE_DIR
#  - XAPIAN_INCLUDE_DIRS
#  - XAPIAN_<component>_FOUND
#  - XAPIAN_<component>_LIBRARY
#  - XAPIAN_LIBRARIES
#  - XAPIAN_LIBRARY_DIR
#  - XAPIAN_LIBRARY_DIRS
#
# The script can be steered by XAPIAN_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME xapian
   INCLUDE_SUFFIXES include INCLUDE_NAMES xapian/xapian.h
   LIBRARY_SUFFIXES lib
   DEFAULT_COMPONENTS xapian )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( xapian DEFAULT_MSG XAPIAN_INCLUDE_DIR
   XAPIAN_LIBRARIES )
mark_as_advanced( XAPIAN_FOUND XAPIAN_INCLUDE_DIR XAPIAN_INCLUDE_DIRS
   XAPIAN_LIBRARIES XAPIAN_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( xapian )
