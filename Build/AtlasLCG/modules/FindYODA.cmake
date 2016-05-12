# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindYODA.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# Module finding YODA in the LCG release. Defines:
#  - YODA_FOUND
#  - YODA_INCLUDE_DIR
#  - YODA_INCLUDE_DIRS
#  - YODA_LIBRARIES
#  - YODA_LIBRARY_DIRS
#
# Can be steered by YODA_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME YODA
   INCLUDE_SUFFIXES include INCLUDE_NAMES YODA/ReaderYODA.h
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS YODA )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( YODA DEFAULT_MSG YODA_INCLUDE_DIR
   YODA_LIBRARIES )
mark_as_advanced( YODA_FOUND YODA_INCLUDE_DIR YODA_INCLUDE_DIRS
   YODA_LIBRARIES YODA_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( yoda )
