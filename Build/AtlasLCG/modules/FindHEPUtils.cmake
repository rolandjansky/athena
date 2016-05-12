# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindHEPUtils.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Locate the HEPUtils package.
# Defines:
#  - HEPUTILS_FOUND
#  - HEPUTILS_INCLUDE_DIR
#  - HEPUTILS_INCLUDE_DIRS
#
# Can be steered by HEPUTILS_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME HEPUtils
   INCLUDE_SUFFIXES include INCLUDE_NAMES HEPUtils/Utils.h )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( HEPUtils DEFAULT_MSG HEPUTILS_INCLUDE_DIR )
mark_as_advanced( HEPUTILS_FOUND HEPUTILS_INCLUDE_DIR HEPUTILS_INCLUDE_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( heputils )
