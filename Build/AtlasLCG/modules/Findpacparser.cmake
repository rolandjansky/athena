# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findpacparser.cmake 724137 2016-02-16 09:01:57Z krasznaa $
#
# Defines:
#  - PACPARSER_FOUND
#  - PACPARSER_INCLUDE_DIR
#  - PACPARSER_INCLUDE_DIRS
#  - PACPARSER_<component>_FOUND
#  - PACPARSER_<component>_LIBRARY
#  - PACPARSER_LIBRARIES
#  - PACPARSER_LIBRARY_DIR
#  - PACPARSER_LIBRARY_DIRS
#
# The script can be steered by PACPARSER_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME pacparser
   INCLUDE_SUFFIXES include INCLUDE_NAMES pacparser.h
   LIBRARY_SUFFIXES lib
   DEFAULT_COMPONENTS pacparser )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( pacparser DEFAULT_MSG PACPARSER_INCLUDE_DIR
   PACPARSER_LIBRARIES )
mark_as_advanced( PACPARSER_FOUND PACPARSER_INCLUDE_DIR PACPARSER_INCLUDE_DIRS
   PACPARSER_LIBRARIES PACPARSER_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( pacparser )
