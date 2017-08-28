# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: AnalysisTopReleaseEnvironmentConfig.cmake 784551 2016-11-16 12:46:32Z krasznaa $
#
# This file holds the compile and runtime environment configuration specific
# for the analysis release.
#

# This is a standalone project, so set the appropriate compile flags:
add_definitions( -DROOTCORE )
add_definitions( -DXAOD_STANDALONE )
add_definitions( -DXAOD_ANALYSIS )
add_definitions( -DROOTCORE_RELEASE_SERIES=25 )

# And the same variables for CMake as well:
set( ROOTCORE TRUE CACHE BOOL
   "Flag specifying that this is a \"RootCore\" release" )
set( XAOD_STANDALONE TRUE CACHE BOOL
   "Flag specifying that this is a standalone build" )
set( XAOD_ANALYSIS TRUE CACHE BOOL
   "Flag specifying that this is an analysis release" )

# Vatriable that should be picked up by the environment creation code:
set( AnalysisTopReleaseEnvironment_ENVIRONMENT
   SET ROOTCOREDIR \${AnalysisTop_DIR}
   SET ROOTCOREBIN \${AnalysisTop_DIR} )
