# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findsqlite.cmake 724137 2016-02-16 09:01:57Z krasznaa $
#
# Defines:
#  - SQLITE_FOUND
#  - SQLITE_INCLUDE_DIR
#  - SQLITE_INCLUDE_DIRS
#  - SQLITE_<component>_FOUND
#  - SQLITE_<component>_LIBRARY
#  - SQLITE_LIBRARIES
#  - SQLITE_LIBRARY_DIR
#  - SQLITE_LIBRARY_DIRS
#
# The script can be steered by SQLITE_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME sqlite
   INCLUDE_SUFFIXES include INCLUDE_NAMES sqlite3.h
   LIBRARY_SUFFIXES lib
   DEFAULT_COMPONENTS sqlite3
   EXTRA_OPTIONS NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( sqlite DEFAULT_MSG SQLITE_INCLUDE_DIR
   SQLITE_LIBRARIES )
mark_as_advanced( SQLITE_FOUND SQLITE_INCLUDE_DIR SQLITE_INCLUDE_DIRS
   SQLITE_LIBRARIES SQLITE_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( sqlite )
