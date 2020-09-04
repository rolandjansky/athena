# $Id: CMakeLists.txt 720646 2016-01-29 14:01:10Z krasznaa $
#
# Build configuration for package AtlasAuthentication.
#

# Declare the package's name:
atlas_subdir( AtlasAuthentication )

# If we are in release rebuilding mode, stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Install the local file(s) of the package:
atlas_install_xmls( data/*.xml )
atlas_install_runtime( data/dbreplica.config )
atlas_install_scripts( share/setupLocalDBReplica_CERN.sh )

# Find the ATLAS externals area:
if( NOT SITEROOT )
   if( ENV{SITEROOT} )
      set( SITEROOT $ENV{SITEROOT} )
   else()
      set( SITEROOT "/afs/cern.ch" )
   endif()
endif()
if( NOT ATLAS_EXTERNAL )
   if( ENV{ATLAS_EXTERNAL} )
      set( ATLAS_EXTERNAL $ENV{ATLAS_EXTERNAL} )
   else()
      set( ATLAS_EXTERNAL "${SITEROOT}/atlas/offline/external" )
   endif()
endif()

# Pick up authentication.xml:
set( AtlasAuthentication_native_version "v18" )
set( AtlasAuthentication_home
   "${ATLAS_EXTERNAL}/AtlasAuth/${AtlasAuthentication_native_version}" )
if (EXISTS ${AtlasAuthentication_home}/authentication.xml )
   atlas_install_xmls( ${AtlasAuthentication_home}/authentication.xml )
else()
   message(WARNING "The required authentication file ${AtlasAuthentication_home}/authentication.xml does not exist!")
endif()
# Configure the environment setup module:
configure_file(
   ${CMAKE_CURRENT_SOURCE_DIR}/AtlasAuthenticationEnvironmentConfig.cmake.in
   ${CMAKE_CURRENT_BINARY_DIR}/AtlasAuthenticationEnvironmentConfig.cmake
   @ONLY )

# And now "find" it:
set( AtlasAuthenticationEnvironment_DIR ${CMAKE_CURRENT_BINARY_DIR}
   CACHE PATH "Location of AtlasAuthenticationEnvironmentConfig.cmake" )
find_package( AtlasAuthenticationEnvironment )
