# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindHerwig.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Module finding Herwig in the LCG release. Defines:
#  - HERWIG_FOUND
#  - HERWIG_INCLUDE_DIR
#  - HERWIG_INCLUDE_DIRS
#  - HERWIG_LIBRARIES
#  - HERWIG_LIBRARY_DIRS
#
# Can be steered by HERWIG_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Herwig
   INCLUDE_SUFFIXES include INCLUDE_NAMES HERWIG65.INC
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS herwig )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Herwig DEFAULT_MSG HERWIG_INCLUDE_DIR
   HERWIG_LIBRARIES )
mark_as_advanced( HERWIG_FOUND HERWIG_INCLUDE_DIR HERWIG_INCLUDE_DIRS
   HERWIG_LIBRARIES HERWIG_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( herwig )
