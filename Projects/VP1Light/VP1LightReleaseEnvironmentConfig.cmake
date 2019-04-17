#
# This file holds the compile and runtime environment configuration specific
# for the VP1Light release, based on the AnalysisBase release.
#

# This is a standalone project, so set the appropriate compile flags:
add_definitions( -DROOTCORE )
add_definitions( -DXAOD_STANDALONE )
add_definitions( -DXAOD_ANALYSIS )
add_definitions( -DROOTCORE_RELEASE_SERIES=25 )
add_definitions( -DBUILDVP1LIGHT )

if(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)	
	add_definitions(-DQT_NO_DEBUG_OUTPUT) # comment if you want to get debug messages in Release
endif(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)

# And the same variables for CMake as well:
set( ROOTCORE TRUE CACHE BOOL
   "Flag specifying that this is a \"RootCore\" release" )
set( XAOD_STANDALONE TRUE CACHE BOOL
   "Flag specifying that this is a standalone build" )
set( BUILDVP1LIGHT TRUE CACHE BOOL
   "Flag specifying that this is a VP1Light release (based on an AnalysisBase release)" )
set( XAOD_ANALYSIS TRUE CACHE BOOL
   "Flag specifying that this is an analysis release" )


# Get OS version
atlas_os_id( _os _isValid )
if( NOT _isValid )
   message( WARNING "Could not determine OS name. VP1Light cannot be built!" )
   return()
endif()

string(REGEX MATCH "mac|ubuntu|slc|centos" DIST ${_os})
if( NOT ${_os} STREQUAL "" )
   set( BUILDVP1LIGHT_DIST ${DIST} CACHE STRING 
      "Label storing the detected platform" FORCE ) 
   message ( STATUS "  >>> Building VP1Light on ${_os}" ) 
else()
    message(STATUS "   >>>> Building VP1Light: WARNING! Detected an unsupported platform! <<<<")
endif()


# Variable that should be picked up by the environment creation code:
set( VP1LightReleaseEnvironment_ENVIRONMENT
   SET ROOTCOREDIR \${VP1Light_DIR}
   SET ROOTCOREBIN \${VP1Light_DIR} )
