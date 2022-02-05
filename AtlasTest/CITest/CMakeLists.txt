# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# This package contains the CI test definitions that are run for each MR.
# The actual test definition can be found in separate <PROJECT>.cmake files.
#

# Declare the package name:
atlas_subdir( CITest )

# CI tests are disabled by default:
option( ATLAS_ENABLE_CI_TESTS "Set up tests in CITest package" OFF )
if( NOT ATLAS_ENABLE_CI_TESTS )
   return()
endif()

# Include CI test utilities:
include( cmake/CITestFunctions.cmake )

# Include CI test definitions:
foreach( project Athena AnalysisBase AthGeneration AthSimulation )
   # Detect project name for full and WorkDir builds:
   # - ATLAS_PROJECT: set to parent project in WorkDir
   # - CMAKE_PROJECT_NAME: project name in full builds
   if( ATLAS_PROJECT STREQUAL ${project} OR CMAKE_PROJECT_NAME STREQUAL ${project} )
      message( STATUS "Enabling CI tests for project ${project}" )
      include( ${project}.cmake )
   endif()
endforeach()
