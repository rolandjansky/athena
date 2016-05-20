# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Finddqm-common.cmake 732986 2016-03-30 16:25:54Z krasznaa $
#
# Try to find DQM-COMMON
# Defines:
#  - DQM-COMMON_FOUND
#  - DQM-COMMON_INCLUDE_DIR
#  - DQM-COMMON_INCLUDE_DIRS
#  - DQM-COMMON_<component>_FOUND
#  - DQM-COMMON_<component>_LIBRARY
#  - DQM-COMMON_LIBRARIES
#  - DQM-COMMON_LIBRARY_DIRS
#  - DQM-COMMON_PYTHON_PATH
#  - DQM-COMMON_BINARY_PATH
#
# Can be steered by DQM-COMMON_ROOT.
#

# Include the helper code:
include( AtlasInternals )

# Declare the module:
atlas_external_module( NAME dqm-common
   INCLUDE_SUFFIXES installed/include INCLUDE_NAMES dqm_core/Algorithm.h
   LIBRARY_SUFFIXES installed/${ATLAS_PLATFORM}/lib
   COMPULSORY_COMPONENTS dqm_core_io dqm_core dqm_dummy_io dqm_dummy )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( dqm-common DEFAULT_MSG DQM-COMMON_INCLUDE_DIR
   DQM-COMMON_LIBRARIES )
mark_as_advanced( DQM-COMMON_FOUND DQM-COMMON_INCLUDE_DIR
   DQM-COMMON_INCLUDE_DIRS DQM-COMMON_LIBRARIES DQM-COMMON_LIBRARY_DIRS )

# Set DQM specific environment:
if( DQM-COMMON_FOUND )
   set( DQM-COMMON_PYTHON_PATH ${DQM-COMMON_ROOT}/installed/share/lib/python
      ${DQM-COMMON_LIBRARY_DIRS} )
   set( DQM-COMMON_BINARY_PATH
      ${DQM-COMMON_ROOT}/installed/${ATLAS_PLATFORM}/bin
      ${DQM-COMMON_ROOT}/installed/share/bin )
endif()

# Add the RPM dependencies:
if( DQM-COMMON_FOUND )
   # Set up a dependency on the main dqm-common RPM package:
   set_property( GLOBAL APPEND PROPERTY ATLAS_EXTERNAL_RPMS
      "dqm-common-${DQM-COMMON_VERSION}_DQMCRelease_${ATLAS_PLATFORM}" )

#   foreach( comp ${dqm-common_FIND_COMPONENTS} )
#      if( DQM-COMMON_${comp}_FOUND )
#         set_property( GLOBAL APPEND PROPERTY ATLAS_EXTERNAL_RPMS
#            "dqm-common-${DQM-COMMON_VERSION}_${comp}_${ATLAS_PLATFORM}" )
#      endif()
#   endforeach()
endif()
