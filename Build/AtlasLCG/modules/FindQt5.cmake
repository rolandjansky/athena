# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindQt5.cmake 762134 2016-07-15 10:00:34Z krasznaa $
#
# This file is here to intercept find_package(Qt5) calls, and
# massage the paths produced by the system module, to make them relocatable.
#

# The LCG include(s):
include( LCGFunctions )

# Qt5 needs at least one component to be requested from it. So let's make
# Core compulsory.
list( APPEND Qt5_FIND_COMPONENTS Core )
list( REMOVE_DUPLICATES Qt5_FIND_COMPONENTS )

# Temporarily clean out CMAKE_MODULE_PATH, so that we could pick up
# FindQt5.cmake from CMake:
set( _modulePathBackup ${CMAKE_MODULE_PATH} )
set( CMAKE_MODULE_PATH )

# Make the code ignore the system path(s):
set( CMAKE_SYSTEM_IGNORE_PATH /usr/include /usr/bin /usr/lib /usr/lib32
   /usr/lib64 )

# Let Qt's own CMake code be found:
find_package( Qt5 )

# Restore CMAKE_MODULE_PATH:
set( CMAKE_MODULE_PATH ${_modulePathBackup} )
unset( _modulePathBackup )
set( CMAKE_SYSTEM_IGNORE_PATH )

# Massage the paths found by the system module:
if( Qt5_FOUND AND NOT GAUDI_ATLAS )

   # Make sure that the lib/ directory is added to the environment:
   set( QT5_LIBRARY_DIRS "${QT5_ROOT}/lib" ) 

   # At this point we'll have to figure out how to convince the Qt5 code
   # to run the build executables inside the build environment. So that
   # they could actually find the Qt5 libraries.
   #set( AM_QT_MOC_EXECUTABLE
   #   "${CMAKE_BINARY_DIR}/atlas_build_run.sh ${AM_QT_MOC_EXECUTABLE}" )
   #set( _qt_uic_executable
   #   "${CMAKE_BINARY_DIR}/atlas_build_run.sh ${_qt_uic_executable}" )
   #set( AM_QT_RCC_EXECUTABLE
   #   "${CMAKE_BINARY_DIR}/atlas_build_run.sh ${AM_QT_RCC_EXECUTABLE}" )

endif()
