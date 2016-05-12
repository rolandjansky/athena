# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findmysql.cmake 724137 2016-02-16 09:01:57Z krasznaa $
#
# Defines:
#  - MYSQL_FOUND
#  - MYSQL_INCLUDE_DIRS
#  - MYSQL_LIBRARY_DIRS
#  - MYSQL_<component>_FOUND
#  - MYSQL_<component>_LIBRARY
#  - MYSQL_LIBRARIES
#
# Can be steered by MYSQL_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME mysql
   INCLUDE_SUFFIXES include INCLUDE_NAMES mysql.h
   LIBRARY_SUFFIXES lib lib64
   DEFAULT_COMPONENTS mysqlclient )

# Find the binary directory:
find_path( MYSQL_BINARY_PATH mysql
   PATH_SUFFIXES bin PATHS ${MYSQL_ROOT}
   NO_SYSTEM_ENVIRONMENT_PATH
   NO_CMAKE_SYSTEM_PATH )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( mysql DEFAULT_MSG MYSQL_INCLUDE_DIR
   MYSQL_LIBRARIES MYSQL_BINARY_PATH )
mark_as_advanced( MYSQL_FOUND MYSQL_INCLUDE_DIR MYSQL_INCLUDE_DIRS
   MYSQL_LIBRARY_DIRS MYSQL_BINARY_PATH )

# Set up the RPM dependency:
lcg_need_rpm( mysql )
