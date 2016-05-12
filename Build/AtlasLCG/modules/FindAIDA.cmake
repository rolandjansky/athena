# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindAIDA.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# - Try to find AIDA
# Defines:
#
#  AIDA_FOUND
#  AIDA_INCLUDE_DIR
#  AIDA_INCLUDE_DIRS
#
# Can be steered by AIDA_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME AIDA
   INCLUDE_SUFFIXES src/cpp INCLUDE_NAMES AIDA/AIDA.h )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( AIDA DEFAULT_MSG AIDA_INCLUDE_DIR )
mark_as_advanced( AIDA_FOUND AIDA_INCLUDE_DIR AIDA_INCLUDE_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( AIDA )
