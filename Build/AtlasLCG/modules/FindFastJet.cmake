# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindFastJet.cmake 741609 2016-04-20 11:53:29Z krasznaa $
#
# - Locate FastJet library and header files
# Defines:
#
#  FASTJET_FOUND
#  FASTJET_INCLUDE_DIR
#  FASTJET_INCLUDE_DIRS
#  FASTJET_<component>_FOUND
#  FASTJET_<component>_LIBRARY
#  FASTJET_LIBRARIES
#  FASTJET_LIBRARY_DIRS
#
# Can be steered by FASTJET_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME FastJet
   INCLUDE_SUFFIXES include INCLUDE_NAMES fastjet/version.hh
   LIBRARY_SUFFIXES lib
   DEFAULT_COMPONENTS fastjet fastjetplugins fastjettools siscone
   siscone_spherical
   COMPULSORY_COMPONENTS fastjet )
if( FASTJET_LIBRARIES )
   find_library( _gfortran NAMES gfortran HINTS $ENV{LD_LIBRARY_PATH} )
   if( _gfortran )
      list( APPEND FASTJET_LIBRARIES
         $<BUILD_INTERFACE:${_gfortran}>
         $<INSTALL_INTERFACE:gfortran> )
   endif()
   unset( _gfortran )
endif()

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( FastJet DEFAULT_MSG FASTJET_INCLUDE_DIR
   FASTJET_LIBRARIES )
mark_as_advanced( FASTJET_FOUND FASTJET_INCLUDE_DIR FASTJET_INCLUDE_DIRS
   FASTJET_LIBRARIES FASTJET_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( fastjet )
