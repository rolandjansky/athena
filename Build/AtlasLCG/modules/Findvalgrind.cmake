# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findvalgrind.cmake 723149 2016-02-10 16:26:22Z krasznaa $
#
# Defines:
#
#  VALGRIND_FOUND
#  VALGRIND_INCLUDE_DIR
#  VALGRIND_INCLUDE_DIRS
#
# Can be steered by VALGRIND_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME valgrind
   INCLUDE_SUFFIXES include INCLUDE_NAMES valgrind/valgrind.h )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( valgrind DEFAULT_MSG VALGRIND_INCLUDE_DIR )
mark_as_advanced( VALGRIND_FOUND VALGRIND_INCLUDE_DIR VALGRIND_INCLUDE_DIRS )

# Set additional environment variables:
set( VALGRIND_ENVIRONMENT
   SET VALGRIND_LIB  ${VALGRIND_ROOT}/lib/valgrind
   SET VALGRIND_OPTS "--smc-check=all" )

# Set up the RPM dependency:
lcg_need_rpm( valgrind )
