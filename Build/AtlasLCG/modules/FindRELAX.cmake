# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindRELAX.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# - Locate RELAX libraries directory.
# Defines:
#
#  RELAX_FOUND
#  RELAX_<component>_LIBRARY
#  RELAX_<component>_ROOTMAP
#  RELAX_ROOTMAPS list of rootmap files required
#  RELAX_LIBRARY_DIRS
#  RELAX_FOUND_COMPONENTS
#
# The script can be guided using RELAX_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME RELAX
   LIBRARY_SUFFIXES lib LIBRARY_POSTFIX Rflx
   DEFAULT_COMPONENTS STL )

# The suffix used for the rootmap files:
set( RELAX_ROOTMAP_SUFFIX .pamtoor )

# Find all the rootmap files:
set( RELAX_ROOTMAPS )
foreach( component ${RELAX_FOUND_COMPONENTS} )
   # deduce the name of the rootmap file for the library
   string( REPLACE "${CMAKE_SHARED_MODULE_SUFFIX}"
      "${RELAX_ROOTMAP_SUFFIX}" RELAX_${component}_ROOTMAP
      "${RELAX_${component}_LIBRARY}" )
   list( APPEND RELAX_ROOTMAPS ${RELAX_${component}_ROOTMAP} )
endforeach()

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( RELAX DEFAULT_MSG RELAX_LIBRARY_DIRS
   RELAX_ROOTMAPS )
mark_as_advanced( RELAX_FOUND RELAX_LIBRARY_DIRS RELAX_ROOTMAPS
   RELAX_FOUND_COMPONENTS )

# Set up the RPM dependency:
lcg_need_rpm( RELAX )
