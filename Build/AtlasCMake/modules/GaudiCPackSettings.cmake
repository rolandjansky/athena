# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: GaudiCPackSettings.cmake 733606 2016-04-04 09:39:19Z krasznaa $
#
# This file is picked up by Gaudi, when it's built against AtlasExternals.
# It configures how Gaudi should use CPack.
#

# Tell the user what's happening:
message( STATUS "Configuring GAUDI to use the ATLAS CPack settings" )

# Remember the project name:
set( _projectName ${CMAKE_PROJECT_NAME} )
# Remember the project version:
set( _projectVersion ${CMAKE_PROJECT_VERSION} )

# Set the GAUDI version according to NICOS, if the environment variables
# are available:
if( NOT "$ENV{NICOS_ATLAS_ALT_RELEASE}" STREQUAL "" AND
      NOT "$ENV{NICOS_ATLAS_ALT_RELEASE}" STREQUAL "None" )
   set( CMAKE_PROJECT_VERSION $ENV{NICOS_ATLAS_ALT_RELEASE} )
elseif( NOT "$ENV{NICOS_ATLAS_RELEASE}" STREQUAL "" )
   set( CMAKE_PROJECT_VERSION $ENV{NICOS_ATLAS_RELEASE} )
endif()

# Call the package GAUDI, and not Gaudi:
set( CMAKE_PROJECT_NAME "GAUDI" )

# Make the GAUDI package depend on the AtlasExternals one:
set( ATLAS_BASE_PROJECTS AtlasExternals ${AtlasExternals_VERSION} )

# Do the regular ATLAS CPack configuration:
include( AtlasInternals )
atlas_cpack_setup()

# Clean up:
set( CMAKE_PROJECT_VERSION ${_projectVersion} )
unset( _projectVersion )
set( CMAKE_PROJECT_NAME ${_projectName} )
unset( _projectName )
unset( ATLAS_BASE_PROJECTS )
