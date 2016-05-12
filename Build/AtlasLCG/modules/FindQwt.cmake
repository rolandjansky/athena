# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindQwt.cmake 731912 2016-03-23 15:42:06Z krasznaa $
#
# Module finding Qwt in the LCG release. Sets:
#  - QWT_INCLUDE_DIR
#  - QWT_INCLUDE_DIRS
#  - QWT_LIBRARY_DIR
#  - QWT_LIBRARY_DIRS
#  - QWT_LIBRARIES
#
# Can be steered by QWT_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Qwt
   INCLUDE_SUFFIXES include INCLUDE_NAMES qwt.h
   LIBRARY_SUFFIXES lib
   DEFAULT_COMPONENTS qwt )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Qwt DEFAULT_MSG QWT_INCLUDE_DIR
   QWT_LIBRARIES )
mark_as_advanced( QWT_FOUND QWT_INCLUDE_DIR QWT_INCLUDE_DIRS
   QWT_LIBRARIES QWT_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( qwt )
