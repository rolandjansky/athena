# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# This file holds the compile and runtime environment configuration specific
# for the analysis release.
#

# This is a standalone project, so set the appropriate compile flags:
add_definitions( -DROOTCORE )
add_definitions( -DXAOD_STANDALONE )
add_definitions( -DXAOD_ANALYSIS )

# And the same variables for CMake as well:
set( ROOTCORE TRUE CACHE BOOL
   "Flag specifying that this is a \"RootCore\" release" )
set( XAOD_STANDALONE TRUE CACHE BOOL
   "Flag specifying that this is a standalone build" )
set( XAOD_ANALYSIS TRUE CACHE BOOL
   "Flag specifying that this is an analysis release" )

# Vatriable that should be picked up by the environment creation code:
set( AnalysisBaseReleaseEnvironment_ENVIRONMENT
   SET ROOTCOREDIR \${AnalysisBase_DIR}
   SET ROOTCOREBIN \${AnalysisBase_DIR} )

# Find the first "acceptable" Python version according to our search path, and
# don't search for the absolute newest. (Which could result in picking up
# Python from /usr on modern OS-es.)
set( Python_FIND_STRATEGY "LOCATION" CACHE STRING
   "Search mode for FindPython.cmake" )
mark_as_advanced( Python_FIND_STRATEGY )

# Do not use Boost's CMake code when building AnalysisBase.
set( Boost_NO_BOOST_CMAKE TRUE CACHE BOOL
   "Disable the usage of Boost's CMake code" )

# Declare the module "found".
set( AnalysisBaseReleaseEnvironment_FOUND TRUE )
