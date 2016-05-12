# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Finddcache_client.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# - Locate dcache_client library
# Defines:
#
#  DCACHE_CLIENT_FOUND
#  DCACHE_CLIENT_INCLUDE_DIR
#  DCACHE_CLIENT_INCLUDE_DIRS
#  DCACHE_CLIENT_LIBRARIES
#  DCACHE_CLIENT_LIBRARY_DIR
#  DCACHE_CLIENT_LIBRARY_DIRS
#
# Can be steered by DCAP_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME dcache_client
   INCLUDE_SUFFIXES include INCLUDE_NAMES dcap.h
   LIBRARY_SUFFIXES lib64
   COMPULSORY_COMPONENTS dcap
   SEARCH_PATHS ${DCAP_ROOT} )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( dcache_client DEFAULT_MSG
   DCACHE_CLIENT_INCLUDE_DIR DCACHE_CLIENT_LIBRARIES )
mark_as_advanced( DCACHE_CLIENT_FOUND DCACHE_CLIENT_INCLUDE_DIR
   DCACHE_CLIENT_INCLUDE_DIRS DCACHE_CLIENT_LIBRARIES
   DCACHE_CLIENT_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( dcap FOUND_NAME DCACHE_CLIENT VERSION_NAME DCAP )
