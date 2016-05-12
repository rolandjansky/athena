# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findblackhat.cmake 724137 2016-02-16 09:01:57Z krasznaa $
#
# Defines:
#  - BLACKHAT_FOUND
#  - BLACKHAT_INCLUDE_DIR
#  - BLACKHAT_INCLUDE_DIRS
#  - BLACKHAT_<component>_FOUND
#  - BLACKHAT_<component>_LIBRARY
#  - BLACKHAT_LIBRARIES
#  - BLACKHAT_LIBRARY_DIR
#  - BLACKHAT_LIBRARY_DIRS
#
# The script can be steered by BLACKHAT_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME blackhat
   INCLUDE_SUFFIXES include INCLUDE_NAMES blackhat/BH_interface.h
   LIBRARY_SUFFIXES lib/blackhat
   DEFAULT_COMPONENTS BH )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( blackhat DEFAULT_MSG BLACKHAT_INCLUDE_DIR
   BLACKHAT_LIBRARIES )
mark_as_advanced( BLACKHAT_FOUND BLACKHAT_INCLUDE_DIR BLACKHAT_INCLUDE_DIRS
   BLACKHAT_LIBRARIES BLACKHAT_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( blackhat )
