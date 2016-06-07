# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: AtlasInternals.cmake 751680 2016-06-02 08:25:55Z krasznaa $
#
# Functions used internally by functions declared in AtlasFunctions.
# Which are for general use inside the package descriptions.

# Include the required module(s):
include( CMakeParseArguments )

# This function can be used to determine whether we are currently in a named
# package. Which is needed in certain situations.
#
# Usage: atlas_is_package( value )
#
function( atlas_is_package var )

   # If the ATLAS_PACKAGE variable is set, then we're in a package.
   # Otherwise we're not.
   if( ATLAS_PACKAGE )
      set( ${var} TRUE PARENT_SCOPE )
   else()
      set( ${var} FALSE PARENT_SCOPE )
   endif()

endfunction( atlas_is_package )

# This function can get the name of the current package to a function that
# needs it.
#
# Usage: atlas_get_package_name( name )
#
function( atlas_get_package_name name )

   # Get the package name from the ${ATLAS_PACKAGE} variable, which should
   # be set by the atlas_subdir(...) function.
   if( ATLAS_PACKAGE )
      set( ${name} ${ATLAS_PACKAGE} PARENT_SCOPE )
   else()
      message( SEND_ERROR "Package name unknown. Please use "
         "atlas_subdir(...) to set it!" )
   endif()

endfunction( atlas_get_package_name )

# This function can return the full path of the current package in the
# source directory.
#
# Usage: atlas_get_package_dir( pkgDir )
#
function( atlas_get_package_dir dir )

   # Just remove the prefix of the source directory of the current
   # package's source directory, and that's it...
   string( REPLACE "${CMAKE_SOURCE_DIR}/" "" _packageDir
      ${CMAKE_CURRENT_SOURCE_DIR} )

   # Return the string:
   set( ${dir} ${_packageDir} PARENT_SCOPE )

endfunction( atlas_get_package_dir )

# This macro is used in the functions taking care of compiling code, to
# set some common compilation options for the packages.
#
# Usage: atlas_set_compiler_flags()
#
macro( atlas_set_compiler_flags )

   # Get the package's name:
   atlas_get_package_name( pkgName )

   # Set a default package version:
   set( _version "${pkgName}-00-00-00" )

   # If the package has a file called "version.cmake", use its contents for
   # the package version:
   if( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/version.cmake" )
      file( READ "${CMAKE_CURRENT_SOURCE_DIR}/version.cmake"
         _version LIMIT 10000 )
      string( STRIP ${_version} _version )
   elseif( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmt/version.cmt" )
      # For backwards compatibility:
      file( READ "${CMAKE_CURRENT_SOURCE_DIR}/cmt/version.cmt"
         _version LIMIT 10000 )
      string( STRIP ${_version} _version )
   endif()

   # Add this version declaration to the build commands:
   add_definitions( -DPACKAGE_VERSION=\"${_version}\" )
   add_definitions( -DPACKAGE_VERSION_UQ=${_version} )

   # Clean up:
   unset( _version )

endmacro( atlas_set_compiler_flags )

# This function is used internally to extract the headers defined in
# LinkDef.h files, if the user didn't pass the header file names explicitly
# in his/her CMakeLists.txt file.
#
# Note that the implementation here is relatively fragile. So in the
# future CINT dictionaries will need to be declared in the CMakeLists.txt
# files by explicitly listing the header files that they operate on. Just
# like it has to be done for the offline compilation as well.
#
# @param header The name of the header file to process
# @param includes Return value, with a list of include files
#
function( atlas_get_linkdef_includes header includes )

   # Get the package/subdirectory name:
   atlas_get_package_name( pkgname )

   # Read in the header into a string. These files should be small, so
   # maximise the read to ~50kB:
   file( READ ${header} _headerString 50000 )

   # Find the "really good" include statements in them:
   string( REGEX MATCHALL "\n\#include \"${pkgname}/[^\"]+\"" _includeLines1
      ${_headerString} )
   # Find the slightly less good ones. A la ApplyJetCalibration...:
   string( REGEX MATCHALL "\n\#include \"\\.\\./${pkgname}/[^\"]+\""
      _includeLines2 ${_headerString} )
   # Find the not so good ones as well...:
   string( REGEX MATCHALL "\n\#include <${pkgname}/[^>]+>" _includeLines3
      ${_headerString} )
   string( REGEX MATCHALL "^\#include <${pkgname}/[^>]+>" _includeLines4
      ${_headerString} )
   string( REGEX MATCHALL "\n\#include <[^>]+/[^>]+>" _includeLines5
      ${_headerString} )

   # The variable to collect all includes into:
   set( _includes )

   # Collect the header names from the ~good statements:
   foreach( line ${_includeLines1} ${_includeLines2} )
      if( line MATCHES "\#include \"(.*)\"" )
         list( APPEND _includes ${CMAKE_MATCH_1} )
      endif()
   endforeach()

   # Now collect them from the bad ones:
   foreach( line ${_includeLines3} ${_includeLines4} ${_includeLines5} )
      if( line MATCHES "\#include <(.*)>" )
         list( APPEND _includes ${CMAKE_MATCH_1} )
      endif()
   endforeach()

   # Remove leading "../" symbols from the include names. This appears
   # in a few packages unfortunately...
   set( _return1 )
   foreach( inc ${_includes} )
      if( inc MATCHES "^\\.\\./(.*)" )
         list( APPEND _return1 ${CMAKE_MATCH_1} )
      else()
         list( APPEND _return1 ${inc} )
      endif()
   endforeach()

   # Set the return value:
   set( ${includes} ${_return1} PARENT_SCOPE )

endfunction( atlas_get_linkdef_includes )

# This function is used by the code to get the "compiler portion"
# of the platform name. E.g. for GCC 4.9.2, return "gcc49". In case
# the compiler and version are not understood, the functions returns
# a false value in its second argument.
#
# The decision is made based on the C++ compiler. As RootCore projects
# really don't use anything else...
#
# Usage: atlas_compiler_id( _cmp _isValid )
#
function( atlas_compiler_id compiler isValid )

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

endfunction( atlas_compiler_id )

# This function is used to get a compact OS designation for the platform
# name. Like "slc6", "mac1010" or "ubuntu15".
#
# Usage: atlas_os_id( _os _isValid )
#
function( atlas_os_id os isValid )

   set( _name )
   if( APPLE )
      # Set a default version in case the following should not work:
      set( _name "mac1010" )
      # Get the MacOS X version number from the command line:
      execute_process( COMMAND sw_vers -productVersion
         TIMEOUT 30
         OUTPUT_VARIABLE _macVers )
      # Parse the variable, which should be in the form "X.Y.Z", or
      # possibly just "X.Y":
      if( _macVers MATCHES "^([0-9]+).([0-9]+).*" )
         set( _name "mac${CMAKE_MATCH_1}${CMAKE_MATCH_2}" )
      else()
         set( ${os} "unknown" PARENT_SCOPE )
         set( ${isValid} FALSE PARENT_SCOPE )
         return()
      endif()
   elseif( UNIX )
      # Set a default version in case the following should not work:
      set( _name "slc6" )
      # Get the linux release ID:
      execute_process( COMMAND lsb_release -i
         TIMEOUT 30
         OUTPUT_VARIABLE _linuxId )
      # Translate it to a shorthand according to our own naming:
      set( _linuxShort )
      if( _linuxId MATCHES "Scientific" )
         set( _linuxShort "slc" )
      elseif( _linuxId MATCHES "Ubuntu" )
         set( _linuxShort "ubuntu" )
      elseif( _linuxId MATCHES "CentOS" )
         set( _linuxShort "cc" )
      else()
         message( WARNING "Linux flavour not recognised" )
         set( _linuxShort "linux" )
      endif()
      # Get the linux version number:
      execute_process( COMMAND lsb_release -r
         TIMEOUT 30
         OUTPUT_VARIABLE _linuxVers )
      # Try to parse it:
      if( _linuxVers MATCHES "^Release:[^0-9]*([0-9]+)\\..*" )
         set( _name "${_linuxShort}${CMAKE_MATCH_1}" )
      else()
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

endfunction( atlas_os_id )

# This function is used internally to construct a platform name for a
# project. Something like: "x86_64-slc6-gcc48-opt".
#
# Usage: atlas_platform_id( _platform )
#
function( atlas_platform_id platform )

  # Get the OS's name:
  atlas_os_id( _os _valid )
  if( NOT _valid )
    set( ${platform} "generic" PARENT_SCOPE )
    return()
  endif()

  # Get the compiler name:
  atlas_compiler_id( _cmp _valid )
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

endfunction( atlas_platform_id )

# Function used internally to describe to IDEs how they should group source
# files of libraries and executables in their interface.
#
# Usage: atlas_group_source_files( ${_sources} )
#
function( atlas_group_source_files )

   # Collect all the passed file names:
   cmake_parse_arguments( ARG "" "" "" ${ARGN} )

   # Loop over all of them:
   foreach( f ${ARG_UNPARSED_ARGUMENTS} )
      # Ignore absolute path names. Like dictionary source files. Those should
      # just show up outside of any group.
      if( NOT IS_ABSOLUTE ${f} )
         # Get the file's path:
         get_filename_component( _path ${f} PATH )
         # Replace the forward slashes with double backward slashes:
         string( REPLACE "/" "\\\\" _group ${_path} )
         # Put the file into the right group:
         source_group( ${_group} FILES ${f} )
      endif()
   endforeach()

endfunction( atlas_group_source_files )

# Macro copied off of Gaudi's __visit__ macro
#
# This recursive macro is used in ordering the packages according
# to their dependencies. To be used internally by atlas_sort_packages,
# not to be used in user code.
#
macro( atlas_package_visit _p )

   # Check if we saw this package already:
   if( NOT __${_p}_visited__ )
      # If not, we are seeing it now:
      set( __${_p}_visited__ TRUE )
      # Get all possible dependencies of this package:
      get_property( _${_p}_dependencies GLOBAL PROPERTY
         ATLAS_PACKAGE_${_p}_DEPENDENCIES )
      foreach( dep ${_${_p}_dependencies} )
         # Call this macro recursively on the dependent packages:
         atlas_package_visit( ${dep} )
      endforeach()
      # Now that all of its dependencies were added, add this package
      # to the ordered list:
      list( APPEND result ${_p} )
   endif()

endmacro( atlas_package_visit )

# Function sorting packages like gaudi_sort_subdirectories does
#
# This sorting is necessary for building the release using sub-projects
# with CTest. As in that case we need to tell CTest to build the packages
# one by one, in the right dependency order. (Otherwise compilation problems
# will not show up associated to the right package in CDash.)
#
# Usage: atlas_sort_packages( ${packages} sorted_packages )
#
function( atlas_sort_packages input output )

   # The resulting list:
   set( result )
   # Get the input list. It is done in such an awkward way because otherwise
   # we couldn't use this simple two parameter interface for the function.
   set( packages ${${input}} )

   # Use the atlas_package_visit recursive macro to populate the
   # ordered list:
   foreach( p ${packages} )
      atlas_package_visit( ${p} )
   endforeach()
   
   # Return the ordered list to the caller:
   set( ${output} ${result} PARENT_SCOPE )

endfunction( atlas_sort_packages )

# Function returning the number of cores that can be used in a Makefile
# based build for the "-jX" parameter.
#
# Usage: atlas_cpu_cores( nCores )
#
function( atlas_cpu_cores nCores )

   # Check if we need to re-evaluate the value:
   if( NOT DEFINED _processorCount )

      # Unknown:
      set( _processorCount 1 )

      # Linux:
      set( cpuinfo_file "/proc/cpuinfo" )
      if( EXISTS "${cpuinfo_file}" )
         file( STRINGS "${cpuinfo_file}" procs REGEX "^processor.: [0-9]+$" )
         list( LENGTH procs _processorCount )
      endif()

      # Mac:
      if( APPLE )
         find_program( cmd_sysctl "sysctl" )
         if( cmd_sysctl )
            execute_process( COMMAND ${cmd_sysctl} -n hw.ncpu
               OUTPUT_VARIABLE _info )
            string( STRIP ${_info} _processorCount )
         endif()
         mark_as_advanced( cmd_sysctl )
      endif()

      # Windows:
      if( WIN32 )
         set( _processorCount "$ENV{NUMBER_OF_PROCESSORS}" )
      endif()
   endif()

   # Set the value for the caller:
   set( ${nCores} ${_processorCount} PARENT_SCOPE )

endfunction( atlas_cpu_cores )

# This macro is used in AtlasProjectConfig.cmake to set up the project's
# use of the atlas_ctest.sh wrapper macro. Which takes care of writing
# package specific log files to the build area if CTEST_USE_LAUNCHERS is
# enabled.
#
# Usage: atlas_ctest_setup()
#
macro( atlas_ctest_setup )

   # Only run this code once:
   get_property( _ctestConfigured GLOBAL PROPERTY ATLAS_CTEST_CONFIGURED SET )
   set_property( GLOBAL PROPERTY ATLAS_CTEST_CONFIGURED TRUE )
   if( NOT _ctestConfigured )

      # Only do anything fancy if we are in CTEST_USE_LAUNCHERS mode:
      if( CTEST_USE_LAUNCHERS )
         # Find the atlas_ctest.sh.in script skeleton:
         find_file( _atlasCTestSkeleton NAMES atlas_ctest.sh.in
            PATH_SUFFIXES scripts PATHS ${CMAKE_MODULE_PATH} )
         # Check whether we found it:
         if( ${_atlasCTestSkeleton} MATCHES "NOTFOUND" )
            message( WARNING
               "Couldn't find atlas_ctest.sh.in. Not saving build logs!" )
            # Still include CTest, even in this case.
            include( CTest )
         else()
            # Configure the script:
            configure_file( ${_atlasCTestSkeleton}
               ${CMAKE_BINARY_DIR}/atlas_ctest.sh @ONLY )
            set( _atlasCTest ${CMAKE_BINARY_DIR}/atlas_ctest.sh )
            set( CMAKE_CTEST_COMMAND ${_atlasCTest} )
            message( STATUS "Saving build logs using: ${_atlasCTest}" )
            # The name of the log directory:
            set( _logDir ${CMAKE_BINARY_DIR}/BuildLogs )
            # Create the directory that we want to write logs to:
            file( MAKE_DIRECTORY ${_logDir} )
            # Let CTest make the basic setup:
            include( CTest )
            # And now customise it for ourselves:
            foreach( _buildop COMPILE LINK CUSTOM )
               get_property( _origLauncher GLOBAL PROPERTY
                  RULE_LAUNCH_${_buildop} )
               string( REGEX REPLACE "--$" "" _launcher ${_origLauncher} )
               set( _newLauncher
                  "${_launcher} --log-dir ${_logDir} --binary-dir" )
               set( _newLauncher "${_newLauncher} ${CMAKE_BINARY_DIR} --" )
               set_property( GLOBAL PROPERTY RULE_LAUNCH_${_buildop}
                  ${_newLauncher} )
            endforeach()
            # Now clean up:
            unset( _atlasCTest )
            unset( _logDir )
            unset( _origLauncher )
            unset( _launcher )
            unset( _newLauncher )
         endif()
      else()
         # Otherwise just include CTest, and be done with it:
         include( CTest )
      endif()
   endif()

   # Some additional cleanup:
   unset( _ctestConfigured )

endmacro( atlas_ctest_setup )

# This macro should be used by projects to set them up for packaging
# using CPack. Projects depending on other ATLAS projects get this
# set up automatically, but base projects (like AtlasExternals) need
# to call it explicitly.
#
# Usage: atlas_cpack_setup()
#
macro( atlas_cpack_setup )

   # Taken from ROOT. Not clear whether we need it...
   #include( InstallRequiredSystemLibraries )

   # Set some variables, constructed from variables set originally for CMake
   # itself:
   if( CMAKE_PROJECT_NAME AND CMAKE_PROJECT_VERSION )
      set( CPACK_PACKAGE_DESCRIPTION
         "${CMAKE_PROJECT_NAME} - ${CMAKE_PROJECT_VERSION}" )
      set( CPACK_PACKAGE_DESCRIPTION_SUMMARY
         "${CMAKE_PROJECT_NAME} - ${CMAKE_PROJECT_VERSION}" )
   elseif( NOT CMAKE_PROJECT_NAME )
      set( CMAKE_PROJECT_NAME "AtlasDummyProject" )
   endif()
   set( CPACK_PACKAGE_VENDOR "ATLAS Collaboration" )
   if( CMAKE_PROJECT_VERSION )
      set( CPACK_PACKAGE_VERSION ${CMAKE_PROJECT_VERSION} )
   else()
      set( CPACK_PACKAGE_VERSION "0.0.1" )
   endif()
   if( CMAKE_PROJECT_VERSION_MAJOR )
      set( CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR} )
   endif()
   if( CMAKE_PROJECT_VERSION_MINOR )
      set( CPACK_PACKAGE_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR} )
   endif()
   if( CMAKE_PROJECT_VERSION_PATCH )
      set( CPACK_PACKAGE_VERSION_PATCH ${CMAKE_PROJECT_VERSION_PATCH} )
   endif()

   # Contact information for the release:
   set( CPACK_PACKAGE_CONTACT "atlas-sw-core@cern.ch" )

   # Create a readme file for the release:
   find_file( _readme NAMES README.txt.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   mark_as_advanced( _readme )
   configure_file( ${_readme} ${CMAKE_BINARY_DIR}/README.txt @ONLY )
   unset( _readme )
   install( FILES ${CMAKE_BINARY_DIR}/README.txt
      DESTINATION . )
   set( CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_BINARY_DIR}/README.txt )
   set( CPACK_RESOURCE_FILE_README ${CMAKE_BINARY_DIR}/README.txt )

   # Create a license file for the release:
   find_file( _license NAMES LICENSE.txt
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   mark_as_advanced( _license )
   set( CPACK_RESOURCE_FILE_LICENSE ${_license} )
   install( FILES ${_license} DESTINATION . )
   unset( _license )

   # The installed release is supposed to be fully relocatable:
   set( CPACK_PACKAGE_RELOCATABLE TRUE )
   # This is the current convention which the environment setup scripts assume.
   # When changing the convention, one must change it in sync in:
   #  - NICOS;
   #  - The environment setup scripts;
   #  - cpack_install.sh.in;
   #  - Here.
   set( CPACK_PACKAGE_INSTALL_DIRECTORY
      "${CMAKE_PROJECT_NAME}/${CPACK_PACKAGE_VERSION}/InstallArea/${ATLAS_PLATFORM}" )
   set( CPACK_INSTALL_PREFIX "usr/${CPACK_PACKAGE_INSTALL_DIRECTORY}" )
   # This is chosen to be in sync with how CPack chooses a name for the source
   # package archive:
   set( CPACK_PACKAGE_FILE_NAME
      "${CMAKE_PROJECT_NAME}_${CPACK_PACKAGE_VERSION}_${ATLAS_PLATFORM}" )
   # RPM specific settings:
   set( CPACK_RPM_PACKAGE_NAME
      "${CMAKE_PROJECT_NAME}_${CPACK_PACKAGE_VERSION}_${ATLAS_PLATFORM}" )
   set( CPACK_RPM_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}" )
   set( CPACK_RPM_PACKAGE_PROVIDES "/bin/sh" )
   set( CPACK_RPM_PACKAGE_ARCHITECTURE "noarch" )
   set( CPACK_RPM_PACKAGE_GROUP "ATLAS Software" )
   set( CPACK_RPM_PACKAGE_LICENSE "Simplified BSD" )

   # Set the ATLAS project package dependencies of this RPM:
   set( CPACK_RPM_PACKAGE_AUTOREQ " no" )
   set( CPACK_RPM_PACKAGE_AUTOREQPROV " no" )
   set( CPACK_RPM_PACKAGE_REQUIRES )
   set( _bases ${ATLAS_BASE_PROJECTS} )
   while( _bases )
      list( GET _bases 0 _base_project )
      list( GET _bases 1 _base_version )
      list( REMOVE_AT _bases 0 1 )
      if( NOT "${CPACK_RPM_PACKAGE_REQUIRES}" STREQUAL "" )
         set( CPACK_RPM_PACKAGE_REQUIRES
            "${CPACK_RPM_PACKAGE_REQUIRES}, " )
      endif()
      set( _req "${_base_project}_${_base_version}_${ATLAS_PLATFORM}" )
      set( CPACK_RPM_PACKAGE_REQUIRES
         "${CPACK_RPM_PACKAGE_REQUIRES}${_req}" )
   endwhile()
   unset( _bases )
   unset( _base_project )
   unset( _base_version )
   unset( _req )

   # Add the external RPM dependencies:
   get_property( _extRpms GLOBAL PROPERTY ATLAS_EXTERNAL_RPMS )
   if( _extRpms )
      list( REMOVE_DUPLICATES _extRpms )
      foreach( _rpm ${_extRpms} )
         if( NOT "${CPACK_RPM_PACKAGE_REQUIRES}" STREQUAL "" )
            set( CPACK_RPM_PACKAGE_REQUIRES
               "${CPACK_RPM_PACKAGE_REQUIRES}, " )
         endif()
         set( CPACK_RPM_PACKAGE_REQUIRES
            "${CPACK_RPM_PACKAGE_REQUIRES}${_rpm}" )
      endforeach()
   endif()
   unset( _extRpms )

   # Use a custom CPack install script:
   find_file( _installScript NAMES cpack_install.cmake
      PATHS ${CMAKE_MODULE_PATH} PATH_SUFFIXES scripts )
   mark_as_advanced( _installScript )
   set( CPACK_INSTALL_SCRIPT ${_installScript} )
   unset( _installScript )

   # Installing the release for CPack only works correctly if the user
   # specified "/InstallArea/${ATLAS_PLATFORM}" as CMAKE_INSTALL_PREFIX
   # on the command line. So print a warning if it is not the case.
   # We still don't hardcode this value into the configuration, as there can be
   # good reasons for using AtlasCMake without building a CPack package, with
   # any given value for CMAKE_INSTALL_PREFIX.
   if( NOT "${CMAKE_INSTALL_PREFIX}" STREQUAL "/InstallArea/${ATLAS_PLATFORM}" )
      message( WARNING "CPack packaging will only work correctly with "
         "CMAKE_INSTALL_PREFIX=/InstallArea/${ATLAS_PLATFORM}" )
   endif()

   # Use a custom script for installing the release for CPack. Which hides
   # build errors from CPack.
   set( CPACK_INSTALL_CMAKE_PROJECTS "" )
   find_file( _installShellScript NAMES cpack_install.sh.in
      PATH_SUFFIXES scripts PATHS ${CMAKE_MODULE_PATH} )
   mark_as_advanced( _installShellScript )
   configure_file( ${_installShellScript} ${CMAKE_BINARY_DIR}/cpack_install.sh
      @ONLY )
   set( CPACK_INSTALL_COMMANDS ${CMAKE_BINARY_DIR}/cpack_install.sh )
   unset( _installShellScript )

   # Select the generator(s):
   if( APPLE )
      set( CPACK_GENERATOR "TGZ" "PackageMaker" )
      set( CPACK_SOURCE_GENERATOR "TBZ2" )
   else()
      set( CPACK_GENERATOR "RPM" )
      set( CPACK_SOURCE_GENERATOR "RPM" )
      set( CPACK_PACKAGE_DEFAULT_LOCATION "/usr" )
   endif()

   # Set up the generator customisation file:
   find_file( _cpackOptions NAMES CPackOptions.cmake.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   mark_as_advanced( _cpackOptions )
   configure_file( ${_cpackOptions} ${CMAKE_BINARY_DIR}/CPackOptions.cmake
      @ONLY )
   set( CPACK_PROJECT_CONFIG_FILE ${CMAKE_BINARY_DIR}/CPackOptions.cmake )
   unset( _cpackOptions )

   # Include CPack itself:
   include( CPack )

endmacro( atlas_cpack_setup )

# This function is used to massage the parameter list specified for
# LINK_LIBRARIES in offline packages, to translate it to something that
# CMake can actually understand.
#
# Usage: atlas_get_libraries( ARG_LINK_LIBRARIES _libraries )
#
function( atlas_get_libraries input output )

   # The massaged results:
   set( _result )

   # Loop over the specified library names:
   foreach( _lib ${${input}} )

      # Exception(s):
      set( _libName ${_lib} )
      if( ${_lib} STREQUAL "PythonLibs" )
         set( _libName "PYTHON" )
      endif()

      # Check if there's a variable existing that would suggest that this
      # is in fact the name of an external package:
      string( TOUPPER ${_libName} _package )
      if( NOT "${${_libName}_LIBRARIES}" STREQUAL "" )
         list( APPEND _result ${${_libName}_LIBRARIES} )
      elseif( NOT "${${_package}_LIBRARIES}" STREQUAL "" )
         list( APPEND _result ${${_package}_LIBRARIES} )
      elseif( NOT "${${_libName}_FOUND}" AND NOT "${${_package}_FOUND}" )
         list( APPEND _result ${_libName} )
      else()
         # If we are here, then this is apparently an external, which doesn't
         # provide libraries.
      endif()
   endforeach()

   # Set the variable in the parent scope:
   set( ${output} ${_result} PARENT_SCOPE )

endfunction( atlas_get_libraries )

# This function is used to massage the parameter list specified for
# INCLUDE_DIRS in offline packages, to translate it to something that
# CMake can actually understand.
#
# Usage: atlas_get_includes( ARG_INCLUDE_DIRS _includes )
#
function( atlas_get_includes input output )

   # The massaged results:
   set( _result )

   # Loop over the specified include directories:
   foreach( _inc ${${input}} )

      # Exception(s):
      set( _incName ${_inc} )
      if( ${_inc} STREQUAL "PythonLibs" )
         set( _incName "PYTHON" )
      endif()

      # Check if there's a variable existing that would suggest that this
      # is in fact the name of an external package:
      string( TOUPPER ${_incName} _package )
      if( NOT "${${_incName}_INCLUDE_DIRS}" STREQUAL "" )
         list( APPEND _result ${${_incName}_INCLUDE_DIRS} )
      elseif( NOT "${${_package}_INCLUDE_DIRS}" STREQUAL "" )
         list( APPEND _result ${${_package}_INCLUDE_DIRS} )
      elseif( NOT "${${_incName}_FOUND}" AND NOT "${${_package}_FOUND}" )
         list( APPEND _result ${_incName} )
      else()
         # If we are here, then this is apparently an external, which doesn't
         # provide an include directory.
      endif()
   endforeach()

   # Set the variable in the parent scope:
   set( ${output} ${_result} PARENT_SCOPE )

endfunction( atlas_get_includes )

# Master macro for finding "simple externals".
#
# Since many of the modules in this package look extremely similar, they are now
# implemented using this macro. To make their maintenance and development a bit
# easier.
#
# The produced code will rely on the <NAME>_ROOT variable to find the requested
# external, and will produce the following possible variables:
#   - <NAME>_INCLUDE_DIRS
#   - <NAME>_LIBRARIES
#   - <NAME>_LIBRARY_DIRS
#   - <NAME>_<component>_FOUND
#   - <NAME>_<component>_LIBRARY
#   - <NAME>_FOUND_COMPONENTS
#
# Remember that this function by itself is not enough to implement a find
# module. At the very least one has to use find_package_handle_standard_args
# after this macro call to make the module work as expected.
#
# Note that there is a slight, but important difference between the
# DEFAULT_COMPONENTS and COMPULSORY_COMPONENTS options. The first one lists
# components that will be looked up if the user didn't ask for any components
# at all, the second one lists components that will be added to the list of
# components regardless whether the user asked for them or not.
#
# Usage: atlas_external_module( NAME name
#                               [INCLUDE_SUFFIXES include1 include2...]
#                               [INCLUDE_NAMES header1 header2...]
#                               [DEFAULT_COMPONENTS component1 component2...]
#                               [COMPULSORY_COMPONENTS component3 component4...]
#                               [LIBRARY_SUFFIXES libDir1 libDir2...]
#                               [LIBRARY_PREFIX prefix_]
#                               [LIBRARY_POSTFIX _postfix]
#                               [EXTRA_OPTIONS cmakeOpt1...]
#                               [SEARCH_PATHS path1 path2...] )
#
macro( atlas_external_module )

   # Parse the arguments:
   set( _singleArgParams "NAME;LIBRARY_PREFIX;LIBRARY_POSTFIX" )
   set( _multiArgParams "INCLUDE_SUFFIXES;INCLUDE_NAMES;DEFAULT_COMPONENTS" )
   set( _multiArgParams "${_multiArgParams};COMPULSORY_COMPONENTS" )
   set( _multiArgParams "${_multiArgParams};LIBRARY_SUFFIXES;EXTRA_OPTIONS" )
   set( _multiArgParams "${_multiArgParams};SEARCH_PATHS" )
   cmake_parse_arguments( ARG "" "${_singleArgParams}" "${_multiArgParams}"
      ${ARGN} )

   # Create an uppercase version of the name:
   string( TOUPPER ${ARG_NAME} nameUpper )

   # If the external was already found, set the module to silent mode.
   if( ${ARG_NAME}_FOUND OR ${nameUpper}_FOUND )
      set( ${ARG_NAME}_FIND_QUIETLY TRUE )
   endif()

   # Find the include directory of the external:
   if( ARG_INCLUDE_NAMES )
      find_path( ${nameUpper}_INCLUDE_DIR NAMES ${ARG_INCLUDE_NAMES}
         PATH_SUFFIXES ${ARG_INCLUDE_SUFFIXES}
         PATHS ${${nameUpper}_ROOT} ${ARG_SEARCH_PATHS}
         NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH
         ${ARG_EXTRA_OPTIONS} )
      set( ${nameUpper}_INCLUDE_DIRS ${${nameUpper}_INCLUDE_DIR} )
   endif()

   # Set up the components:
   if( ARG_DEFAULT_COMPONENTS AND NOT ${ARG_NAME}_FIND_COMPONENTS )
      set( ${ARG_NAME}_FIND_COMPONENTS ${ARG_DEFAULT_COMPONENTS} )
   endif()
   if( ARG_COMPULSORY_COMPONENTS )
      list( APPEND ${ARG_NAME}_FIND_COMPONENTS ${ARG_COMPULSORY_COMPONENTS} )
      list( REMOVE_DUPLICATES ${ARG_NAME}_FIND_COMPONENTS )
   endif()

   # Find the library/libraries:
   if( ${ARG_NAME}_FIND_COMPONENTS )
      # Clear out the library variables:
      set( ${nameUpper}_LIBRARIES )
      set( ${nameUpper}_LIBRARY_DIRS )
      # Find the requested component(s):
      foreach( component ${${ARG_NAME}_FIND_COMPONENTS} )
         # Search for the requested library:
         find_library( ${nameUpper}_${component}_LIBRARY
            NAMES ${ARG_LIBRARY_PREFIX}${component}${ARG_LIBRARY_POSTFIX}
            PATH_SUFFIXES ${ARG_LIBRARY_SUFFIXES}
            PATHS ${${nameUpper}_ROOT} ${ARG_SEARCH_PATHS}
            NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH
            ${ARG_EXTRA_OPTIONS} )
         # Deal with the search result:
         if( ${nameUpper}_${component}_LIBRARY AND
               EXISTS "${${nameUpper}_${component}_LIBRARY}" )
            set( ${nameUpper}_${component}_FOUND TRUE )
            list( APPEND ${nameUpper}_FOUND_COMPONENTS ${component} )
            list( APPEND ${nameUpper}_LIBRARIES
               ${${nameUpper}_${component}_LIBRARY} )
            get_filename_component( libdir ${${nameUpper}_${component}_LIBRARY}
               PATH )
            list( APPEND ${nameUpper}_LIBRARY_DIRS ${libdir} )
         else()
            set( ${nameUpper}_${component}_FOUND FALSE )
         endif()
         # Mark the component location as an advanced property:
         mark_as_advanced( ${nameUpper}_${component}_LIBRARY
            ${nameUpper}_${component}_FOUND )
      endforeach()
      # Remove the duplicate library directory entries:
      if( ${nameUpper}_LIBRARY_DIRS )
         list( REMOVE_DUPLICATES ${nameUpper}_LIBRARY_DIRS )
      endif()
   endif()

endmacro( atlas_external_module )

# Macro used privately by atlas_merge_project_files to cut down on the number
# of code lines in the file.
macro( _merge_files targetName propPrefix fileName dest )

   # Check if the property is set:
   get_property( _propSet GLOBAL PROPERTY ${propPrefix}_FILES SET )

   # If yes, set up the merge for the files defined in it:
   if( _propSet )

      # Get the names of the files to be merged:
      get_property( _files GLOBAL PROPERTY ${propPrefix}_FILES )
      # Get the targets creating these files:
      get_property( _targets GLOBAL PROPERTY ${propPrefix}_TARGETS )

      # Get the directory name of the output file:
      get_filename_component( _dir ${fileName} PATH )

      # Set up the target/command perfoming the merge:
      add_custom_target( ${targetName} ALL
         COMMAND ${CMAKE_COMMAND} -E make_directory ${_dir}
         COMMAND ${_mergeFilesCmd} ${fileName} ${_files}
         DEPENDS ${_files} )
      add_dependencies( ${targetName} ${_targets} )
      set_property( TARGET ${targetName} PROPERTY FOLDER ${CMAKE_PROJECT_NAME} )
      set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} APPEND PROPERTY
         ADDITIONAL_MAKE_CLEAN_FILES ${fileName} )

      # Now, this code will never be run if *any* of the partial files failed
      # to build. This is okay for a test/build area. But when installing the
      # project, we need to execute the merge no matter what.
      install( CODE "if( NOT EXISTS ${fileName} )
                        message( WARNING \"Creating partial ${fileName}\" )
                        execute_process( COMMAND ${_mergeFilesCmd} ${fileName}
                           ${_files} )
                     endif()" )

      # Set up the installation of the merged file:
      install( FILES ${fileName} DESTINATION ${dest} OPTIONAL )

      # Clean up:
      unset( _files )
      unset( _targets )
      unset( _dir )

   endif()
   unset( _propSet )

endmacro( _merge_files )

# This function takes care of merging all the file types of the projects that
# should be merged. Like the rootmap and clid files generated for the individual
# libraries.
#
# Usage: atlas_merge_project_files()
#
function( atlas_merge_project_files )

   # Find the merge script:
   find_program( _mergeFilesCmd mergeFiles.sh
      PATH_SUFFIXES scripts PATHS ${CMAKE_MODULE_PATH} )
   mark_as_advanced( _mergeFilesCmd )

   # Merge the rootmap files:
   _merge_files( ${CMAKE_PROJECT_NAME}RootMapMerge ATLAS_ROOTMAP
      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.rootmap
      ${CMAKE_INSTALL_LIBDIR} )

   # Merge the CLID files:
   _merge_files( ${CMAKE_PROJECT_NAME}ClidMerge ATLAS_CLID
      ${CMAKE_SHARE_OUTPUT_DIRECTORY}/clid.db
      ${CMAKE_INSTALL_SHAREDIR} )

   # Merge the components files:
   _merge_files( ${CMAKE_PROJECT_NAME}ComponentsMerge ATLAS_COMPONENTS
      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.components
      ${CMAKE_INSTALL_LIBDIR} )

   # Merge the confdb files:
   _merge_files( ${CMAKE_PROJECT_NAME}ConfdbMerge ATLAS_CONFDB
      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.confdb
      ${CMAKE_INSTALL_LIBDIR} )

endfunction( atlas_merge_project_files )

# Delete the private macro:
unset( _merge_files )
