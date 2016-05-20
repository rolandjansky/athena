# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findgperftools.cmake 732185 2016-03-24 13:58:59Z krasznaa $
#
# Defines:
#
#  GPERFTOOLS_FOUND
#  GPERFTOOLS_INCLUDE_DIR
#  GPERFTOOLS_LIBRARIES
#
# Can be steered using GPERFTOOLS_ROOT.
#

# Include the helper code:
include( AtlasInternals )

# Declare the module:
atlas_external_module( NAME gperftools
   INCLUDE_SUFFIXES include INCLUDE_NAMES gperftools/tcmalloc.h
   LIBRARY_SUFFIXES lib
   DEFAULT_COMPONENTS tcmalloc )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( gperftools DEFAULT_MSG
   GPERFTOOLS_INCLUDE_DIRS GPERFTOOLS_LIBRARIES )
mark_as_advanced( GPERFTOOLS_FOUND GPERFTOOLS_INCLUDE_DIR
   GPERFTOOLS_INCLUDE_DIRS GPERFTOOLS_LIBRARIES GPERFTOOLS_LIBRARY_DIRS )
