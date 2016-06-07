# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: AtlasFunctions.cmake 753396 2016-06-07 15:36:05Z krasznaa $
#
# This is the main file that needs to be included in order to get access to the
# ATLAS CMake functions.
#
# When building a release or a test area according to the normal instructions,
# it's not necessary to include it explicity. But in certain situations it may
# be necessary to include it with
#
#  include( AtlasFunctions )
#
# in the user code. Note that user code should only ever include this one file,
# and even this should only be included if absolutely necessary.
#

# Include the required module(s):
include( CMakeParseArguments )
include( ExternalProject )
include( AtlasInternals )
include( AtlasLibraryFunctions )
include( AtlasDictionaryFunctions )
include( AtlasInstallFunctions )
include( AtlasCompilerSettings )

# Standalone implementation for the function setting up an entire project
# for compilation. It needs to be used in the top level CMakeLists.txt file
# of the project.
#
# A standalone project (one that doesn't build against another one) can
# use it like:
#
#   find_package( AtlasProject )
#   atlas_project( BaseProject 1.0.0 )
#
# While a work area project (building against a base release) could use
# it like:
#
#   find_package( BaseProject 1.0.0 )
#   atlas_project( WorkProject 0.9.0
#                  USE BaseProject 1.0.0
#                  [RELEASE_RECOMPILE] )
#
# When specifying RELEASE_RECOMPILE, packages that need to be
# recompiled from the base release (because of packages compiled in the
# work area), are found and recompiled. Without it only packages in the
# work area are compiled.
#
# Usage: atlas_project( AnalysisBase 2.3.23
#                       [USE BaseRelease 1.2.3]
#                       [FORTRAN]
#                       [RELEASE_RECOMPILE] )
#
function( atlas_project name version )

   # Parse all options:
   cmake_parse_arguments( ARG "RELEASE_RECOMPILE;FORTRAN" "" "USE" ${ARGN} )

   # Tell the user what's happening:
   message( STATUS "Configuring ATLAS project with name "
      "\"${name}\" and version \"${version}\"" )

   # Check that the user specified meaningful base projects using the
   # USE parameter.
   if( ARG_USE )
      list( LENGTH ARG_USE _nUse )
      if( _nUse LESS 2 )
         message( SEND_ERROR "Wrong number of arguments in USE parameters" )
         return()
      endif()
   endif()

   # Reset some global properties used by the code:
   set_property( GLOBAL PROPERTY ATLAS_EXPORTS OFF )
   set_property( GLOBAL PROPERTY ATLAS_EXPORTED_TARGETS "" )
   set_property( GLOBAL PROPERTY ATLAS_EXPORTED_PACKAGES "" )

   # Do not set any rpath values on any of the files:
   set( CMAKE_SKIP_RPATH ON )
   set( CMAKE_SKIP_RPATH ON PARENT_SCOPE )
   set( CMAKE_SKIP_BUILD_RPATH ON )
   set( CMAKE_SKIP_BUILD_RPATH ON PARENT_SCOPE )
   set( CMAKE_SKIP_INSTALL_RPATH ON )
   set( CMAKE_SKIP_INSTALL_RPATH ON PARENT_SCOPE )

   # Do not consider the includes associated with imported targets as system
   # includes. System includes on imported targets are explicitly marked as
   # such.
   set( CMAKE_NO_SYSTEM_FROM_IMPORTED ON )
   set( CMAKE_NO_SYSTEM_FROM_IMPORTED ON PARENT_SCOPE )

   # Decide about the languages used in the project:
   set( _languages CXX C )
   if( ARG_FORTRAN )
      list( APPEND _languages Fortran )
   endif()

   # Declare the project to CMake itself:
   project( ${name} ${_languages} )
   set( CMAKE_PROJECT_NAME ${name} CACHE STRING "Name of the project" )
   set( CMAKE_PROJECT_VERSION ${version} CACHE STRING "Version of the project" )

   # Since we no longer use RPATH for the build area, make sure that the
   # linker commands see the runtime environment of the current project.
   # so that private dependencies on external libraries could be resolved
   # correctly.
   set( CMAKE_C_LINK_EXECUTABLE
      "${CMAKE_BINARY_DIR}/atlas_build_run.sh ${CMAKE_C_LINK_EXECUTABLE}" )
   set( CMAKE_C_LINK_EXECUTABLE
      "${CMAKE_BINARY_DIR}/atlas_build_run.sh ${CMAKE_C_LINK_EXECUTABLE}"
      PARENT_SCOPE )

   set( CMAKE_CXX_LINK_EXECUTABLE
      "${CMAKE_BINARY_DIR}/atlas_build_run.sh ${CMAKE_CXX_LINK_EXECUTABLE}" )
   set( CMAKE_CXX_LINK_EXECUTABLE
      "${CMAKE_BINARY_DIR}/atlas_build_run.sh ${CMAKE_CXX_LINK_EXECUTABLE}"
      PARENT_SCOPE )

   set( CMAKE_C_CREATE_SHARED_LIBRARY
      "${CMAKE_BINARY_DIR}/atlas_build_run.sh ${CMAKE_C_CREATE_SHARED_LIBRARY}" )
   set( CMAKE_C_CREATE_SHARED_LIBRARY
      "${CMAKE_BINARY_DIR}/atlas_build_run.sh ${CMAKE_C_CREATE_SHARED_LIBRARY}"
      PARENT_SCOPE )

   set( CMAKE_CXX_CREATE_SHARED_LIBRARY
      "${CMAKE_BINARY_DIR}/atlas_build_run.sh ${CMAKE_CXX_CREATE_SHARED_LIBRARY}" )
   set( CMAKE_CXX_CREATE_SHARED_LIBRARY
      "${CMAKE_BINARY_DIR}/atlas_build_run.sh ${CMAKE_CXX_CREATE_SHARED_LIBRARY}"
      PARENT_SCOPE )

   set( CMAKE_C_CREATE_SHARED_MODULE
      "${CMAKE_BINARY_DIR}/atlas_build_run.sh ${CMAKE_C_CREATE_SHARED_MODULE}" )
   set( CMAKE_C_CREATE_SHARED_MODULE
      "${CMAKE_BINARY_DIR}/atlas_build_run.sh ${CMAKE_C_CREATE_SHARED_MODULE}"
      PARENT_SCOPE )

   set( CMAKE_CXX_CREATE_SHARED_MODULE
      "${CMAKE_BINARY_DIR}/atlas_build_run.sh ${CMAKE_CXX_CREATE_SHARED_MODULE}" )
   set( CMAKE_CXX_CREATE_SHARED_MODULE
      "${CMAKE_BINARY_DIR}/atlas_build_run.sh ${CMAKE_CXX_CREATE_SHARED_MODULE}"
      PARENT_SCOPE )

   # Decode the version string into 3 numbers:
   set( PROJECT_VERSION_REGEX "([0-9]+).([0-9]+).([0-9]+)" )
   if( CMAKE_PROJECT_VERSION MATCHES ${PROJECT_VERSION_REGEX} )
      set( CMAKE_PROJECT_VERSION_MAJOR ${CMAKE_MATCH_1} CACHE INTERNAL
         "Major version of the project")
      set( CMAKE_PROJECT_VERSION_MINOR ${CMAKE_MATCH_2} CACHE INTERNAL
         "Minor version of the project")
      set( CMAKE_PROJECT_VERSION_PATCH ${CMAKE_MATCH_3} CACHE INTERNAL
         "Patch version of the project")
   else()
      # Apparently this is something else, like a nightly:
      set( CMAKE_PROJECT_VERSION_MAJOR 999 )
      set( CMAKE_PROJECT_VERSION_MINOR 999 )
      set( CMAKE_PROJECT_VERSION_PATCH 0 )
   endif()

   # Tell the user what build type is being used:
   if( CMAKE_BUILD_TYPE )
      message( STATUS "Using build type: ${CMAKE_BUILD_TYPE}" )
   else()
      message( STATUS
         "No explicit build type requested, using \"Default\" build" )
   endif()

   # Construct a platform name:
   atlas_platform_id( _platform )
   set( ATLAS_PLATFORM ${_platform} CACHE STRING
      "Build platform of the project" FORCE )
   set( BUILDNAME ${_platform} CACHE STRING
      "Build platform for CDash" FORCE )
   message( STATUS "Using platform name: ${ATLAS_PLATFORM}" )

   # Set a default installation path. One that RPM making works with:
   if( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
      set( CMAKE_INSTALL_PREFIX "/InstallArea/${ATLAS_PLATFORM}"
         CACHE PATH "Installation path for the project" FORCE )
   endif()

   # Set where to put binary files during compilation:
   set( CMAKE_LIBRARY_OUTPUT_DIRECTORY
      "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/lib" CACHE STRING
      "Directory used to store shared libraries during compilation" FORCE )
   set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY
      "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/lib" CACHE STRING
      "Directory used to store static libraries during compilation" FORCE )
   set( CMAKE_RUNTIME_OUTPUT_DIRECTORY
      "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/bin" CACHE STRING
      "Directory used to store executables during compilation" FORCE )
   # Force Xcode to use these directories for any target...
   foreach( _config DEBUG RELEASE MINSIZEREL RELWITHDEBINFO )
      set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_config}
         "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/lib" CACHE STRING
         "Directory used to store shared libraries during compilation" FORCE )
      set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_config}
         "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/lib" CACHE STRING
         "Directory used to store static libraries during compilation" FORCE )
      set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_config}
         "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/bin" CACHE STRING
         "Directory used to store executables during compilation" FORCE )
   endforeach()
   set( CMAKE_INCLUDE_OUTPUT_DIRECTORY
      "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/include" CACHE STRING
      "Directory used to look up header files during compilation" FORCE )
   set( CMAKE_PYTHON_OUTPUT_DIRECTORY
      "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/python" CACHE STRING
      "Directory collecting python modules in the build area" FORCE )
   set( CMAKE_DATA_OUTPUT_DIRECTORY
      "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/data" CACHE STRING
      "Directory collecting data files in the build area" FORCE )
   set( CMAKE_SCRIPT_OUTPUT_DIRECTORY
      "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/scripts" CACHE STRING
      "Directory collecting script files in the build area" FORCE )
   set( CMAKE_JOBOPT_OUTPUT_DIRECTORY
      "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/jobOptions" CACHE STRING
      "Directory collecting jobOptions in the build area" FORCE )
   set( CMAKE_XML_OUTPUT_DIRECTORY
      "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/XML" CACHE STRING
      "Directory collecting XML files in the build area" FORCE )
   set( CMAKE_SHARE_OUTPUT_DIRECTORY
      "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/share" CACHE STRING
      "Directory collecting data files" FORCE )
   set( CMAKE_DOC_OUTPUT_DIRECTORY
      "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/doc" CACHE STRING
      "Directory collecting documentation files" FORCE )

   # Set where to put files during installation:
   set( CMAKE_INSTALL_LIBDIR "lib"
      CACHE STRING "Library installation directory" FORCE )
   set( CMAKE_INSTALL_BINDIR "bin"
      CACHE STRING "Executable installation directory" FORCE )
   set( CMAKE_INSTALL_TESTDIR "test-bin"
      CACHE STRING "Test executable installation directory" FORCE )
   # Force Xcode to use these directories for any target...
   foreach( _config DEBUG;RELEASE;MINSIZEREL;RELWITHDEBINFO )
      set( CMAKE_INSTALL_LIBDIR_${_config} "lib"
         CACHE STRING "Library installation directory" FORCE )
      set( CMAKE_INSTALL_BINDIR_${_config} "bin"
         CACHE STRING "Executable installation directory" FORCE )
      set( CMAKE_INSTALL_TESTDIR_${_config} "test-bin"
         CACHE STRING "Test executable installation directory" FORCE )
   endforeach()
   set( CMAKE_INSTALL_INCDIR "${DESTDIR}/${CMAKE_INSTALL_PREFIX}/include"
      CACHE STRING "Header installation directory" FORCE )
   set( CMAKE_INSTALL_SRCDIR "src" CACHE STRING
      "Package source installation directory" FORCE )
   set( CMAKE_INSTALL_PYTHONDIR "python" CACHE STRING
      "Python installation directory" FORCE )
   set( CMAKE_INSTALL_DATADIR "data" CACHE STRING
      "Data installation directory" FORCE )
   set( CMAKE_INSTALL_SCRIPTDIR "scripts" CACHE STRING
      "Script installation directory" FORCE )
   set( CMAKE_INSTALL_JOBOPTDIR "jobOptions" CACHE STRING
      "JobOption installation directory" FORCE )
   set( CMAKE_INSTALL_XMLDIR "XML" CACHE STRING
      "XML installation directory" FORCE )
   set( CMAKE_INSTALL_SHAREDIR "share" CACHE STRING
      "Shared data file installation directory" FORCE )
   set( CMAKE_INSTALL_DOCDIR "doc" CACHE STRING
      "Documentation file installation directory" FORCE )
   set( CMAKE_INSTALL_CMAKEDIR "cmake" CACHE STRING
      "CMake file installation directory" FORCE )

   # The default installation component name:
   set( CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "Main"
      CACHE STRING "Default installation component name" )

   # Enable folder view in IDEs:
   set_property( GLOBAL PROPERTY USE_FOLDERS ON )

   # Install all the CMake module code into the install area:
   find_file( _thisFile NAMES AtlasFunctions.cmake
      PATHS ${CMAKE_MODULE_PATH} )
   get_filename_component( _thisDir ${_thisFile} PATH )
   install( DIRECTORY ${_thisDir}/
      DESTINATION ${CMAKE_INSTALL_CMAKEDIR}/modules
      USE_SOURCE_PERMISSIONS
      PATTERN ".svn" EXCLUDE
      PATTERN "*~" EXCLUDE )
   mark_as_advanced( _thisFile )
   unset( _thisFile )
   unset( _thisDir )

   # Find the packages in the current working directory:
   set( _packageDirs )
   file( GLOB_RECURSE cmakelist_files RELATIVE ${CMAKE_SOURCE_DIR}
      CMakeLists.txt )
   foreach( file ${cmakelist_files} )
      # Ignore the top level file itself:
      if( NOT file STREQUAL CMakeLists.txt )
         get_filename_component( package ${file} PATH )
         list( APPEND _packageDirs ${package} )
         unset( package )
      endif()
   endforeach()
   unset( cmakelist_files )
   list( LENGTH _packageDirs _nPackageDirs )
   message( STATUS "Found ${_nPackageDirs} package(s)" )

   # Reset the base project variables:
   set( ATLAS_BASE_PROJECTS )
   set( ATLAS_BASE_PROJECT_NAMES )

   # If the build depends on an installed release, let's find it now:
   if( ARG_USE )
      # Loop over all specified base releases, in the order in which they
      # were specified:
      set( _bases ${ARG_USE} )
      while( _bases )
         # Check that the number of arguments looks good:
         list( LENGTH _bases _len )
         if( _len LESS 2 )
            message( SEND_ERROR
               "Wrong number of arguments given to the USE option" )
            return()
         endif()
         # Extract the release name and version, and then remove these entries
         # from the list:
         list( GET _bases 0 _base_project )
         list( GET _bases 1 _base_version )
         list( REMOVE_AT _bases 0 1 )
         # Make sure that the project version is a regular version number:
         if( NOT _base_version MATCHES "^[0-9]+[0-9.]*" )
            # Let's not specify a version in this case...
            message( STATUS "Using base project \"${_base_project}\" without "
               "its \"${_base_version}\" version name/number" )
            set( _base_version )
         endif()
         # Find the base release:
         find_package( ${_base_project} ${_base_version} QUIET )
         # Remember what projects, and what exact versions we found:
         list( APPEND ATLAS_BASE_PROJECTS ${_base_project}
            ${${_base_project}_VERSION} )
         set( ATLAS_BASE_PROJECT_NAMES
            "${ATLAS_BASE_PROJECT_NAMES} ${_base_project}" )
      endwhile()
      unset( _bases )
      unset( _base_project )
      unset( _base_version )
      unset( _len )
   endif()

   # Reset the platform name, as the base projects may use a different one than
   # the one used for this project.
   set( ATLAS_PLATFORM ${_platform} )
   set( BUILDNAME ${_platform} )

   # Set the ATLAS_BASE_PROJECTS variable in the parent scope. To be able to
   # use it in the CPack configuration:
   set( ATLAS_BASE_PROJECTS ${ATLAS_BASE_PROJECTS} PARENT_SCOPE )

   # Include the packages:
   set( _counter 0 )
   foreach( _pkgDir ${_packageDirs} )
      math( EXPR _doNotPrint "${_counter} % 10" )
      math( EXPR _counter "${_counter} + 1" )
      if( NOT _doNotPrint )
         message( STATUS "Configuring package ${_counter} / ${_nPackageDirs}" )
      endif()
      add_subdirectory( ${_pkgDir} )
   endforeach()
   unset( _counter )
   unset( _doNotPrint )

   # Find the releases' packages again, this time setting each of them up:
   if( ARG_USE )
      # The include mode that we'll be using:
      if( ARG_RELEASE_RECOMPILE )
         set( _include_mode RECOMPILE )
      else()
         set( _include_mode INCLUDE )
      endif()
      # Loop over all specified base releases, in reverse order:
      set( _bases ${ARG_USE} )
      while( _bases )
         # Extract the release name and version, and then remove these entries
         # from the list:
         list( LENGTH _bases _length )
         math( EXPR _projNameIdx "${_length} - 2" )
         math( EXPR _projVersIdx "${_length} - 1" )
         list( GET _bases ${_projNameIdx} _base_project )
         list( GET _bases ${_projVersIdx} _base_version )
         list( REMOVE_AT _bases ${_projNameIdx} ${_projVersIdx} )
         # Make sure that the project version is a regular version number:
         if( NOT _base_version MATCHES "^[0-9]+[0-9.]*" )
            # Let's not specify a version in this case...
            message( STATUS "Using base project \"${_base_project}\" without "
               "its \"${_base_version}\" version name/number" )
            set( _base_version )
         endif()
         # Find the base release:
         find_package( ${_base_project} ${_base_version}
            COMPONENTS ${_include_mode} QUIET )
      endwhile()
      unset( _include_mode )
      unset( _bases )
      unset( _length )
      unset( _projNameIdx )
      unset( _projVersIdx )
      unset( _base_project )
      unset( _base_version )
   endif()

   # Reset the platform name, as the base projects may use a different one than
   # the one used for this project.
   set( ATLAS_PLATFORM ${_platform} )
   set( ATLAS_PLATFORM ${_platform} PARENT_SCOPE )
   set( BUILDNAME ${_platform} )

   # Start constructing the files for the sub-project setup:
   get_property( _compiledPackages GLOBAL PROPERTY
      ATLAS_PACKAGES_TO_COMPILE )
   atlas_sort_packages( _compiledPackages _sortedPackages )
   unset( _compiledPackages )

   # Set the list with a name that can be picked up by other
   # pieces of the code:
   set( CTEST_PROJECT_SUBPROJECTS ${_sortedPackages} )

   # Get the number of processor cores to use for the compilation.
   # Used by the release building script(s).
   atlas_cpu_cores( PROCESSOR_COUNT )

   # Get the name of the running machine. Needed for the CTest script(s).
   site_name( SITE_NAME )

   # Generate the build control script(s):
   find_file( _expBuild ExperimentalBuild.cmake.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   configure_file( ${_expBuild}
      ${CMAKE_BINARY_DIR}/ExperimentalBuild.cmake @ONLY )
   mark_as_advanced( _expBuild )
   unset( _expBuild )
   find_file( _nightlyBuild NightlyBuild.cmake.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   configure_file( ${_nightlyBuild}
      ${CMAKE_BINARY_DIR}/NightlyBuild.cmake @ONLY )
   mark_as_advanced( _nightlyBuild )
   unset( _nightlyBuild )

   # Generat the Project.xml file, which describes to CDash the internal
   # structure of this project.
   set( _projectFileName ${CMAKE_BINARY_DIR}/Project.xml )
   file( WRITE ${_projectFileName}
      "<!-- Auto-generated file, please do not edit -->\n"
      "<Project name=\"${name}\" >\n" )
   foreach( pkg ${_sortedPackages} )
      file( APPEND ${_projectFileName} "   <SubProject name=\"${pkg}\" >\n" )
      get_property( dependencies GLOBAL PROPERTY
         ATLAS_PACKAGE_${pkg}_DEPENDENCIES )
      foreach( dep ${dependencies} )
         file( APPEND ${_projectFileName}
            "      <Dependency name=\"${dep}\" />\n" )
      endforeach()
      file( APPEND ${_projectFileName} "   </SubProject>\n" )
   endforeach()
   file( APPEND ${_projectFileName} "</Project>\n" )
   unset( _projectFileName )
   unset( _sortedPackages )

   # Generate a packages.txt file that lists the names and versions of all the
   # packages in the release.
   set( _packagesFileName ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/packages.txt )
   file( WRITE ${_packagesFileName}
      "# Regular package(s) built in ${CMAKE_PROJECT_NAME} - "
      "${CMAKE_PROJECT_VERSION}\n" )
   foreach( _pkgDir ${_packageDirs} )
      # Get the version of the package:
      set( _version "<unknown>" )
      if( EXISTS "${CMAKE_SOURCE_DIR}/${_pkgDir}/version.cmake" )
         file( READ "${CMAKE_SOURCE_DIR}/${_pkgDir}/version.cmake"
            _version LIMIT 10000 )
         string( STRIP ${_version} _version )
      elseif( EXISTS "${CMAKE_SOURCE_DIR}/${_pkgDir}/cmt/version.cmt" )
         # For backwards compatibility:
         file( READ "${CMAKE_SOURCE_DIR}/${_pkgDir}/cmt/version.cmt"
            _version LIMIT 10000 )
         string( STRIP ${_version} _version )
      endif()
      # Add a line into the file:
      file( APPEND ${_packagesFileName} "${_pkgDir} ${_version}\n" )
   endforeach()
   # Add some possible special packages to the file:
   get_property( _specialPackagesSet GLOBAL PROPERTY ATLAS_SPECIAL_PACKAGES
      SET )
   if( _specialPackagesSet )
      file( APPEND ${_packagesFileName}
         "# Special package(s) in ${CMAKE_PROJECT_NAME} - "
         "${CMAKE_PROJECT_VERSION}\n" )
      get_property( _specialPackages GLOBAL PROPERTY ATLAS_SPECIAL_PACKAGES )
      while( _specialPackages )
         # A sanity check:
         list( LENGTH _specialPackages _len )
         if( _len LESS 2 )
            message( WARNING "Incorrect setting in ATLAS_SPECIAL_PACKAGES" )
            break()
         endif()
         # Get the package name and version:
         list( GET _specialPackages 0 _package )
         list( GET _specialPackages 1 _version )
         list( REMOVE_AT _specialPackages 0 1 )
         # Add the line:
         file( APPEND ${_packagesFileName} "${_package} ${_version}\n" )
         # Clean up:
         unset( _package )
         unset( _version )
      endwhile()
   endif()
   unset( _specialPackagesSet )
   # Tell the user what happened:
   message( STATUS "Generated file: ${_packagesFileName}" )
   # Install this file:
   install( FILES ${_packagesFileName} DESTINATION . )
   # Clean up:
   unset( _version )
   unset( _packagesFileName )
   unset( _packageDirs )

   # Generate a compilers.txt file that list the types and versions of the
   # compilers that were used to build this project.
   set( _compilersFileName ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/compilers.txt )
   file( WRITE ${_compilersFileName}
      "# Compilers used to build ${CMAKE_PROJECT_NAME} - "
      "${CMAKE_PROJECT_VERSION}\n"
      "CMAKE_C_COMPILER_ID ${CMAKE_C_COMPILER_ID}\n"
      "CMAKE_C_COMPILER_VERSION ${CMAKE_C_COMPILER_VERSION}\n"
      "CMAKE_CXX_COMPILER_ID ${CMAKE_CXX_COMPILER_ID}\n"
      "CMAKE_CXX_COMPILER_VERSION ${CMAKE_CXX_COMPILER_VERSION}\n"
      "CMAKE_Fortran_COMPILER_ID ${CMAKE_Fortran_COMPILER_ID}\n"
      "CMAKE_Fortran_COMPILER_VERSION ${CMAKE_Fortran_COMPILER_VERSION}\n" )
   # Tell the user what happened:
   message( STATUS "Generated file: ${_compilersFileName}" )
   # Install this file:
   install( FILES ${_compilersFileName} DESTINATION . )
   # Clean up:
   unset( _compilersFileName )

   # Export the project's library targets:
   get_property( _exports GLOBAL PROPERTY ATLAS_EXPORTS )
   if( _exports )
      install( EXPORT ${CMAKE_PROJECT_NAME}Targets
         FILE ${CMAKE_PROJECT_NAME}Config-targets.cmake
         DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
         NAMESPACE "${CMAKE_PROJECT_NAME}::" )
      # Sanitize the created file, replacing all FATAL_ERROR messages
      # with WARNING ones:
      set( CMAKE_TARGETS_FILE
         ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_CMAKEDIR}/${CMAKE_PROJECT_NAME}Config-targets.cmake )
      find_file( _sanitizer atlas_export_sanitizer.cmake.in
         PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
      configure_file( ${_sanitizer}
         ${CMAKE_BINARY_DIR}/atlas_export_sanitizer.cmake @ONLY )
      install( SCRIPT ${CMAKE_BINARY_DIR}/atlas_export_sanitizer.cmake )
      unset( CMAKE_TARGETS_FILE )
      mark_as_advanced( _sanitizer )
      unset( _sanitizer )
   endif()

   # Get the names of the exported targets and packages, needed for the project
   # configuration file generation.
   get_property( ATLAS_EXPORTED_TARGETS GLOBAL PROPERTY ATLAS_EXPORTED_TARGETS )
   get_property( ATLAS_EXPORTED_PACKAGES GLOBAL PROPERTY
      ATLAS_EXPORTED_PACKAGES )

   # Construct the project's configuration file(s):
   find_file( _projConf ProjectConfig.cmake.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   configure_file( ${_projConf}
      ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake @ONLY )
   mark_as_advanced( _projConf )
   unset( _projConf )
   install( FILES ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
      DESTINATION ${CMAKE_INSTALL_CMAKEDIR} )
   find_file( _projVers ProjectConfig-version.cmake.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   configure_file( ${_projVers}
      ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config-version.cmake @ONLY )
   mark_as_advanced( _projVers )
   unset( _projVers )
   install( FILES
      ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config-version.cmake
      DESTINATION ${CMAKE_INSTALL_CMAKEDIR} )

   # Set up an environment setup file which should be enough to source
   # in order to set up an environment for compiling code (using CMake)
   # against the installed release.
   find_file( _setupBuild setup_build.sh.in
      PATH_SUFFIXES scripts PATHS ${CMAKE_MODULE_PATH} )
   configure_file( ${_setupBuild}
      ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/setup_build.sh @ONLY )
   mark_as_advanced( _setupBuild )
   unset( _setupBuild )
   install( FILES ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/setup_build.sh
      DESTINATION . )

   # Set up the environment setup file meant for BASH/ZSH:
   find_file( _setupSkeleton setup.sh.in
      PATH_SUFFIXES scripts PATHS ${CMAKE_MODULE_PATH} )
   configure_file( ${_setupSkeleton}
      ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/setup.sh @ONLY )
   mark_as_advanced( _setupSkeleton )
   unset( _setupSkeleton )
   install( FILES ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/setup.sh
      DESTINATION . )
   set_directory_properties( PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
      ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/setup.sh )

   # Set up the environment setup file meant for TCSH:
   find_file( _setupSkeletonTcsh setup.csh.in
      PATH_SUFFIXES scripts PATHS ${CMAKE_MODULE_PATH} )
   configure_file( ${_setupSkeletonTcsh}
      ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/setup.csh @ONLY )
   mark_as_advanced( _setupSkeletonTcsh )
   unset( _setupSkeletonTcsh )
   install( FILES ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/setup.csh
      DESTINATION . )
   set_directory_properties( PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
      ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/setup.csh )

   # Set up some variables for the client CMakeLists.txt file generation.
   # Decide whether the clients need to find fortran:
   if( ARG_FORTRAN )
      set( FORTRAN_ARGUMENT "FORTRAN" )
   else()
      set( FORTRAN_ARGUMENT )
   endif()
   # Decide whether AtlasLCG will be available to the clients:
   get_property( _packages_found GLOBAL PROPERTY PACKAGES_FOUND )
   list( FIND _packages_found LCG _lcg_index )
   if( NOT "${_lcg_index}" EQUAL "-1" )
      set( _h "# Set up the runtime environment setup script(s):\n" )
      set( _h "${_h}lcg_generate_env( SH_FILE \${CMAKE_BINARY_DIR}/" )
      set( _h "${_h}\${ATLAS_PLATFORM}/env_setup.sh )\n" )
      set( _h "${_h}install( FILES \${CMAKE_BINARY_DIR}/\${ATLAS_PLATFORM}/" )
      set( _h "${_h}env_setup.sh\n" )
      set( _h "${_h}   DESTINATION . )" )
      set( ENVFILE_GEN ${_h} )
      unset( _h )
   else()
      set( ENVFILE_GEN )
   endif()

   # Set up a project CMakeLists.txt files that projects building on top
   # of this one may want to use.
   find_file( _clientCMakeLists ClientCMakeLists.txt.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   configure_file( ${_clientCMakeLists}
      ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/ClientCMakeLists.txt @ONLY )
   mark_as_advanced( _clientCMakeLists )
   unset( _clientCMakeLists )
   install( FILES ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/ClientCMakeLists.txt
      DESTINATION . )

   # Clean up:
   unset( FORTRAN_ARGUMENT )
   unset( _packages_found )
   unset( ENVFILE_GEN )

   # Add the CTest customisation file to the build tree:
   find_file( _ctestCustom CTestCustom.cmake.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   configure_file( ${_ctestCustom}
      ${CMAKE_BINARY_DIR}/CTestCustom.cmake @ONLY )
   mark_as_advanced( _ctestCustom )
   unset( _ctestCustom )

   # Set up a script that can be used during the build to run executables
   # in a "full runtime environment":
   find_file( _buildRun atlas_build_run.sh.in
      PATH_SUFFIXES scripts PATHS ${CMAKE_MODULE_PATH} )
   configure_file( ${_buildRun} ${CMAKE_BINARY_DIR}/atlas_build_run.sh @ONLY )
   mark_as_advanced( _buildRun )
   unset( _buildRun )

   # Set variables necessary for the ReleaseData generation:
   string( TIMESTAMP CMAKE_BUILD_DATE "%Y-%m-%d %H:%M" )
   if( "$ENV{NICOS_NIGHTLY_NAME}" STREQUAL "" )
      set( ENV{NICOS_NIGHTLY_NAME} "private" )
   endif()
   if( "$ENV{NICOS_PROJECT_RELNAME_COPY}" STREQUAL "" )
      set( ENV{NICOS_PROJECT_RELNAME_COPY} "private" )
   endif()
   if( "$ENV{NICOS_VAL_MODE}" STREQUAL "" )
      set( ENV{NICOS_VAL_MODE} "None" )
   endif()
   if( "$ENV{NICOS_ATLAS_ALT_RELEASE}" STREQUAL "" )
      set( ENV{NICOS_ATLAS_ALT_RELEASE} "None" )
   endif()
   if( "$ENV{NICOS_JOBID}" STREQUAL "" )
      set( ENV{NICOS_JOBID} "0000" )
   endif()
   if( "$ENV{gcc_version}" STREQUAL "" )
      set( ENV{gcc_version}
         "${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}" )
   endif()

   # And now generate/install the ReleaseData file:
   find_file( _releaseData ReleaseData.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   configure_file( ${_releaseData}
      ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/ReleaseData )
   mark_as_advanced( _releaseData )
   unset( _releaseData )
   install( FILES ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/ReleaseData
      DESTINATION . )

   # Check if Doxygen is available. With a minimum version.
   find_package( Doxygen 1.8.9.1 )

   # Create a Doxyfile for the project:
   # We should be setting up CMAKE_INPUT_TAGFILES here...
   set( CMAKE_OUTPUT_TAGFILE
      ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/doc/${CMAKE_PROJECT_NAME}.tag )
   install( FILES
      ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/doc/${CMAKE_PROJECT_NAME}.tag
      DESTINATION doc OPTIONAL )
   find_file( _doxyfile Doxyfile.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   configure_file( ${_doxyfile} ${CMAKE_BINARY_DIR}/Doxyfile @ONLY )
   mark_as_advanced( _doxyfile )

   # If Doxygen is available, set up a target with it:
   if( DOXYGEN_FOUND )
      message( STATUS "Creating the \"doc\" target" )
      add_custom_target( doc
         COMMAND ${CMAKE_BINARY_DIR}/atlas_build_run.sh
         ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
         COMMENT "Generating the Doxygen documentation" )
      # Also set up an installation rule for the files created by this rule:
      install( DIRECTORY ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/doc
         DESTINATION . USE_SOURCE_PERMISSIONS OPTIONAL )
   endif()

   # Set up all project-wide operations, which come after all the packages
   # have been built:
   atlas_merge_project_files()

endfunction( atlas_project )

# This macro is used as the first declaration in software packages. It sets
# up a few things:
#  - Sets a variable called ATLAS_PACKAGE that holds the name of the package,
#    and is used by almost all of the build/install commands used later on.
#  - Sets up an interface library called "<PkgName>Pkg". It is used to construct
#    the (include) dependencies of the package. By making this interface library
#    depend on the interface libraries of all the dependent packages.
#  - Sets up a custom target called "Package_<PkgName>", which will eventually
#    have all build operations of the package as a dependency. Which is used
#    both for interactive compilation (build everything in a given package),
#    and to provide build results to the nightly system.
#  - It installs all the compilable sources of the package into the install
#    area. Making it possible to recompile the package from the base release
#    when the user builds a base package in his/her work area.
#
# Usage: atlas_subdir( CxxUtils )
#
macro( atlas_subdir name )

   # Set the package's name, so other function calls in the package's
   # CMakeLists.txt file could pick it up.
   set( ATLAS_PACKAGE ${name} )
   set_property( GLOBAL APPEND PROPERTY ATLAS_EXPORTED_PACKAGES ${name} )

   # Create a target that can be used to get the names of all the packages
   # in the current project, in a generator expression:
   if( NOT TARGET ATLAS_PACKAGES_TARGET )
      add_custom_target( ATLAS_PACKAGES_TARGET )
      set_property( TARGET ATLAS_PACKAGES_TARGET PROPERTY FOLDER
         ${CMAKE_PROJECT_NAME} )
   endif()

   # If we are in "release mode", check if this package needs to be
   # recompiled or not:
   if( ATLAS_RELEASE_MODE )

      # If the package is in the test area, then exit the package right away:
      if( TARGET Package_${name} )
         return()
      endif()

      # Only proceed if we are not in dryrun mode:
      if( NOT ATLAS_RELEASE_RECOMPILE_DRYRUN )

         # Check if this package needs to be recompiled:
         get_property( _recompilePackages GLOBAL PROPERTY
            ATLAS_PACKAGES_TO_RECOMPILE )
         list( FIND _recompilePackages ${name} _index )
         if( _index EQUAL -1 )
            # If not, then create the relevant interface libraries as imported
            # ones. First set up ${name}Pkg:
            if( NOT TARGET ${ATLAS_RELEASE_NAME}::${name}Pkg )
               message( SEND_ERROR
                  "Interface library ${name}Pkg not available from release "
                  "${ATLAS_RELEASE_NAME}" )
            else()
               if( NOT TARGET ${name}Pkg )
                  add_library( ${name}Pkg INTERFACE IMPORTED GLOBAL )
                  get_property( _incSet TARGET ${ATLAS_RELEASE_NAME}::${name}Pkg
                     PROPERTY INTERFACE_INCLUDE_DIRECTORIES SET )
                  if( _incSet )
                     get_target_property( _includes
                        ${ATLAS_RELEASE_NAME}::${name}Pkg
                        INTERFACE_INCLUDE_DIRECTORIES )
                     set_target_properties( ${name}Pkg PROPERTIES
                        INTERFACE_INCLUDE_DIRECTORIES "${_includes}" )
                     unset( _includes )
                  endif()
                  get_property( _libSet TARGET ${ATLAS_RELEASE_NAME}::${name}Pkg
                     PROPERTY INTERFACE_LINK_LIBRARIES SET )
                  if( _libSet )
                     get_target_property( _libraries
                        ${ATLAS_RELEASE_NAME}::${name}Pkg
                        INTERFACE_LINK_LIBRARIES )
                     set_target_properties( ${name}Pkg PROPERTIES
                        INTERFACE_LINK_LIBRARIES "${_libraries}" )
                     unset( _libraries )
                  endif()
               endif()
            endif()
            # Now set up ${name}PkgPrivate:
            if( NOT TARGET ${ATLAS_RELEASE_NAME}::${name}PkgPrivate )
               message( SEND_ERROR
                  "Interface library ${name}PkgPrivate not available from "
                  "release ${ATLAS_RELEASE_NAME}" )
            else()
               if( NOT TARGET ${name}PkgPrivate )
                  add_library( ${name}PkgPrivate INTERFACE IMPORTED GLOBAL )
                  get_property( _incSet
                     TARGET ${ATLAS_RELEASE_NAME}::${name}PkgPrivate
                     PROPERTY INTERFACE_INCLUDE_DIRECTORIES SET )
                  if( _incSet )
                     get_target_property( _includes
                        ${ATLAS_RELEASE_NAME}::${name}PkgPrivate
                        INTERFACE_INCLUDE_DIRECTORIES )
                     set_target_properties( ${name}PkgPrivate PROPERTIES
                        INTERFACE_INCLUDE_DIRECTORIES "${_includes}" )
                     unset( _includes )
                  endif()
                  get_property( _libSet
                     TARGET ${ATLAS_RELEASE_NAME}::${name}PkgPrivate
                     PROPERTY INTERFACE_LINK_LIBRARIES SET )
                  if( _libSet )
                     get_target_property( _libraries
                        ${ATLAS_RELEASE_NAME}::${name}PkgPrivate
                        INTERFACE_LINK_LIBRARIES )
                     set_target_properties( ${name}PkgPrivate PROPERTIES
                        INTERFACE_LINK_LIBRARIES "${_libraries}" )
                     unset( _libraries )
                  endif()
               endif()
            endif()
            # Clean up:
            unset( _incSet )
            unset( _libSet )
         else()

            # The package is being built, add it to the list of "local"
            # packages:
            set_property( TARGET ATLAS_PACKAGES_TARGET APPEND PROPERTY
               ATLAS_PACKAGES ${name} )

            # If the target doesn't exist yet (after seeing all the packages
            # in the work area), then define it now:
            if( NOT TARGET ${name}Pkg )
               add_library( ${name}Pkg INTERFACE )
               atlas_get_package_dir( _dir )
               target_include_directories( ${name}Pkg INTERFACE
                  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                  $<INSTALL_INTERFACE:src/${_dir}> )
               unset( _dir )
            endif()
            if( NOT TARGET ${name}PkgPrivate )
               add_library( ${name}PkgPrivate INTERFACE )
            endif()

            # Create the custom targets:
            if( NOT TARGET Package_${name} )
               add_custom_target( Package_${name} ALL
                  COMMAND ${CMAKE_COMMAND} -E echo
                  "${name}: Package re-build succeeded" )
            endif()
            add_dependencies( Package_${name} ${name}Pkg
               ${name}PkgPrivate )

            # Put the package targets into a non-too-prominent folder:
            set_property( TARGET Package_${name}
               PROPERTY FOLDER ${ATLAS_RELEASE_NAME} )

         endif()
      endif()

   else()

      # We have the package in the work area:
      message( STATUS "Configuring the build of package: ${name}" )

      # The package is being built, add it to the list of local
      # packages:
      set_property( TARGET ATLAS_PACKAGES_TARGET APPEND PROPERTY
         ATLAS_PACKAGES ${name} )

      # Check if a package with this name was already declared:
      if( TARGET ${name}Pkg OR TARGET ${name}PkgPrivate OR
            TARGET Package_${name} )
         message( WARNING "Package ${name} is already declared." )
      else()

         # Declare that the package is defined:
         set_property( GLOBAL PROPERTY ATLAS_PACKAGE_${name} 1 )

         # Add this package to the global list of packages being built:
         get_property( _compiledPackages GLOBAL PROPERTY
            ATLAS_PACKAGES_TO_COMPILE )
         list( APPEND _compiledPackages ${name} )
         list( REMOVE_DUPLICATES _compiledPackages )
         set_property( GLOBAL PROPERTY ATLAS_PACKAGES_TO_COMPILE
            ${_compiledPackages} )

         # Get the source path of the package:
         atlas_get_package_dir( pkgDir )

         # The initial target of the package is now an interface library. One
         # that declares the source directory of the package as an include
         # directory. And which depends on the similar interface libraries
         # from all the dependent packages.
         add_library( ${name}Pkg INTERFACE )
         target_include_directories( ${name}Pkg INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:src/${pkgDir}> )
         unset( _dir )
         install( TARGETS ${name}Pkg
            EXPORT ${CMAKE_PROJECT_NAME}Targets
            DESTINATION ${CMAKE_INSTALL_LIBDIR} )
         set_property( GLOBAL PROPERTY ATLAS_EXPORTS ON )
         set_property( GLOBAL APPEND PROPERTY ATLAS_EXPORTED_TARGETS
            ${name}Pkg )

         # Create an interface library that will collect the private
         # dependencies of the package.
         add_library( ${name}PkgPrivate INTERFACE )
         install( TARGETS ${name}PkgPrivate
            EXPORT ${CMAKE_PROJECT_NAME}Targets
            DESTINATION ${CMAKE_INSTALL_LIBDIR} )
         set_property( GLOBAL APPEND PROPERTY ATLAS_EXPORTED_TARGETS
            ${name}PkgPrivate )

         # Create a final target for the package. One that will depend on all
         # build operations of the package.
         add_custom_target( Package_${name} ALL
            COMMAND ${CMAKE_COMMAND} -E echo
            "${name}: Package build succeeded" )

         # And now connect up the two:
         add_dependencies( Package_${name} ${name}Pkg ${name}PkgPrivate )

         # Put the package targets into the package's folder:
         atlas_get_package_dir( pkgDir )
         set_property( TARGET Package_${name} PROPERTY FOLDER ${pkgDir} )

         # Set up the installation of the source files from the package:
         install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
            DESTINATION ${CMAKE_INSTALL_SRCDIR}/${pkgDir}
            USE_SOURCE_PERMISSIONS
            PATTERN ".svn" EXCLUDE )

      endif()

   endif()

endmacro( atlas_subdir )

# This function declares the dependencies of a package on other packages.
# It takes a list of package names with their full path, but in reality
# it only uses the names of the packages themselves. (The last directory in the
# path.)
#
# In ATLAS_RELEASE_MODE it also makes the decision whether binaries of the
# package need to be recompiled or not.
#
# Usage: atlas_depends_on_subdirs( [PUBLIC] Control/AthContainers
#                                  [PRIVATE Control/CxxUtils] )
#
function( atlas_depends_on_subdirs )

   # Parse the options (package names) given to the function:
   cmake_parse_arguments( ARG "" "" "PUBLIC;PRIVATE" ${ARGN} )

   # Get the package/subdirectory name:
   atlas_get_package_name( pkgName )

   # Make a decision about the package's recompilation now:
   if( ATLAS_RELEASE_MODE )
      # If we are not in recompilation mode, bail now:
      if( NOT ATLAS_RELEASE_RECOMPILE )
         return()
      endif()
      # If we are in "dryrun" mode, then we just need to figure out
      # whether this package needs to be recompiled. But the
      # ATLAS_PACKAGE_RECOMPILE variable must not yet be set.
      if( ATLAS_RELEASE_RECOMPILE_DRYRUN )
         # If the package is already in the test area, don't try to
         # recompile it:
         if( TARGET Package_${pkgName} )
            return()
         endif()
         # Check if any of the dependent packages are being recompiled:
         set( _recompile OFF )
         foreach( dep ${ARG_PUBLIC} ${ARG_PRIVATE} ${ARG_UNPARSED_ARGUMENTS} )
            # Get the package name that we should use in the dependency:
            get_filename_component( package ${dep} NAME )
            get_property( _package GLOBAL PROPERTY ATLAS_PACKAGE_${package} )
            if( _package EQUAL 1 )
               set( _recompile ON )
               break()
            endif()
         endforeach()
         # If the package doesn't need to be recompiled, we can exit now:
         if( NOT _recompile )
            return()
         endif()
         # Let packages dependent on this one be also recompiled:
         set_property( GLOBAL PROPERTY ATLAS_PACKAGE_${pkgName} 1 )
         # Add this package to the variable set up by the release configuration
         # file:
         get_property( _recompilePackages GLOBAL PROPERTY
            ATLAS_PACKAGES_TO_RECOMPILE )
         list( APPEND _recompilePackages ${pkgName} )
         list( REMOVE_DUPLICATES _recompilePackages )
         set_property( GLOBAL PROPERTY ATLAS_PACKAGES_TO_RECOMPILE
            ${_recompilePackages} )
         return()
      else()
         # If we are past the dryrun stage, check if the package appears
         # in the ATLAS_PACKAGES_TO_RECOMPILE list. If it does, then set the
         # package into recompilation mode.
         get_property( _recompilePackages GLOBAL PROPERTY
            ATLAS_PACKAGES_TO_RECOMPILE )
         list( FIND _recompilePackages ${pkgName} _index )
         if( _index EQUAL -1 )
            # The package is not to be recompiled, we can exit:
            return()
         endif()
         # The package needs to be recompiled:
         set( ATLAS_PACKAGE_RECOMPILE ON PARENT_SCOPE )
         message( STATUS "Configuring the re-build of package: ${pkgName}" )
      endif()
   endif()

   # Link this package's interface library against all of the used packages'
   # interface libraries:
   set( _depDirs )
   set( _pubDepPkgLibs )
   # Process the public dependencies:
   foreach( dep ${ARG_PUBLIC} )
      # Get the package name that we should use in the dependency:
      get_filename_component( _depPackage ${dep} NAME )
      list( APPEND _depDirs ${_depPackage} )
      list( APPEND _pubDepPkgLibs ${_depPackage}Pkg )
   endforeach()
   # Unparsed arguments, when the user didn't specify either PUBLIC or PRIVATE,
   # are considered public dependencies.
   foreach( dep ${ARG_UNPARSED_ARGUMENTS} )
      # Get the package name that we should use in the dependency:
      get_filename_component( _depPackage ${dep} NAME )
      list( APPEND _depDirs ${_depPackage} )
      list( APPEND _pubDepPkgLibs ${_depPackage}Pkg )
   endforeach()
   target_link_libraries( ${pkgName}Pkg INTERFACE
      ${_pubDepPkgLibs} )

   # Private dependencies are added to another interface library, if they
   # exist.
   set( _privDepPkgLibs )
   foreach( dep ${ARG_PRIVATE} )
      # Get the package name that we should use in the dependency:
      get_filename_component( _depPackage ${dep} NAME )
      list( APPEND _depDirs ${_depPackage} )
      list( APPEND _privDepPkgLibs ${_depPackage}Pkg )
   endforeach()
   target_link_libraries( ${pkgName}PkgPrivate INTERFACE
      ${_privDepPkgLibs} )

   # Save the dependencies into a global property, for further use:
   set_property( GLOBAL PROPERTY ATLAS_PACKAGE_${pkgName}_DEPENDENCIES
      ${_depDirs} )

endfunction( atlas_depends_on_subdirs )

# Function creating an alias for a specific file. It is used to create
# aliases of executables/scripts. Mostly just for convenience reasons.
#
# Usage: atlas_add_alias( name command1 command2... )
#
function( atlas_add_alias name )

   # This rule is ignored in release mode:
   if( ATLAS_RELEASE_MODE )
      return()
   endif()

   # A little sanity check:
   if( NOT ARGN )
      message( SEND_ERROR "No command specified for alias ${name}" )
      return()
   endif()

   # Get the package/subdirectory name:
   atlas_get_package_name( pkgName )

   # Get the package directory:
   atlas_get_package_dir( pkgDir )

   # Construct the command to execute:
   set( _cmd )
   foreach( _arg ${ARGN} )
      set( _cmd "${_cmd} \"${_arg}\"" )
   endforeach()

   # Construct the script's contents:
   set( _content "#!/bin/sh\n" )
   set( _content "${_content}set -e\n" )
   set( _content "${_content}exec ${_cmd} \"\$@\"\n" )

   # Create a wrapper script with the requested name:
   file( GENERATE OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name}
      CONTENT ${_content} )
   # Make it executable:
   if( NOT TARGET ${name}AttribSet )
      add_custom_target( ${name}AttribSet ALL
         COMMAND chmod 755 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name} )
      set_property( TARGET ${name}AttribSet PROPERTY LABELS ${pkgName} )
      set_property( TARGET ${name}AttribSet PROPERTY FOLDER ${pkgDir} )
   else()
      message( WARNING "Target ${name}AttribSet already exists. Won't be "
         "able to make ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name} executable" )
   endif()
   # And install it:
   install( PROGRAMS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name}
      DESTINATION ${CMAKE_INSTALL_BINDIR} )

endfunction( atlas_add_alias )

# This function can be used to declare executables in a package.
#
# Usage:  atlas_add_executable( ExecutableName util/source1.cxx...
#                               [INCLUDE_DIRS Include1...]
#                               [LINK_LIBRARIES Library1...] )
#
function( atlas_add_executable exeName )

   # In release recompilation dryrun mode exit now:
   if( ATLAS_RELEASE_RECOMPILE_DRYRUN )
      return()
   endif()

   # Parse the options given to the function:
   cmake_parse_arguments( ARG "" "" "INCLUDE_DIRS;LINK_LIBRARIES" ${ARGN} )

   # Skip the binary building if we are in release mode, but the package is not
   # getting recompiled:
   if( ATLAS_RELEASE_MODE AND NOT ATLAS_PACKAGE_RECOMPILE )
      # If we don't need to recompile because this executable is already
      # built in the work area, then exit now...
      if( TARGET ${exeName} )
         return()
      endif()
      # Check that we have this executable in the release area:
      if( NOT TARGET ${ATLAS_RELEASE_NAME}::${exeName} )
         message( SEND_ERROR
            "Executable ${exeName} not available from release "
            "${ATLAS_RELEASE_NAME}" )
         return()
      endif()
      # Set up the imported executable:
      add_executable( ${exeName} IMPORTED GLOBAL )
      # Set its properties:
      get_property( _builds TARGET ${ATLAS_RELEASE_NAME}::${exeName}
         PROPERTY IMPORTED_CONFIGURATIONS )
      set_target_properties( ${exeName} PROPERTIES
         IMPORTED_CONFIGURATIONS "${_builds}" )
      foreach( _build ${_builds} )
         get_target_property( _location ${ATLAS_RELEASE_NAME}::${exeName}
            IMPORTED_LOCATION_${_build} )
         set_target_properties( ${exeName} PROPERTIES
            IMPORTED_LOCATION_${_build} "${_location}" )
         unset( _location )
      endforeach()
      unset( _builds )
      # Put the imported target into a non-too-prominent folder:
      set_property( TARGET ${exeName}
         PROPERTY FOLDER ${ATLAS_RELEASE_NAME} )
      # Now we are finished:
      return()
   endif()

   # Check if an executable with this name has already been
   # declared:
   if( TARGET ${exeName} )
      message( WARNING "Executable with name ${exeName} already declared" )
      return()
   endif()

   # Set common compiler options:
   atlas_set_compiler_flags()

   # Get the package/subdirectory name:
   atlas_get_package_name( pkgName )

   # Get the package directory:
   atlas_get_package_dir( pkgDir )

   # Allow wildcards in the source names:
   file( GLOB _sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
      ${ARG_UNPARSED_ARGUMENTS} )

   # Put the files into source groups. So they would show up in a ~reasonable
   # way in an IDE like Xcode:
   atlas_group_source_files( ${_sources} )

   # Declare the executable:
   add_executable( ${exeName} ${_sources} )

   # Set it's properties:
   add_dependencies( ${exeName} ${pkgName}Pkg )
   add_dependencies( Package_${pkgName} ${exeName} )
   set_property( TARGET ${exeName} PROPERTY LABELS ${pkgName} )
   set_property( TARGET ${exeName} PROPERTY FOLDER ${pkgDir} )
   target_include_directories( ${exeName} BEFORE PRIVATE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
      $<TARGET_PROPERTY:${pkgName}Pkg,INTERFACE_INCLUDE_DIRECTORIES>
      $<TARGET_PROPERTY:${pkgName}PkgPrivate,INTERFACE_INCLUDE_DIRECTORIES>
      ${ARG_INCLUDE_DIRS} )
   if( ARG_LINK_LIBRARIES )
      target_link_libraries( ${exeName} ${ARG_LINK_LIBRARIES} )
   endif()
   # Link the executable against the dl library as aggressively as possible:
   if( APPLE )
      target_link_libraries( ${exeName} dl )
   else()
      target_link_libraries( ${exeName}
         "-Wl,--no-as-needed -ldl -Wl,--as-needed" )
   endif()

   # In case we are building optimised executables with debug info, and we have
   # objcopy available, detach the debug information into a separate file.
   if( "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" AND CMAKE_OBJCOPY )
      add_custom_command( TARGET ${exeName} POST_BUILD
         COMMAND ${CMAKE_OBJCOPY} --only-keep-debug ${exeName} ${exeName}.dbg
         COMMAND ${CMAKE_OBJCOPY} --strip-debug ${exeName}
         COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=${exeName}.dbg ${exeName}
         WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
         COMMENT
         "Detaching debug info of ${exeName} into ${exeName}.dbg" )
      install( FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${exeName}.dbg
         DESTINATION ${CMAKE_INSTALL_BINDIR}
         COMPONENT "Debug"
         OPTIONAL )
   endif()

   # Set up an alias to the executable with an .exe postfix. One that should
   # warn the user that they should stop using this postfix...
   if( UNIX )
      # The script's contents:
      set( _content "#!/bin/sh\n" )
      set( _content "${_content}set -e\n" )
      set( _content "${_content}echo \"WARNING Please stop using ")
      set( _content "${_content}${exeName}.exe, and use " )
      set( _content "${_content}${exeName} instead\"\n" )
      set( _content "${_content}echo \"\"\n" )
      set( _content "${_content}exec ${exeName} \"\$@\"\n" )
      # Generate the script:
      file( GENERATE OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${exeName}.exe
         CONTENT ${_content} )
      # Make it executable:
      if( NOT TARGET ${exeName}ExeAttribSet )
         add_custom_target( ${exeName}ExeAttribSet ALL
            COMMAND chmod 755 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${exeName}.exe )
         set_property( TARGET ${exeName}ExeAttribSet PROPERTY LABELS
            ${pkgName} )
         set_property( TARGET ${exeName}ExeAttribSet PROPERTY FOLDER
            ${pkgDir} )
      else()
         message( WARNING "Target with name ${exeName}ExeAttribSet already "
            "exists. Won't be able to make "
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${exeName}.exe executable" )
      endif()
      # And install it:
      install( PROGRAMS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${exeName}.exe
         DESTINATION ${CMAKE_INSTALL_BINDIR} )
   endif()

   # Set up the installation of the executable:
   if( ATLAS_RELEASE_MODE )
      install( TARGETS ${exeName}
         DESTINATION ${CMAKE_INSTALL_BINDIR} OPTIONAL )
   else()
      install( TARGETS ${exeName}
         EXPORT ${CMAKE_PROJECT_NAME}Targets
         DESTINATION ${CMAKE_INSTALL_BINDIR} OPTIONAL )
      set_property( GLOBAL PROPERTY ATLAS_EXPORTS ON )
      set_property( GLOBAL APPEND PROPERTY ATLAS_EXPORTED_TARGETS ${exeName} )
   endif()

endfunction( atlas_add_executable )

# This function can be used to declare unit tests in a package. It can be
# called in two ways. Either:
#
#   atlas_add_test( TestName SOURCES test/source1.cxx...
#                   [INCLUDE_DIRS Dir1...]
#                   [LINK_LIBRARIES Library1...]
#                   [EXTRA_PATTERNS patterns]
#                   [PRE_EXEC_SCRIPT script]
#                   [POST_EXEC_SCRIPT script]
#                   [ENVIRONMENT env]
#                   [PROPERTIES <name> <value>...] )
#
# Or like:
#
#   atlas_add_test( TestName SCRIPT test/script.sh
#                   [EXTRA_PATTERNS patterns]
#                   [PRE_EXEC_SCRIPT script]
#                   [POST_EXEC_SCRIPT script]
#                   [ENVIRONMENT env]
#                   [PROPERTIES <name> <value>...] )
#
function( atlas_add_test testName )

   # In release recompilation dryrun mode exit now:
   if( ATLAS_RELEASE_RECOMPILE_DRYRUN )
      return()
   endif()

   # Skip the binary building if we are in release mode, but the package is not
   # getting recompiled:
   if( ATLAS_RELEASE_MODE AND NOT ATLAS_PACKAGE_RECOMPILE )
      return()
   endif()

   # Parse the options given to the function:
   set( _singleParamArgs SCRIPT EXTRA_PATTERNS ENVIRONMENT
      PRE_EXEC_SCRIPT POST_EXEC_SCRIPT )
   set( _multiParamArgs SOURCES INCLUDE_DIRS LINK_LIBRARIES PROPERTIES )
   cmake_parse_arguments( ARG "" "${_singleParamArgs}"
      "${_multiParamArgs}" ${ARGN} )

   # A sanity check:
   if( ( ARG_SOURCES AND ARG_SCRIPT ) OR
         ( NOT ARG_SOURCES AND NOT ARG_SCRIPT ) )
      message( SEND_ERROR "Script must be called with either SOURCES or "
         "SCRIPT in the arguments" )
      return()
   endif()

   # Set common compiler options:
   atlas_set_compiler_flags()

   # Get the package/subdirectory name:
   atlas_get_package_name( pkgName )

   # Get the package directory:
   atlas_get_package_dir( pkgDir )

   # If the user gave source files, let's build a test executable:
   if( ARG_SOURCES )

      # Allow wildcards in the source names:
      file( GLOB _sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
         ${ARG_SOURCES} )
      if( NOT _sources )
         message( WARNING "No sources available for test ${testName}" )
         return()
      endif()

      # Put the files into source groups. So they would show up in a ~reasonable
      # way in an IDE like Xcode:
      atlas_group_source_files( ${_sources} )

      # Make sure that this target doesn't exist yet:
      if( TARGET ${pkgName}_${testName} )
         message( WARNING "Target with name ${pkgName}_${testName} already "
            "exists. Can't set up test." )
         return()
      endif()

      # Declare the executable:
      add_executable( ${pkgName}_${testName} ${_sources} )

      # Set it's properties:
      add_dependencies( ${pkgName}_${testName} ${pkgName}Pkg )
      add_dependencies( Package_${pkgName} ${pkgName}_${testName} )
      set_property( TARGET ${pkgName}_${testName} PROPERTY LABELS ${pkgName} )
      set_property( TARGET ${pkgName}_${testName} PROPERTY FOLDER ${pkgDir} )
      set_property( TARGET ${pkgName}_${testName} PROPERTY
         OUTPUT_NAME "${testName}.exe" )
      set_property( TARGET ${pkgName}_${testName} PROPERTY
         RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test-bin" )
      foreach( _config DEBUG RELEASE MINSIZEREL RELWITHDEBINFO )
         set_property( TARGET ${pkgName}_${testName} PROPERTY
            RUNTIME_OUTPUT_DIRECTORY_${_config}
            "${CMAKE_CURRENT_BINARY_DIR}/test-bin" )
      endforeach()
      target_include_directories( ${pkgName}_${testName} PRIVATE
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
         $<TARGET_PROPERTY:${pkgName}Pkg,INTERFACE_INCLUDE_DIRECTORIES>
         $<TARGET_PROPERTY:${pkgName}PkgPrivate,INTERFACE_INCLUDE_DIRECTORIES>
         ${ARG_INCLUDE_DIRS} )
      target_link_libraries( ${pkgName}_${testName}
         ${ARG_LINK_LIBRARIES} )

   elseif( ARG_SCRIPT )

      # Create the test script installation target of the package if it
      # doesn't exist yet:
      if( NOT TARGET ${pkgName}TestScriptInstall )
         add_custom_target( ${pkgName}TestScriptInstall ALL SOURCES
            $<TARGET_PROPERTY:${pkgName}TestScriptInstall,TEST_SCRIPTS> )
         add_dependencies( Package_${pkgName} ${pkgName}TestScriptInstall )
         set_property( TARGET ${pkgName}TestScriptInstall
            PROPERTY LABELS ${pkgName} )
         set_property( TARGET ${pkgName}TestScriptInstall
            PROPERTY FOLDER ${pkgDir} )
      endif()

      # The script file found should be executable itself.
      # We just make a soft link to it in the build directory.
      if( IS_ABSOLUTE ${ARG_SCRIPT} )
         set( _relPath ${ARG_SCRIPT} )
      else()
         file( RELATIVE_PATH _relPath ${CMAKE_CURRENT_BINARY_DIR}/test-bin
            ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_SCRIPT} )
      endif()
      add_custom_command( OUTPUT
         ${CMAKE_CURRENT_BINARY_DIR}/test-bin/${testName}.exe
         COMMAND ${CMAKE_COMMAND} -E make_directory
         ${CMAKE_CURRENT_BINARY_DIR}/test-bin
         COMMAND ${CMAKE_COMMAND} -E create_symlink ${_relPath}
         ${CMAKE_CURRENT_BINARY_DIR}/test-bin/${testName}.exe )
      set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
         ${CMAKE_CURRENT_BINARY_DIR}/test-bin/${testName}.exe )
      # Remember to set up this link:
      set_property( TARGET ${pkgName}TestScriptInstall APPEND PROPERTY
         TEST_SCRIPTS ${CMAKE_CURRENT_BINARY_DIR}/test-bin/${testName}.exe )

   else()
      message( FATAL_ERROR "Internal logic error detected!" )
   endif()

   # Create the directory in which the unit tests will be executed from:
   file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun )

   # Set up the variables for the unit test executor script:
   if( ARG_PRE_EXEC_SCRIPT )
      set( PRE_EXEC_SCRIPT ${ARG_PRE_EXEC_SCRIPT} )
   else()
      set( PRE_EXEC_SCRIPT "# No pre-exec necessary" )
   endif()
   if( ARG_POST_EXEC_SCRIPT )
      set( POST_EXEC_SCRIPT ${ARG_POST_EXEC_SCRIPT} )
   else()
      set( POST_EXEC_SCRIPT "if type post.sh >/dev/null 2>&1; then
    post.sh ${testName} \"${ARG_EXTRA_PATTERNS}\"
fi" )
   endif()

   # Generate a test script that will run this unit test in the
   # correct environment:
   find_file( _utSkeleton unit_test_executor.sh.in
      PATH_SUFFIXES scripts PATHS ${CMAKE_MODULE_PATH} )
   configure_file( ${_utSkeleton}
      ${CMAKE_CURRENT_BINARY_DIR}/${testName}.sh @ONLY )
   mark_as_advanced( _utSkeleton )
   unset( _utSkeleton )

   # And run this script as the unit test:
   add_test( NAME ${pkgName}_${testName}_ctest
      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${testName}.sh
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun )

   # Set its properties:
   set_tests_properties( ${pkgName}_${testName}_ctest PROPERTIES
      TIMEOUT 120
      LABELS ${pkgName} )
   if( ARG_ENVIRONMENT )
      set_property( TEST ${pkgName}_${testName}_ctest PROPERTY
         ENVIRONMENT "${ARG_ENVIRONMENT}" )
   endif()
   if( ARG_PROPERTIES )
      set_tests_properties( ${pkgName}_${testName}_ctest PROPERTIES
         ${ARG_PROPERTIES} )
   endif()

endfunction( atlas_add_test )

# This function is used in the ATLAS projects instead of CMake's built-in
# ExternalProject_Add function, to make some tweaks to how external projects
# get built exactly.
#
# The function receives an extra VERSION parameter on top of all the arguments
# that can be given to ExternalProject_Add itself. The value of this parameter
# is substituted into all "%VERSION%" values in the parameter list.
#
function( AtlasExternalProject_Add name )

   # Pick up the VERSION argument
   cmake_parse_arguments( ARG "" "VERSION" "" ${ARGN} )

   # Massage the arguments that we will pass to the ExternalProject_Add
   # function:
   set( _arguments )
   foreach( _arg ${ARG_UNPARSED_ARGUMENTS} )
      string( REPLACE "%VERSION%" ${ARG_VERSION} _toAdd ${_arg} )
      list( APPEND _arguments ${_toAdd} )
   endforeach()

   # Now call the built-in function with the right arguments:
   ExternalProject_Add( ${name} ${_arguments} )

endfunction()

# Macro for turning off the -Wl,--as-needed linker flag for the current
# package. Needed a couple of packages.
#
# Usage: atlas_disable_as_needed()
#
macro( atlas_disable_as_needed )

   # Remove this flag from all of the linker flag variables:
   string( REPLACE "-Wl,--as-needed" "" CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS}" )
   string( REPLACE "-Wl,--as-needed" "" CMAKE_SHARED_LINKER_FLAGS
      "${CMAKE_SHARED_LINKER_FLAGS}" )
   string( REPLACE "-Wl,--as-needed" "" CMAKE_MODULE_LINKER_FLAGS
      "${CMAKE_MODULE_LINKER_FLAGS}" )

endmacro( atlas_disable_as_needed )
