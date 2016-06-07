# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindGaudi.cmake 753396 2016-06-07 15:36:05Z krasznaa $
#
# CMake script attempting to find a Gaudi installation that we can build
# the offline code against. Without making any use of the CMake code
# bundled with the Gaudi installation.
#
# The module reads hints for the location of Gaudi from the following
# variable(s):
#
#   GAUDI_ROOT      - The installation directory of the Gaudi version to use
#
# When invoked, the module sets up all environment variables to make it possible
# to use ATLAS packages that depend on the various Gaudi packages, through an
# atlas_depends_on_subdirs call.
#

# Only include this file once:
if( GAUDI_FOUND )
   return()
endif()

# Pull in Gaudi's location from the environment if necessary:
if( NOT GAUDI_ROOT )
   if( NOT "$ENV{GAUDI_ROOT}" STREQUAL "" )
      set( GAUDI_ROOT $ENV{GAUDI_ROOT} )
   elseif( NOT Gaudi_FIND_QUIETLY )
      message( WARNING "GAUDI_ROOT not set" )
      return()
   endif()
endif()

# Try to find Gaudi in the specified directory:
find_path( GAUDI_INSTALLAREA GaudiConfig.cmake PATHS ${GAUDI_ROOT} )
set( Gaudi_ROOT_DIR ${GAUDI_INSTALLAREA} )
find_path( GAUDI_INCLUDE_DIR GAUDI_VERSION.h
   PATH_SUFFIXES include PATHS ${GAUDI_ROOT} )
set( GAUDI_INCLUDE_DIRS ${GAUDI_INCLUDE_DIR} )
set( GAUDI_LIBRARY_DIR ${GAUDI_INSTALLAREA}/lib )
set( GAUDI_LIBRARY_DIRS ${GAUDI_LIBRARY_DIR} )
set( GAUDI_PYTHON_PATH ${GAUDI_INSTALLAREA}/python )

# Additional environment settings:
set( GAUDI_ENVIRONMENT
   SET GAUDI_ROOT ${GAUDI_ROOT}
   APPEND JOBOPTSEARCHPATH ${GAUDI_INSTALLAREA}/jobOptions
   APPEND DATAPATH ${GAUDI_INSTALLAREA}/share )

# Handle the usual parameters given to find_package(...) calls:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Gaudi DEFAULT_MSG GAUDI_INSTALLAREA
   GAUDI_INCLUDE_DIR )
mark_as_advanced( GAUDI_FOUND GAUDI_INSTALLAREA Gaudi_ROOT_DIR GAUDI_INCLUDE_DIR
   GAUDI_INCLUDE_DIRS )

# If Gaudi was not found, finish here:
if( NOT GAUDI_FOUND )
   return()
endif()

# The used Gaudi libraries use Boost, ROOT, TBB and Python:
find_package( Boost QUIET REQUIRED COMPONENTS filesystem thread system regex )
find_package( ROOT QUIET REQUIRED COMPONENTS Core )
find_package( TBB QUIET )
find_package( PythonLibs QUIET REQUIRED )

# Set up the package interface libraries that atlas_depends_on_subdirs
# will be able to use:
macro( _gaudi_setup_pkg_lib pkgName )
   add_library( ${pkgName}Pkg INTERFACE IMPORTED )
   set_target_properties( ${pkgName}Pkg PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${GAUDI_INSTALLAREA}/include" )
endmacro( _gaudi_setup_pkg_lib )
_gaudi_setup_pkg_lib( GaudiAlg )
_gaudi_setup_pkg_lib( GaudiAud )
_gaudi_setup_pkg_lib( GaudiKernel )
_gaudi_setup_pkg_lib( GaudiPython )
_gaudi_setup_pkg_lib( GaudiPluginService )
_gaudi_setup_pkg_lib( GaudiSvc )
_gaudi_setup_pkg_lib( PartPropSvc )
_gaudi_setup_pkg_lib( RootHistCnv )
unset( _gaudi_setup_pkg_lib )

# Set up the libraries that we link against in the ATLAS code:
add_library( GaudiPluginService SHARED IMPORTED )
set_property( TARGET GaudiPluginService PROPERTY IMPORTED_CONFIGURATIONS
   RELEASE )
set_target_properties( GaudiPluginService PROPERTIES
   IMPORTED_LOCATION_RELEASE "${GAUDI_INSTALLAREA}/lib/libGaudiPluginService.so"
   IMPORTED_SONAME_RELEASE "@rpath/libGaudiPluginService.so"
   INTERFACE_INCLUDE_DIRECTORIES "${GAUDI_INSTALLAREA}/include" )

add_library( GaudiKernel SHARED IMPORTED )
set_property( TARGET GaudiKernel PROPERTY IMPORTED_CONFIGURATIONS RELEASE )
set_target_properties( GaudiKernel PROPERTIES
   IMPORTED_LOCATION_RELEASE "${GAUDI_INSTALLAREA}/lib/libGaudiKernel.so"
   IMPORTED_SONAME_RELEASE "@rpath/libGaudiKernel.so"
   INTERFACE_LINK_LIBRARIES
   "${Boost_LIBRARIES};${ROOT_LIBRARIES};${TBB_LIBRARIES};GaudiPluginService"
   INTERFACE_INCLUDE_DIRECTORIES
   "${GAUDI_INSTALLAREA}/include;${Boost_INCLUDE_DIRS};${TBB_INCLUDE_DIRS};${ROOT_INCLUDE_DIRS}"
   COMPILE_FLAGS "${ROOT_CXX_FLAGS}" )

# Import (the) executable(s) from Gaudi that we need during the build:
add_executable( listcomponents IMPORTED )
set_property( TARGET listcomponents PROPERTY IMPORTED_CONFIGURATIONS
   RELEASE )
set_target_properties( listcomponents PROPERTIES
   IMPORTED_LOCATION         "${GAUDI_INSTALLAREA}/bin/listcomponents.exe"
   IMPORTED_LOCATION_RELEASE "${GAUDI_INSTALLAREA}/bin/listcomponents.exe" )

add_executable( xenv IMPORTED )
set_property( TARGET xenv PROPERTY IMPORTED_CONFIGURATIONS
   RELEASE )
set_target_properties( xenv PROPERTIES
   IMPORTED_LOCATION         "${GAUDI_INSTALLAREA}/scripts/xenv"
   IMPORTED_LOCATION_RELEASE "${GAUDI_INSTALLAREA}/scripts/xenv" )

add_executable( genconf IMPORTED )
set_property( TARGET genconf PROPERTY IMPORTED_CONFIGURATIONS
   RELEASE )
set_target_properties( genconf PROPERTIES
   IMPORTED_LOCATION         "${GAUDI_INSTALLAREA}/bin/genconf.exe"
   IMPORTED_LOCATION_RELEASE "${GAUDI_INSTALLAREA}/bin/genconf.exe" )

# Set up the RPM dependency for Gaudi:
if( GAUDI_VERSION )
   set( version ${GAUDI_VERSION} )
elseif( NOT "$ENV{GAUDI_VERSION}" STREQUAL "" )
   set( version $ENV{GAUDI_VERSION} )
elseif( PROJECT_VERSION )
   # This should kick in when building a release with NICOS:
   set( version ${PROJECT_VERSION} )
else()
   set( version "${CMAKE_PROJECT_VERSION}" )
endif()
set_property( GLOBAL APPEND PROPERTY ATLAS_EXTERNAL_RPMS
   "GAUDI_${version}_${ATLAS_PLATFORM}" )
message( "Added dependency: GAUDI_${version}_${ATLAS_PLATFORM}" )
unset( version )
