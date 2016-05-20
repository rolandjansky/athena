# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findgdb.cmake 711151 2015-11-27 12:00:55Z krasznaa $
#
# Defines:
#
#  GDB_FOUND
#  GDB_INCLUDE_DIR
#  GDB_INCLUDE_DIRS
#  GDB_<component>_FOUND
#  GDB_<component>_LIBRARY
#  GDB_LIBRARIES
#  GDB_LIBRARY_DIRS
#
# Can be steered by GDB_ROOT.
#

# Include the helper code:
include( AtlasInternals )

# Declare the module:
atlas_external_module( NAME gdb
   INCLUDE_SUFFIXES include INCLUDE_NAMES bfd.h
   LIBRARY_SUFFIXES lib lib64
   DEFAULT_COMPONENTS bfd iberty
   EXTRA_OPTIONS NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( gdb DEFAULT_MSG GDB_INCLUDE_DIRS
   GDB_LIBRARIES )
mark_as_advanced( GDB_FOUND GDB_INCLUDE_DIR GDB_INCLUDE_DIRS
   GDB_LIBRARIES GDB_LIBRARY_DIRS )
