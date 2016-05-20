# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: AtlasCMakeConfig-version.cmake 711151 2015-11-27 12:00:55Z krasznaa $

# The version is just a dummy. Note that we can't actually use svn's
# Revision keyword here... (As it's not in a format that CMake would
# understand as a version.)
set( PACKAGE_VERSION "1.0.0" )

# Check whether the requested PACKAGE_FIND_VERSION is compatible with this
# installed version:
if( "${PACKAGE_VERSION}" VERSION_LESS "${PACKAGE_FIND_VERSION}" )
   set( PACKAGE_VERSION_COMPATIBLE FALSE )
else()
   set( PACKAGE_VERSION_COMPATIBLE TRUE )
   if( "${PACKAGE_VERSION}" VERSION_EQUAL "${PACKAGE_FIND_VERSION}" )
      set( PACKAGE_VERSION_EXACT TRUE )
   endif()
endif()
