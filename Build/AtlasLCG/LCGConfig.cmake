# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: LCGConfig.cmake 746545 2016-05-12 11:40:48Z krasznaa $
#
# File implementing the code that gets called when a project imports
# LCG using something like:
#
#  find_package( LCG 75 )
#
# Hopefully this could be adopted by the SFT group in the official LCG
# builds, but for now it's only a demonstrator.
#
# To make CMake find this file, you have to call CMake either like:
#
#   CMAKE_PREFIX_PATH=../source/AtlasCMake cmake ../source
#
# , or like:
#
#   cmake -DLCG_DIR=../../AtlasCMake ../source
#
# The script takes the value of the environment variable (or CMake cache
# variable) LCG_RELEASES_BASE to find LCG releases. Or if it's not set, it
# looks up the releases from AFS.
#

# This function is used by the code to get the "compiler portion"
# of the platform name. E.g. for GCC 4.9.2, return "gcc49". In case
# the compiler and version are not understood, the functions returns
# a false value in its second argument.
#
# The decision is made based on the C++ compiler.
#
# Usage: lcg_compiler_id( _cmp _isValid )
#
function( lcg_compiler_id compiler isValid )

   # Translate the compiler ID:
   set( _prefix )
   if( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
      set( _prefix "gcc" )
   elseif( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
      set( _prefix "clang" )
   elseif( CMAKE_CXX_COMPILER_ID STREQUAL "Intel" )
      set( _prefix "icc" )
   else()
      set( ${compiler} "unknown" PARENT_SCOPE )
      set( ${isValid} FALSE PARENT_SCOPE )
      return()
   endif()

   # Translate the compiler version:
   set( _version )
   if( CMAKE_CXX_COMPILER_VERSION MATCHES "^([0-9]+).([0-9]+).*" )
     set( _version "${CMAKE_MATCH_1}${CMAKE_MATCH_2}" )
   elseif( CMAKE_CXX_COMPILER_VERSION MATCHES "^([0-9]+).*" )
      set( _version "${CMAKE_MATCH_1}" )
   endif()

   # Set the return variables:
   set( ${compiler} "${_prefix}${_version}" PARENT_SCOPE )
   set( ${isValid} TRUE PARENT_SCOPE )

endfunction( lcg_compiler_id )

# This function is used to get a compact OS designation for the platform
# name. Like "slc6", "mac1010" or "cc7".
#
# Usage: lcg_os_id( _os _isValid )
#
function( lcg_os_id os isValid )

   # Reset the result variable as a start:
   set( _name )

   if( APPLE )
      # Get the MacOS X version number from the command line:
      execute_process( COMMAND sw_vers -productVersion
         TIMEOUT 30
         OUTPUT_VARIABLE _lcgMacVers )
      # Parse the variable, which should be in the form "X.Y.Z", or
      # possibly just "X.Y":
      if( _lcgMacVers MATCHES "^([0-9]+).([0-9]+).*" )
         set( _name "mac${CMAKE_MATCH_1}${CMAKE_MATCH_2}" )
      else()
         message( SEND_ERROR "MacOS version (${_lcgMacVers}) not parseable" )
         set( ${os} "unknown" PARENT_SCOPE )
         set( ${isValid} FALSE PARENT_SCOPE )
         return()
      endif()
   elseif( UNIX )
      # Get the linux release ID:
      execute_process( COMMAND lsb_release -i
         TIMEOUT 30
         OUTPUT_VARIABLE _lcgLinuxId )
      # Translate it to a shorthand according to our own naming:
      set( _linuxShort )
      if( _lcgLinuxId MATCHES "Scientific" )
         set( _linuxShort "slc" )
      elseif( _lcgLinuxId MATCHES "Ubuntu" )
         set( _linuxShort "ubuntu" )
      elseif( _lcgLinuxId MATCHES "CentOS" )
         set( _linuxShort "cc" )
      else()
         message( WARNING "Linux flavour (${_lcgLinuxId}) not recognised" )
         set( _linuxShort "linux" )
      endif()
      # Get the linux version number:
      execute_process( COMMAND lsb_release -r
         TIMEOUT 30
         OUTPUT_VARIABLE _lcgLinuxVers )
      # Try to parse it:
      if( _lcgLinuxVers MATCHES "^Release:[^0-9]*([0-9]+)\\..*" )
         set( _name "${_linuxShort}${CMAKE_MATCH_1}" )
      else()
         message( SEND_ERROR "Linux release (${_lcgLinuxVers}) not parseable" )
         set( ${os} "unknown" PARENT_SCOPE )
         set( ${isValid} FALSE PARENT_SCOPE )
         return()
      endif()
   else()
      set( ${os} "unknown" PARENT_SCOPE )
      set( ${isValid} FALSE PARENT_SCOPE )
      return()
   endif()

   # Set the return values:
   set( ${os} ${_name} PARENT_SCOPE )
   set( ${isValid} TRUE PARENT_SCOPE )

endfunction( lcg_os_id )

# This function is used internally to construct a platform name for a
# project. Something like: "x86_64-slc6-gcc48-opt".
#
# Usage: lcg_platform_id( _platform )
#
function( lcg_platform_id platform )

   # Get the OS's name:
   lcg_os_id( _os _valid )
   if( NOT _valid )
      set( ${platform} "generic" PARENT_SCOPE )
      return()
   endif()

   # Get the compiler name:
   lcg_compiler_id( _cmp _valid )
   if( NOT _valid )
      set( ${platform} "generic" PARENT_SCOPE )
      return()
   endif()

   # Construct the postfix of the platform name:
   if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
      set( _postfix "dbg" )
   else()
      set( _postfix "opt" )
   endif()

   # Set the platform return value:
   set( ${platform} "${CMAKE_SYSTEM_PROCESSOR}-${_os}-${_cmp}-${_postfix}"
      PARENT_SCOPE )

endfunction( lcg_platform_id )

# Function setting the home/root directories of all the LCG packages.
# The function takes the name of an LCG text file as argument. One that
# holds the description of a list of packages held by the release.
#
# Usage: lcg_setup_packages( ${fileName} ${LCG_RELEASE_DIR} )
#
function( lcg_setup_packages lcgFile lcgReleaseDir )

   # Tell the user what's happening:
   if( NOT LCG_FIND_QUIETLY )
      message( STATUS "Setting up LCG release using: ${lcgFile}" )
   endif()

   # Read in the contents of the file:
   file( STRINGS ${lcgFile} _fileContents LIMIT_INPUT 20000
      REGEX ".*;.*;.*;.*;.*" )

   # Loop over each line of the configuration file:
   foreach( _line ${_fileContents} )

      # The component's name:
      list( GET _line 0 name )
      string( TOUPPER ${name} nameUpper )
      string( REPLACE "+" "P" nameUpper ${nameUpper} )
      # The component's identifier:
      list( GET _line 1 id )
      # The component's version:
      list( GET _line 2 version )
      string( STRIP ${version} version )
      # The component's base directory:
      list( GET _line 3 dir1 )
      string( STRIP ${dir1} dir2 )
      string( REPLACE "./" "" dir3 ${dir2} )
      # The component's dependencies:
      list( GET _line 4 dep )
      string( REPLACE "," ";" dep ${dep} )

      # Set up the component. In an extremely simple way for now, which
      # just assumes that the Find<Component>.cmake files will find
      # these components based on the <Component>_ROOT or possibly
      # <Component>_DIR variable.
      set( ${nameUpper}_ROOT ${lcgReleaseDir}/${dir3}
         CACHE PATH "Directory for ${name}-${version}" )
      set( ${nameUpper}_DIR ${lcgReleaseDir}/${dir3}
         CACHE PATH "Directory for ${name}-${version}" )
      set( ${nameUpper}_VERSION ${version}
         CACHE STRING "Version of ${name}" )

      # Set up the component's dependencies:
      set( _dependencies )
      foreach( _dependency ${dep} )
         string( STRIP ${_dependency} _dep )
         if( "${_dep}" MATCHES "^(.*)-[^-]+" )
            if( "${CMAKE_MATCH_1}" STREQUAL "eigen" )
               list( APPEND _dependencies Eigen )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "tbb" )
               list( APPEND _dependencies TBB )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "fftw" )
               list( APPEND _dependencies FFTW )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "doxygen" )
               list( APPEND _dependencies Doxygen )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "soqt" )
               list( APPEND _dependencies SoQt )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "expat" )
               list( APPEND _dependencies EXPAT )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "coin3d" )
               list( APPEND _dependencies Coin3D )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "Python" )
               list( APPEND _dependencies PythonLibs )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "fastjet" )
               list( APPEND _dependencies FastJet )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "Qt" )
               list( APPEND _dependencies Qt4 )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "xrootd" )
               list( APPEND _dependencies Xrootd )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "oracle" )
               list( APPEND _dependencies Oracle )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "QMtest" )
               list( APPEND _dependencies QMTest )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "crmc" )
               list( APPEND _dependencies Crmc )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "lhapdf" )
               list( APPEND _dependencies Lhapdf )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "pyquen" )
               list( APPEND _dependencies Pyquen )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "starlight" )
               list( APPEND _dependencies Starlight )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "heputils" )
               list( APPEND _dependencies HEPUtils )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "pythia6" )
               list( APPEND _dependencies Pythia6 )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "pythia8" )
               list( APPEND _dependencies Pythia8 )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "evtgen" )
               list( APPEND _dependencies EvtGen )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "photos" )
               list( APPEND _dependencies Photos )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "photos++" )
               list( APPEND _dependencies Photospp )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "tauola" )
               list( APPEND _dependencies Tauola )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "tauola++" )
               list( APPEND _dependencies Tauolapp )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "yoda" )
               list( APPEND _dependencies YODA )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "thepeg" )
               list( APPEND _dependencies ThePEG )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "hijing" )
               list( APPEND _dependencies Hijing )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "hydjet" )
               list( APPEND _dependencies Hydjet )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "rivet" )
               list( APPEND _dependencies Rivet )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "baurmc" )
               list( APPEND _dependencies BaurMC )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "hepmcanalysis" )
               list( APPEND _dependencies HepMCAnalysis )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "jimmy" )
               list( APPEND _dependencies Jimmy )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "herwig" )
               list( APPEND _dependencies Herwig )
            elseif( "${CMAKE_MATCH_1}" STREQUAL "lhapdfsets" OR
                  "${CMAKE_MATCH_1}" STREQUAL "cython" OR
                  "${CMAKE_MATCH_1}" STREQUAL "cmaketools" OR
                  "${CMAKE_MATCH_1}" STREQUAL "swig" OR
                  "${CMAKE_MATCH_1}" STREQUAL "GCCXML" )
               # These modules are ignored...
            else()
               list( APPEND _dependencies ${CMAKE_MATCH_1} )
            endif()
         endif()
      endforeach()
      set( ${nameUpper}_DEPENDS ${_dependencies}
         CACHE STRING "Dependencies of ${name}-${version}" )
      unset( _dependencies )
   endforeach()

endfunction( lcg_setup_packages )

# Function setting up all the packages from the selected LCG release.
# The code is moved into a function, so that variables generated inside the
# function would not be visible to the file calling find_package(LCG).
#
# Usage: lcg_setup_release( ${LCG_RELEASE_DIR} )
#
function( lcg_setup_release lcgReleaseDir )

   # The components to set up:
   set( LCG_COMPONENTS externals generators )

   # Set up the packages provided by LCG using the files specified:
   foreach( _component ${LCG_COMPONENTS} )

      # Construct the file name to load:
      set( _file ${lcgReleaseDir}/LCG_${_component}_${LCG_PLATFORM}.txt )
      if( NOT EXISTS ${_file} )
         message( SEND_ERROR
            "LCG component \"${_component}\" not available for platform: "
            "${LCG_PLATFORM}" )
         set( LCG_FOUND FALSE CACHE BOOL
            "Flag showing whether LCG was found or not" FORCE ) 
         continue()
      endif()

      # Set up the packages described by this file:
      lcg_setup_packages( ${_file} ${lcgReleaseDir} )

   endforeach()

endfunction( lcg_setup_release )

# Get the platform ID:
if( NOT "$ENV{LCG_PLATFORM}" STREQUAL "" )
   set( LCG_PLATFORM $ENV{LCG_PLATFORM}
      CACHE STRING "Platform name for the LCG release being used" )
else()
   lcg_platform_id( LCG_PLATFORM )
   set( LCG_PLATFORM ${LCG_PLATFORM}
      CACHE STRING "Platform name for the LCG release being used" )
endif()

# Tell the user what's happening:
if( NOT LCG_FIND_QUIETLY )
   message( STATUS
      "Setting up LCG release \"${LCG_VERSION}\" for platform: "
      "${LCG_PLATFORM}" )
endif()

# Some sanity checks:
if( LCG_FIND_COMPONENTS )
   message( WARNING "Components \"${LCG_FIND_COMPONENTS}\" requested, but "
      "finding LCG components is not supported" )
endif()

# Construct the path to pick up the release from:
set( LCG_RELEASE_DIR ${LCG_RELEASE_BASE}/LCG_${LCG_VERSION}
   CACHE PATH "Directory holding the LCG release" )

# Start out with the assumption that LCG is now found:
set( LCG_FOUND TRUE CACHE BOOL
   "Flag showing whether LCG was found or not" )
mark_as_advanced( LCG_FOUND )

# Set up the release:
lcg_setup_release( ${LCG_RELEASE_DIR} )

# Extra setting(s) for some package(s):
file( GLOB BOOST_INCLUDEDIR "${BOOST_ROOT}/include/*" )
list( APPEND CMAKE_PREFIX_PATH ${PYTHON_ROOT} )
file( GLOB PYTHON_INCLUDE_DIR "${PYTHON_ROOT}/include/*" )
set( PYTHON_INCLUDE_DIR ${PYTHON_INCLUDE_DIR} CACHE PATH
   "Python include directory" FORCE )
file( GLOB PYTHON_LIBRARY "${PYTHON_ROOT}/lib/libpython*.so" )
set( PYTHON_LIBRARY ${PYTHON_LIBRARY}
   CACHE FILEPATH "Python library" FORCE )
list( APPEND CMAKE_PREFIX_PATH ${DOXYGEN_ROOT} )
set( GSL_ROOT_DIR ${GSL_ROOT} )
list( APPEND CMAKE_PREFIX_PATH ${QT_ROOT} )
list( APPEND CMAKE_PREFIX_PATH ${COIN3D_ROOT} )
list( APPEND CMAKE_PREFIX_PATH ${EXPAT_ROOT} )

# Get the current directory:
get_filename_component( _thisdir "${CMAKE_CURRENT_LIST_FILE}" PATH )

# Add the module directory to CMake's module path:
list( INSERT CMAKE_MODULE_PATH 0 ${_thisdir}/modules )
list( REMOVE_DUPLICATES CMAKE_MODULE_PATH )

# Include the LCGFunctions module:
include( LCGFunctions )

# Extra build options needed by LCG packages:
set( LCG_COMPVERS "49" CACHE STRING "LCG compiler version" )
set( CMAKE_CXX_STANDARD 14 CACHE STRING "C++ standard used for the build" )
set( CMAKE_CXX_EXTENSIONS FALSE CACHE BOOL "(Dis)allow using GNU extensions" )

# Install this package with the current project:
install( DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/modules
   DESTINATION cmake
   USE_SOURCE_PERMISSIONS
   PATTERN ".svn" EXCLUDE
   PATTERN "*~" EXCLUDE )
install( FILES ${CMAKE_CURRENT_LIST_DIR}/LCGConfig.cmake
   ${CMAKE_CURRENT_LIST_DIR}/LCGConfig-version.cmake
   DESTINATION cmake )

# If CTEST_USE_LAUNCHERS is turned on, create a dummy log file already
# during configuration, which NICOS can pick up, and conclude that the
# package's "build" was successful.
if( CTEST_USE_LAUNCHERS )
   file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/BuildLogs )
   file( WRITE ${CMAKE_BINARY_DIR}/BuildLogs/AtlasLCG.log
      "Dummy build log\n"
      "AtlasLCG: Package build succeeded" )
endif()

# If there's a version file available, declare the version of this package
# to the project. And remember that find_package(LCG) may be called a number
# of times while configuring the project.
if( EXISTS ${_thisdir}/version.cmake )
   get_property( _oldVal GLOBAL PROPERTY ATLAS_SPECIAL_PACKAGES )
   if( NOT _oldVal MATCHES "Build/AtlasLCG" )
      file( READ ${_thisdir}/version.cmake _version LIMIT 10000 )
      set_property( GLOBAL APPEND PROPERTY ATLAS_SPECIAL_PACKAGES
         "Build/AtlasLCG" "${_version}" )
   endif()
   unset( _oldVal )
elseif( EXISTS ${_thisdir}/cmt/version.cmt )
   get_property( _oldVal GLOBAL PROPERTY ATLAS_SPECIAL_PACKAGES )
   if( NOT _oldVal MATCHES "Build/AtlasLCG" )
      file( READ ${_thisdir}/cmt/version.cmt _version LIMIT 10000 )
      set_property( GLOBAL APPEND PROPERTY ATLAS_SPECIAL_PACKAGES
         "Build/AtlasLCG" "${_version}" )
   endif()
   unset( _oldVal )
endif()

# Clean up:
unset( _thisdir )
