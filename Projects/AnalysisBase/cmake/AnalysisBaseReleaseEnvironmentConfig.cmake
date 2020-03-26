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

# Since this project may be built on platforms that provide Python 3
# out of the box, let's explicitly stick to Python 2 (coming from the
# externals project) for now.
set( ATLAS_FORCE_PYTHON2 TRUE CACHE BOOL
   "Force the usage of Python 2 in the AnalysisBase project" )

# Do not use Boost's CMake code when building AnalysisBase.
set( Boost_NO_BOOST_CMAKE TRUE CACHE BOOL
   "Disable the usage of Boost's CMake code" )

# Declare the module "found".
set( AnalysisBaseReleaseEnvironment_FOUND TRUE )
