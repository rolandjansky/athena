# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindFrontier_Client.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Defines:
#  - FRONTIER_CLIENT_FOUND
#  - FRONTIER_CLIENT_INCLUDE_DIR
#  - FRONTIER_CLIENT_INCLUDE_DIRS
#  - FRONTIER_CLIENT_<component>_LIBRARY
#  - FRONTIER_CLIENT_<component>_FOUND
#  - FRONTIER_CLIENT_LIBRARIES
#  - FRONTIER_CLIENT_LIBRARY_DIRS
#
# Can be steered by FRONTIER_CLIENT_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Frontier_Client
   INCLUDE_SUFFIXES include INCLUDE_NAMES frontier_client/frontier.h
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS frontier_client )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Frontier_Client DEFAULT_MSG
   FRONTIER_CLIENT_INCLUDE_DIR FRONTIER_CLIENT_LIBRARIES )
mark_as_advanced( FRONTIER_CLIENT_FOUND FRONTIER_CLIENT_INCLUDE_DIR
   FRONTIER_CLIENT_INCLUDE_DIRS FRONTIER_CLIENT_LIBRARIES
   FRONTIER_CLIENT_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( Frontier_Client )
