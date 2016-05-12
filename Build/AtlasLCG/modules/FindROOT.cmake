# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindROOT.cmake 744728 2016-05-04 07:37:43Z krasznaa $
#
# Find the ROOT libraries, headers and tools.
# Sets:
#  ROOTSYS
#  ROOT_INCLUDE_DIR
#  ROOT_INCLUDE_DIRS
#  ROOT_LIBRARIES
#  ROOT_LIBRARY_DIRS
#  ROOT_<component>_LIBRARY
#  ROOT_<component>_FOUND
#  ROOT_VERSION_STRING
#  ROOT_BINARY_PATH
#  ROOT_PYTHON_PATH
#
# As for all the other modules, the search of this module can be steered
# using ROOT_ROOT.

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME ROOT
   INCLUDE_SUFFIXES include INCLUDE_NAMES TROOT.h
   LIBRARY_SUFFIXES lib
   DEFAULT_COMPONENTS Core Cling RIO Hist Tree TreePlayer Matrix
   GenVector MathCore MathMore XMLIO Graf Gui Rint Physics
   COMPULSORY_COMPONENTS Core
   EXTRA_OPTIONS NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH )

# Get ROOT's version:
if( ROOT_INCLUDE_DIR )
   file( STRINGS ${ROOT_INCLUDE_DIR}/RVersion.h _RVersion
      REGEX "define *ROOT_RELEASE " )
   string( REGEX MATCH
      "\"(([0-9]+)\\.([0-9]+)/([0-9]+)([a-z]*|-rc[0-9]+))\""
      _RVersion ${_RVersion} )
   set( ROOT_VERSION_STRING ${CMAKE_MATCH_1} CACHE INTERNAL
      "Version of ROOT" FORCE )
   set( ROOT_VERSION_MAJOR ${CMAKE_MATCH_2} CACHE INTERNAL
      "Major version of ROOT" FORCE )
   set( ROOT_VERSION_MINOR ${CMAKE_MATCH_3} CACHE INTERNAL
      "Minor version of ROOT" FORCE )
   set( ROOT_VERSION_PATCH ${CMAKE_MATCH_4} CACHE INTERNAL
      "Patch version of ROOT" FORCE )
endif()

# Variable(s) used in the build code:
find_path( ROOT_BINARY_PATH rootcling
   PATH_SUFFIXES bin
   PATHS ${ROOT_ROOT}
   NO_SYSTEM_ENVIRONMENT_PATH
   NO_CMAKE_SYSTEM_PATH )

# Find the python path:
find_path( ROOT_PYTHON_PATH ROOT.py
   PATH_SUFFIXES lib PATHS ${ROOT_ROOT}
   NO_SYSTEM_ENVIRONMENT_PATH
   NO_CMAKE_SYSTEM_PATH )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( ROOT DEFAULT_MSG ROOT_INCLUDE_DIR
   ROOT_VERSION_STRING ROOT_LIBRARIES ROOT_BINARY_PATH ROOT_PYTHON_PATH )
mark_as_advanced( ROOT_FOUND ROOT_INCLUDE_DIR ROOT_INCLUDE_DIRS
   ROOT_LIBRARIES ROOT_LIBRARY_DIRS ROOT_BINARY_PATH ROOT_PYTHON_PATH )

# Settings for Gaudi:
if( GAUDI_ATLAS )
  set( ROOT_FIND_QUIETLY TRUE )
  set( ROOT_rootcling_CMD ${ROOT_BINARY_PATH}/rootcling )
  set( ROOT_genreflex_CMD ${ROOT_BINARY_PATH}/genreflex )
  set( _root_required_vars ROOT_Core_LIBRARY )
  include( EnableROOT6 )
endif()

# Set ROOTSYS:
if( ROOT_FOUND )
   get_filename_component( ROOTSYS ${ROOT_INCLUDE_DIR} PATH )
   if( GAUDI_ATLAS )
      set( ROOT_ENVIRONMENT SET ROOTSYS ${ROOTSYS} )
   else()
      set( ROOT_ENVIRONMENT FORCESET ROOTSYS ${ROOTSYS} )
   endif()
endif()

# Set up the RPM dependency:
lcg_need_rpm( ROOT )
 