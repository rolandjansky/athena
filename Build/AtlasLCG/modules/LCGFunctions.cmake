# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: LCGFunctions.cmake 734807 2016-04-08 10:32:33Z krasznaa $
#
# Some helper functions, used by the Find<Bla>.cmake modules of the package.
#

# CMake built-in module(s):
include( CMakeParseArguments )

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
# Usage: lcg_external_module( NAME name
#                             [INCLUDE_SUFFIXES include1 include2...]
#                             [INCLUDE_NAMES header1 header2...]
#                             [DEFAULT_COMPONENTS component1 component2...]
#                             [COMPULSORY_COMPONENTS component3 component4...]
#                             [LIBRARY_SUFFIXES libDir1 libDir2...]
#                             [LIBRARY_PREFIX prefix_]
#                             [LIBRARY_POSTFIX _postfix]
#                             [EXTRA_OPTIONS cmakeOpt1...]
#                             [SEARCH_PATHS path1 path2...] )
#
function( lcg_external_module )

   # Parse the arguments:
   set( _singleArgParams NAME LIBRARY_PREFIX LIBRARY_POSTFIX )
   set( _multiArgParams INCLUDE_SUFFIXES INCLUDE_NAMES DEFAULT_COMPONENTS
      COMPULSORY_COMPONENTS LIBRARY_SUFFIXES EXTRA_OPTIONS SEARCH_PATHS )
   cmake_parse_arguments( ARG "" "${_singleArgParams}" "${_multiArgParams}"
      ${ARGN} )

   # Create an uppercase version of the name:
   string( TOUPPER ${ARG_NAME} nameUpper )

   # If the external was already found, set the module to silent mode.
   if( ${ARG_NAME}_FOUND OR ${nameUpper}_FOUND )
      set( ${ARG_NAME}_FIND_QUIETLY TRUE )
   endif()

   # Find all the externals that this external depends on:
#   if( ${nameUpper}_DEPENDS )
#      foreach( _dependency ${${nameUpper}_DEPENDS} )
#         if( ${ARG_NAME}_FIND_QUIETLY )
#            set( ${_dependency}_FIND_QUIETLY TRUE )
#         endif()
#         find_package( ${_dependency} )
#      endforeach()
#   endif()

   # Find the include directory of the external:
   if( ARG_INCLUDE_NAMES )
      find_path( ${nameUpper}_INCLUDE_DIR NAMES ${ARG_INCLUDE_NAMES}
         PATH_SUFFIXES ${ARG_INCLUDE_SUFFIXES}
         PATHS ${${nameUpper}_ROOT} ${ARG_SEARCH_PATHS}
         ${ARG_EXTRA_OPTIONS} )
      set( _relocatableDir ${${nameUpper}_INCLUDE_DIR} )
      _lcg_make_paths_relocatable( _relocatableDir )
      if( GAUDI_ATLAS )
         set( ${nameUpper}_INCLUDE_DIRS ${${nameUpper}_INCLUDE_DIR} )
         set( ${nameUpper}_INCLUDE_DIRS ${${nameUpper}_INCLUDE_DIR}
            PARENT_SCOPE )
      else()
         set( ${nameUpper}_INCLUDE_DIRS
            $<BUILD_INTERFACE:${${nameUpper}_INCLUDE_DIR}>
            $<INSTALL_INTERFACE:${_relocatableDir}> )
         set( ${nameUpper}_INCLUDE_DIRS
            $<BUILD_INTERFACE:${${nameUpper}_INCLUDE_DIR}>
            $<INSTALL_INTERFACE:${_relocatableDir}> PARENT_SCOPE )
      endif()
      unset( _relocatableDir )
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
         find_library( _${nameUpper}_${component}_library
            NAMES ${ARG_LIBRARY_PREFIX}${component}${ARG_LIBRARY_POSTFIX}
            PATH_SUFFIXES ${ARG_LIBRARY_SUFFIXES}
            PATHS ${${nameUpper}_ROOT} ${ARG_SEARCH_PATHS}
            ${ARG_EXTRA_OPTIONS} )
         mark_as_advanced( _${nameUpper}_${component}_library )
         # Deal with the search result:
         if( _${nameUpper}_${component}_library )
            set( ${nameUpper}_${component}_FOUND TRUE )
            set( ${nameUpper}_${component}_FOUND TRUE PARENT_SCOPE )
            set( _relocatableLib ${_${nameUpper}_${component}_library} )
            _lcg_make_paths_relocatable( _relocatableLib )
            if( GAUDI_ATLAS )
               set( ${nameUpper}_${component}_LIBRARY
                  ${_${nameUpper}_${component}_library} )
               set( ${nameUpper}_${component}_LIBRARY
                  ${_${nameUpper}_${component}_library} PARENT_SCOPE )
            else()
               set( ${nameUpper}_${component}_LIBRARY
                  $<BUILD_INTERFACE:${_${nameUpper}_${component}_library}>
                  $<INSTALL_INTERFACE:${_relocatableLib}> )
               set( ${nameUpper}_${component}_LIBRARY
                  $<BUILD_INTERFACE:${_${nameUpper}_${component}_library}>
                  $<INSTALL_INTERFACE:${_relocatableLib}> PARENT_SCOPE )
            endif()
            list( APPEND ${nameUpper}_FOUND_COMPONENTS ${component} )
            list( APPEND ${nameUpper}_LIBRARIES
               ${${nameUpper}_${component}_LIBRARY} )
            get_filename_component( _libdir
               ${_${nameUpper}_${component}_library} PATH )
            list( APPEND ${nameUpper}_LIBRARY_DIRS ${_libdir} )
            unset( _relocatableLib )
            unset( _libdir )
         else()
            set( ${nameUpper}_${component}_FOUND FALSE )
            set( ${nameUpper}_${component}_FOUND FALSE PARENT_SCOPE )
         endif()
         unset( _${nameUpper}_{component}_library )
         # Mark the component location as an advanced property:
         mark_as_advanced( ${nameUpper}_${component}_LIBRARY
            ${nameUpper}_${component}_FOUND )
      endforeach()
      # Remove the duplicate library directory entries:
      if( ${nameUpper}_LIBRARY_DIRS )
         list( REMOVE_DUPLICATES ${nameUpper}_LIBRARY_DIRS )
      endif()
   endif()

   # Return all the useful variables:
   set( ${nameUpper}_FOUND_COMPONENTS ${${nameUpper}_FOUND_COMPONENTS}
      PARENT_SCOPE )
   set( ${nameUpper}_LIBRARIES ${${nameUpper}_LIBRARIES} PARENT_SCOPE )
   set( ${nameUpper}_LIBRARY_DIRS ${${nameUpper}_LIBRARY_DIRS} PARENT_SCOPE )

endfunction( lcg_external_module )

# Helper function to set up a projects dependence on LCG RPM packages
#
# This function needs to be used in the Find<Bla>.cmake modules to set up the
# current project's dependence on LCG's RPM packages.
#
# Usage: lcg_need_rpm( Python
#                      [FOUND_NAME PYTHONLIBS]
#                      [VERSION_NAME PYTHON] )
#
function( lcg_need_rpm name )

   # Add the optional argument(s):
   cmake_parse_arguments( ARG "" "FOUND_NAME;VERSION_NAME" "" ${ARGN} )

   # Create the "found name" of the package:
   if( ARG_FOUND_NAME )
      set( nameFound ${ARG_FOUND_NAME} )
   else()
      string( TOUPPER ${name} nameFound )
   endif()

   # If the package was not found, return now:
   if( NOT ${nameFound}_FOUND )
      return()
   endif()

   # Create a "version name" for the package:
   if( ARG_VERSION_NAME )
      set( nameVersion ${ARG_VERSION_NAME} )
   else()
      set( nameVersion ${nameFound} )
   endif()

   # Construct the package's version. The trick here is that LCG replaces
   # "-" characters with "_" ones in the version names.
   string( REPLACE "-" "_" version "${${nameVersion}_VERSION}" )

   # The same applies for the platform name. SFT for some reason likes to
   # use underscores in that too...
   string( REPLACE "-" "_" platform "${LCG_PLATFORM}" )

   # Construct the RPM package name for this dependency:
   set( _rpmName
      "LCG_${LCG_VERSION}_${name}_${version}_${platform}" )

   # Add it to the global list of dependencies:
   set_property( GLOBAL APPEND PROPERTY ATLAS_EXTERNAL_RPMS ${_rpmName} )

endfunction( lcg_need_rpm )

# Helper function used internally to make paths easier to relocate
#
# This function is used internally to put in ${LCG_RELEASE_BASE} into path
# lists, so that the generated environment setup files would be easier to
# adjust to a relocation.
#
# Usage: _lcg_make_paths_relocatable( _python_paths )
#
function( _lcg_make_paths_relocatable pathList )

   # Create the result list:
   set( _result )

   # Loop over the received list, and do the replacements:
   foreach( _path ${${pathList}} )
      string( REPLACE "${LCG_RELEASE_BASE}" "\${LCG_RELEASE_BASE}"
         _relocatablePath ${_path} )
      list( APPEND _result ${_relocatablePath} )
   endforeach()

   # Return the list to the caller:
   set( ${pathList} ${_result} PARENT_SCOPE )

endfunction( _lcg_make_paths_relocatable )

# Helper function used internally to make replacements in paths
#
# In order to be able to generate setup files that would refer to ATLAS
# projects with valid relative paths after installation on AFS, this function
# is used to massage path names according to replacement rules specified
# by the user.
#
# Usage: _lcg_replace_paths( _python_paths REPLACE_LIST )
#
function( _lcg_replace_paths pathList replaceList )

   # Create the result list:
   set( _result )

   # Loop over the environment list:
   foreach( _path ${${pathList}} )

      # Loop over the replacement list:
      set( _replaceListCopy ${${replaceList}} )
      while( _replaceListCopy )

         # Extract the original and replacement strings:
         list( GET _replaceListCopy 0 _original )
         list( GET _replaceListCopy 1 _replacement )
         list( REMOVE_AT _replaceListCopy 0 1 )

         # In case we picked up something funky:
         if( "${_original}" STREQUAL "" )
            continue()
         endif()

         # If this is a valid replacement, then do so. Do not stop
         # though, as multiple replacements may need to be made in the
         # path.
         string( REPLACE "${_original}" "${_replacement}" _path ${_path} )
      endwhile()

      # Add the (possibly modified) path to the output:
      list( APPEND _result ${_path} )
   endforeach()

   # Return the list to the caller:
   set( ${pathList} ${_result} PARENT_SCOPE )

endfunction( _lcg_replace_paths )

# Helper function to evaluate if an environment setting points to some path
#
# The environment setup script tries to be smart with re-using previously
# configured environment variables in the setup of higher level environment
# variables. To do this correctly, we need to know which variables describe
# paths, and which describe something else. (Like a DB connection setting.)
#
# This function is used to select environment variables that should be
# considered to be re-used in the setup of other environment variables.
#
# Usage: _lcg_is_absolute_path( ${_name} _enviromentsCopy value )
#
function( _lcg_is_absolute_path varName environmentsListName returnValueName )

   # The default answer is no...
   set( ${returnValueName} FALSE )

   # Make a copy of the environment list:
   set( _envCopy ${${environmentsListName}} )

   # Loop over its elements:
   while( _envCopy )
      # Get the instruction and the mandatory name:
      list( GET _envCopy 0 _instruction )
      list( GET _envCopy 1 _name )
      # Handle the UNSET command in a special way:
      if( "${_instruction}" STREQUAL "UNSET" )
         list( REMOVE_AT _envCopy 0 1 )
      elseif( "${_instruction}" STREQUAL "SET" OR
            "${_instruction}" STREQUAL "FORCESET" )
         list( GET _envCopy 2 _value )
         # Check if we found the variable we're looking for:
         if( "${varName}" STREQUAL "${_name}" )
            # Check if it sets a path or not:
            if( IS_DIRECTORY "${_value}" AND
                  IS_ABSOLUTE "${_value}" )
               set( ${returnValueName} TRUE PARENT_SCOPE )
            else()
               set( ${returnValueName} FALSE PARENT_SCOPE )
            endif()
            return()
         endif()
         list( REMOVE_AT _envCopy 0 1 2 )
      else()
         list( REMOVE_AT _envCopy 0 1 2 )
      endif()
   endwhile()

endfunction( _lcg_is_absolute_path )

# Function creating environment setup files based on the found externals
#
# This function can be called at the end of configuring the build of a project,
# to produce configuration files of different types.
#
# Usage: lcg_generate_env( [SH_FILE ${CMAKE_BINARY_DIR}/setup.sh]
#                          [CSH_FILE ${CMAKE_BINARY_DIR}/setup.csh]
#                          [XENV_FILE ${CMAKE_BINARY_DIR}/setup.xenv]
#                          [REPLACE "orig1" "new1" "orig2" "new2"...] )
#
function( lcg_generate_env )

   # Parse the arguments:
   cmake_parse_arguments( ARG "" "SH_FILE;CSH_FILE;XENV_FILE"
      "REPLACE" ${ARGN} )

   # Tell the user what's happening:
   message( STATUS "Generating external environment configuration" )

   # Environment variables handled by this function:
   set( _python_paths )
   set( _binary_paths )
   set( _library_paths )
   set( _include_paths )
   set( _environments )

   # Get a list of all the packages that were found using find_package(...).
   # This is a (well hidden) CMake feature:
   get_property( _packages_found GLOBAL PROPERTY PACKAGES_FOUND )

   # Loop over these packages:
   foreach( _package ${_packages_found} )

      # Skip this package itself:
      if( "${_package}" STREQUAL "LCG" )
         continue()
      endif()

      # Also skip all the ATLAS offline and analysis packages:
      set( _atlasPackages AtlasExternals DetCommon AtlasCore AtlasConditions
         AtlasEvent AtlasReconstruction AtlasTrigger AtlasAnalysis
         AtlasSimulation AtlasOffline AtlasHLT
         AtlasProduction AtlasDerivation AtlasP1HLT AtlasPhysics AtlasCAFHLT
         AtlasP1MON AtlasProd1 AtlasProd2 AtlasTier0 AtlasTestHLT TrigMC
         AthAnalysisBase AthSimulationBase AnalysisBase )
      list( FIND _atlasPackages ${_package} _atlasPackageIndex )
      unset( _atlasPackages )
      if( NOT "${_atlasPackageIndex}" EQUAL -1 )
         continue()
      endif()
      unset( _atlasPackageIndex )

      # The package name in upper-case:
      string( TOUPPER ${_package} _packageUpper )

      # Find the package again, to pull in its properties:
      find_package( ${_package} QUIET )

      # Get the paths of the package:
      list( APPEND _python_paths ${${_package}_PYTHON_PATH} )
      list( APPEND _binary_paths ${${_package}_BINARY_PATH} )
      list( APPEND _library_paths ${${_package}_LIBRARY_DIRS} )
      list( APPEND _include_paths ${${_package}_INCLUDE_DIRS} )
      list( APPEND _environments ${${_package}_ENVIRONMENT} )
      if( NOT _package STREQUAL _packageUpper )
         list( APPEND _python_paths ${${_packageUpper}_PYTHON_PATH} )
         list( APPEND _binary_paths ${${_packageUpper}_BINARY_PATH} )
         list( APPEND _library_paths ${${_packageUpper}_LIBRARY_DIRS} )
         list( APPEND _include_paths ${${_packageUpper}_INCLUDE_DIRS} )
         list( APPEND _environments ${${_packageUpper}_ENVIRONMENT} )
      endif()

   endforeach()

   # Remove the duplicates from the paths:
   foreach( _path _python_paths _binary_paths _library_paths _include_paths )
      if( NOT "${${_path}}" STREQUAL "" )
         list( REMOVE_DUPLICATES ${_path} )
      endif()
   endforeach()

   # Remove system paths from these:
   set( _library_paths_copy ${_library_paths} )
   set( _library_paths )
   foreach( l ${_library_paths_copy} )
      if( NOT l MATCHES
            "^($<BUILD_INTERFACE:|$<INSTALL_INTERFACE:)?(/usr|/usr/local)?/lib(32|64)?" )
         set( _library_paths ${_library_paths} ${l} )
      endif()
   endforeach()
   unset( _library_paths_copy )

   set( _binary_paths_copy ${_binary_paths} )
   set( _binary_paths )
   foreach( b ${_binary_paths_copy} )
      if( NOT b MATCHES
            "^($<BUILD_INTERFACE:|$<INSTALL_INTERFACE:)?(/usr)?/bin" )
         set( _binary_paths ${_binary_paths} ${b} )
      endif()
   endforeach()
   unset( _binary_paths_copy )

   # Clean out the include paths variable:
   if( _include_paths )
      list( REMOVE_ITEM _include_paths /usr/include )
   endif()

   # Make a copy of the environments, which is used later on to evaluate
   # further replacements in the paths:
   set( _environmentsOrig "${_environments}" )

   # Use LCG_RELEASE_BASE in the paths if possible:
   if( LCG_RELEASE_BASE )
      _lcg_make_paths_relocatable( _python_paths )
      _lcg_make_paths_relocatable( _binary_paths )
      _lcg_make_paths_relocatable( _library_paths )
      _lcg_make_paths_relocatable( _include_paths )
      _lcg_make_paths_relocatable( _environments )
   endif()

   # Make replacements in the paths if necessary:
   if( ARG_REPLACE )
      _lcg_replace_paths( _python_paths ARG_REPLACE )
      _lcg_replace_paths( _binary_paths ARG_REPLACE )
      _lcg_replace_paths( _library_paths ARG_REPLACE )
      _lcg_replace_paths( _include_paths ARG_REPLACE )
      _lcg_replace_paths( _environments ARG_REPLACE )
   endif()

   # Write an SH file:
   if( ARG_SH_FILE )

      # Start the file:
      message( STATUS "Writing runtime environment to file: ${ARG_SH_FILE}" )
      file( WRITE ${ARG_SH_FILE}.in "# Generated by lcg_generate_env...\n" )

      # Set LCG_RELEASE_BASE if it's available:
      if( LCG_RELEASE_BASE )
         file( APPEND ${ARG_SH_FILE}.in
            "if [ -z \"\${LCG_RELEASE_BASE}\" ]; then\n"
            "   export LCG_RELEASE_BASE=${LCG_RELEASE_BASE}\n"
            "fi\n" )
      endif()
      # Set LCG_PLATFORM if it's available:
      if( LCG_PLATFORM )
         file( APPEND ${ARG_SH_FILE}.in
            "if [ -z \"\${LCG_PLATFORM}\" ]; then\n"
            "   export LCG_PLATFORM=${LCG_PLATFORM}\n"
            "fi\n" )
      endif()

      # List of configured environment variables:
      set( _envVarNamesAndValues )

      # Set the general environment variables:
      set( _envCopy ${_environments} )
      while( _envCopy )
         # Get the instruction and the mandatory name:
         list( GET _envCopy 0 _instruction )
         list( GET _envCopy 1 _name )
         # Now decide what to do with this instruction:
         if( "${_instruction}" STREQUAL "UNSET" )
            file( APPEND ${ARG_SH_FILE}.in
               "unset ${_name}\n" )
            list( REMOVE_AT _envCopy 0 1 )
         elseif( "${_instruction}" STREQUAL "SET" )
            list( GET _envCopy 2 _value )
            _lcg_replace_paths( _value _envVarNamesAndValues )
            file( APPEND ${ARG_SH_FILE}.in
               "if [ -z \"\${${_name}}\" ]; then\n"
               "   export ${_name}=${_value}\n"
               "fi\n" )
            list( REMOVE_AT _envCopy 0 1 2 )
            _lcg_is_absolute_path( ${_name} _environmentsOrig _isPath )
            if( ${_isPath} )
               list( APPEND _envVarNamesAndValues "${_value}" "\${${_name}}" )
            endif()
         elseif( "${_instruction}" STREQUAL "FORCESET" )
            list( GET _envCopy 2 _value )
            _lcg_replace_paths( _value _envVarNamesAndValues )
            file( APPEND ${ARG_SH_FILE}.in
               "export ${_name}=${_value}\n" )
            list( REMOVE_AT _envCopy 0 1 2 )
            _lcg_is_absolute_path( ${_name} _environmentsOrig _isPath )
            if( ${_isPath} )
               list( APPEND _envVarNamesAndValues "${_value}" "\${${_name}}" )
            endif()
         elseif( "${_instruction}" STREQUAL "APPEND" )
            list( GET _envCopy 2 _value )
            _lcg_replace_paths( _value _envVarNamesAndValues )
            file( APPEND ${ARG_SH_FILE}.in
               "if [ -z \"\${${_name}}\" ]; then\n"
               "   export ${_name}=${_value}\n"
               "else\n"
               "   export ${_name}=\${${_name}}:${_value}\n"
               "fi\n" )
            list( REMOVE_AT _envCopy 0 1 2 )
         elseif( "${_instruction}" STREQUAL "PREPEND" )
            list( GET _envCopy 2 _value )
            _lcg_replace_paths( _value _envVarNamesAndValues )
            file( APPEND ${ARG_SH_FILE}.in
               "if [ -z \"\${${_name}}\" ]; then\n"
               "   export ${_name}=${_value}\n"
               "else\n"
               "   export ${_name}=${_value}:\${${_name}}\n"
               "fi\n" )
            list( REMOVE_AT _envCopy 0 1 2 )
         else()
            # Don't try to continue with this if we hit an issue:
            message( WARNING "Instruction ${_instruction} not known" )
            break()
         endif()
      endwhile()

      # Make another round of replacements in the paths:
      _lcg_replace_paths( _python_paths _envVarNamesAndValues )
      _lcg_replace_paths( _binary_paths _envVarNamesAndValues )
      _lcg_replace_paths( _library_paths _envVarNamesAndValues )
      _lcg_replace_paths( _include_paths _envVarNamesAndValues )

      # Clean up:
      unset( _envVarNamesAndValues )

      # Make the lists into lists understood by bash:
      string( REPLACE ";" ":" _python_paths_written  "${_python_paths}" )
      string( REPLACE ";" ":" _binary_paths_written  "${_binary_paths}" )
      string( REPLACE ";" ":" _library_paths_written "${_library_paths}" )
      string( REPLACE ";" ":" _include_paths_written "${_include_paths}" )

      # Set the various paths:
      if( NOT "${_python_paths_written}" STREQUAL "" )
         file( APPEND ${ARG_SH_FILE}.in
            "if [ -z \"\${PYTHONPATH}\" ]; then\n"
            "   export PYTHONPATH=${_python_paths_written}\n"
            "else\n"
            "   export PYTHONPATH=${_python_paths_written}:\${PYTHONPATH}\n"
            "fi\n" )
      endif()
      if( NOT "${_binary_paths_written}" STREQUAL "" )
         file( APPEND ${ARG_SH_FILE}.in
            "if [ -z \"\${PATH}\" ]; then\n"
            "   export PATH=${_binary_paths_written}\n"
            "else\n"
            "   export PATH=${_binary_paths_written}:\${PATH}\n"
            "fi\n" )
      endif()
      if( NOT "${_library_paths_written}" STREQUAL "" )
         file( APPEND ${ARG_SH_FILE}.in
            "if [ -z \"\${LD_LIBRARY_PATH}\" ]; then\n"
            "   export LD_LIBRARY_PATH=${_library_paths_written}\n"
            "else\n"
            "   export LD_LIBRARY_PATH=${_library_paths_written}:"
            "\${LD_LIBRARY_PATH}\n"
            "fi\n" )
      endif()
      if( APPLE AND NOT "${_library_paths_written}" STREQUAL "" )
         file( APPEND ${ARG_SH_FILE}.in
            "if [ -z \"\${DYLD_LIBRARY_PATH}\" ]; then\n"
            "   export DYLD_LIBRARY_PATH=${_library_paths_written}\n"
            "else\n"
            "   export DYLD_LIBRARY_PATH=${_library_paths_written}:"
            "\${DYLD_LIBRARY_PATH}\n"
            "fi\n" )
      endif()
      if( NOT "${_include_paths_written}" STREQUAL "" )
         file( APPEND ${ARG_SH_FILE}.in
            "if [ -z \"\${ROOT_INCLUDE_PATH}\" ]; then\n"
            "   export ROOT_INCLUDE_PATH=${_include_paths_written}\n"
            "else\n"
            "   export ROOT_INCLUDE_PATH=${_include_paths_written}:"
            "\${ROOT_INCLUDE_PATH}\n"
            "fi\n" )
      endif()

      # Generate the proper file out of the created skeleton at the
      # generation step. (To resolve possible generator expressions
      # in the path names.)
      file( GENERATE OUTPUT ${ARG_SH_FILE} INPUT ${ARG_SH_FILE}.in )

   endif()

   # Write a CSH file:
   if( ARG_CSH_FILE )

      # Start the file:
      message( STATUS "Writing runtime environment to file: ${ARG_CSH_FILE}" )
      file( WRITE ${ARG_CSH_FILE}.in "# Generated by lcg_generate_env...\n" )

      # Set LCG_RELEASE_BASE if it's available:
      if( LCG_RELEASE_BASE )
         file( APPEND ${ARG_CSH_FILE}.in
            "if (! \$?LCG_RELEASE_BASE ) then\n"
            "   setenv LCG_RELEASE_BASE ${LCG_RELEASE_BASE}\n"
            "endif\n" )
      endif()
      # Set LCG_PLATFORM if it's available:
      if( LCG_PLATFORM )
         file( APPEND ${ARG_CSH_FILE}.in
            "if (! \$?LCG_PLATFORM ) then\n"
            "   setenv LCG_PLATFORM ${LCG_PLATFORM}\n"
            "endif\n" )
      endif()

      # List of configured environment variables:
      set( _envVarNamesAndValues )

      # Set the general environment variables:
      set( _envCopy ${_environments} )
      while( _envCopy )
         # Get the instruction and the mandatory name:
         list( GET _envCopy 0 _instruction )
         list( GET _envCopy 1 _name )
         # Now decide what to do with this instruction:
         if( "${_instruction}" STREQUAL "UNSET" )
            file( APPEND ${ARG_CSH_FILE}.in
               "unset ${_name}\n" )
            list( REMOVE_AT _envCopy 0 1 )
         elseif( "${_instruction}" STREQUAL "SET" )
            list( GET _envCopy 2 _value )
            _lcg_replace_paths( _value _envVarNamesAndValues )
            file( APPEND ${ARG_CSH_FILE}.in
               "if (! \$?${_name} ) then\n"
               "   setenv ${_name} ${_value}\n"
               "endif\n" )
            list( REMOVE_AT _envCopy 0 1 2 )
            _lcg_is_absolute_path( ${_name} _environmentsOrig _isPath )
            if( ${_isPath} )
               list( APPEND _envVarNamesAndValues "${_value}" "\${${_name}}" )
            endif()
         elseif( "${_instruction}" STREQUAL "FORCESET" )
            list( GET _envCopy 2 _value )
            _lcg_replace_paths( _value _envVarNamesAndValues )
            file( APPEND ${ARG_CSH_FILE}.in
               "setenv ${_name} ${_value}\n" )
            list( REMOVE_AT _envCopy 0 1 2 )
            _lcg_is_absolute_path( ${_name} _environmentsOrig _isPath )
            if( ${_isPath} )
               list( APPEND _envVarNamesAndValues "${_value}" "\${${_name}}" )
            endif()
         elseif( "${_instruction}" STREQUAL "APPEND" )
            list( GET _envCopy 2 _value )
            _lcg_replace_paths( _value _envVarNamesAndValues )
            file( APPEND ${ARG_CSH_FILE}.in
               "if (! \$?${_name} ) then\n"
               "   setenv ${_name} ${_value}\n"
               "else\n"
               "   setenv ${_name} \$${_name}:${_value}\n"
               "endif\n" )
            list( REMOVE_AT _envCopy 0 1 2 )
         elseif( "${_instruction}" STREQUAL "PREPEND" )
            list( GET _envCopy 2 _value )
            _lcg_replace_paths( _value _envVarNamesAndValues )
            file( APPEND ${ARG_CSH_FILE}.in
               "if (! \$?${_name} ) then\n"
               "   setenv ${_name} ${_value}\n"
               "else\n"
               "   setenv ${_name} ${_value}:\$${_name}\n"
               "endif\n" )
            list( REMOVE_AT _envCopy 0 1 2 )
         else()
            # Don't try to continue with this if we hit an issue:
            message( WARNING "Instruction ${_instruction} not known" )
            break()
         endif()
      endwhile()

      # Make another round of replacements in the paths:
      _lcg_replace_paths( _python_paths _envVarNamesAndValues )
      _lcg_replace_paths( _binary_paths _envVarNamesAndValues )
      _lcg_replace_paths( _library_paths _envVarNamesAndValues )
      _lcg_replace_paths( _include_paths _envVarNamesAndValues )

      # Make the lists into lists understood by tcsh:
      string( REPLACE ";" ":" _python_paths_written  "${_python_paths}" )
      string( REPLACE ";" ":" _binary_paths_written  "${_binary_paths}" )
      string( REPLACE ";" ":" _library_paths_written "${_library_paths}" )
      string( REPLACE ";" ":" _include_paths_written "${_include_paths}" )

      # Set the various paths:
      if( NOT "${_python_paths_written}" STREQUAL "" )
         file( APPEND ${ARG_CSH_FILE}.in
            "if (! \$?PYTHONPATH ) then\n"
            "   setenv PYTHONPATH ${_python_paths_written}\n"
            "else\n"
            "   setenv PYTHONPATH ${_python_paths_written}:\$PYTHONPATH\n"
            "endif\n" )
      endif()
      if( NOT "${_binary_paths_written}" STREQUAL "" )
         file( APPEND ${ARG_CSH_FILE}.in
            "if (! \$?PATH ) then\n"
            "   setenv PATH ${_binary_paths_written}\n"
            "else\n"
            "   setenv PATH ${_binary_paths_written}:\$PATH\n"
            "endif\n" )
      endif()
      if( NOT "${_library_paths_written}" STREQUAL "" )
         file( APPEND ${ARG_CSH_FILE}.in
            "if (! \$?LD_LIBRARY_PATH ) then\n"
            "   setenv LD_LIBRARY_PATH ${_library_paths_written}\n"
            "else\n"
            "   setenv LD_LIBRARY_PATH ${_library_paths_written}:"
            "\$LD_LIBRARY_PATH\n"
            "endif\n" )
      endif()
      if( APPLE AND NOT "${_library_paths_written}" STREQUAL "" )
         file( APPEND ${ARG_CSH_FILE}.in
            "if (! \$?DYLD_LIBRARY_PATH ) then\n"
            "   setenv DYLD_LIBRARY_PATH ${_library_paths_written}\n"
            "else\n"
            "   setenv DYLD_LIBRARY_PATH ${_library_paths_written}:"
            "\$DYLD_LIBRARY_PATH\n"
            "endif\n" )
      endif()
      if( NOT "${_include_paths_written}" STREQUAL "" )
         file( APPEND ${ARG_CSH_FILE}.in
            "if (! \$?ROOT_INCLUDE_PATH ) then\n"
            "   setenv ROOT_INCLUDE_PATH ${_include_paths_written}\n"
            "else\n"
            "   setenv ROOT_INCLUDE_PATH ${_include_paths_written}:"
            "\$ROOT_INCLUDE_PATH\n"
            "endif\n" )
      endif()

      # Generate the proper file out of the created skeleton at the
      # generation step. (To resolve possible generator expressions
      # in the path names.)
      file( GENERATE OUTPUT ${ARG_CSH_FILE} INPUT ${ARG_CSH_FILE}.in )

   endif()

   # Write an XENV file:
   if( ARG_XENV_FILE )

      # Tell the user what's happening:
      message( STATUS "Writing runtime environment to file: ${ARG_XENV_FILE}" )

      # Write the file's header:
      file( WRITE ${ARG_XENV_FILE}.in
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<env:config xmlns:env=\"EnvSchema\" "
         "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
         "xsi:schemaLocation=\"EnvSchema EnvSchema.xsd\">\n" )

      # Set LCG_RELEASE_BASE if it's available:
      if( LCG_RELEASE_BASE )
         file( APPEND ${ARG_XENV_FILE}.in
            "   <env:default variable=\"LCG_RELEASE_BASE\">"
            "${LCG_RELEASE_BASE}</env:default>\n" )
      endif()
      # Set LCG_PLATFORM if it's available:
      if( LCG_PLATFORM )
         file( APPEND ${ARG_XENV_FILE}.in
            "   <env:default variable=\"LCG_PLATFORM\">"
            "${LCG_PLATFORM}</env:default>\n" )
      endif()

      # Set the general environment variables:
      set( _envCopy ${_environments} )
      while( _envCopy )
         # Get the instruction and the mandatory name:
         list( GET _envCopy 0 _instruction )
         list( GET _envCopy 1 _name )
         # Now decide what to do with this instruction:
         if( "${_instruction}" STREQUAL "UNSET" )
            file( APPEND ${ARG_XENV_FILE}.in
               "   <env:unset variable=\"${_name}\"/>\n" )
            list( REMOVE_AT _envCopy 0 1 )
         elseif( "${_instruction}" STREQUAL "SET" OR
               "${_instruction}" STREQUAL "FORCESET" )
            list( GET _envCopy 2 _value )
            file( APPEND ${ARG_XENV_FILE}.in
               "   <env:set variable=\"${_name}\">${_value}</env:set>\n" )
            list( REMOVE_AT _envCopy 0 1 2 )
         elseif( "${_instruction}" STREQUAL "APPEND" )
            list( GET _envCopy 2 _value )
            file( APPEND ${ARG_XENV_FILE}.in
               "   <env:append variable=\"${_name}\">${_value}</env:append>\n" )
            list( REMOVE_AT _envCopy 0 1 2 )
         elseif( "${_instruction}" STREQUAL "PREPEND" )
            list( GET _envCopy 2 _value )
            file( APPEND ${ARG_XENV_FILE}.in
               "   <env:prepend variable=\"${_name}\">${_value}"
               "</env:prepend>\n" )
            list( REMOVE_AT _envCopy 0 1 2 )
         else()
            # Don't try to continue with this if we hit an issue:
            message( WARNING "Instruction ${_instruction} not known" )
            break()
         endif()
      endwhile()

      # Set the python paths:
      foreach( _path ${_python_paths} )
         file( APPEND ${ARG_XENV_FILE}.in
            "   <env:prepend variable=\"PYTHONPATH\">${_path}</env:prepend>\n" )
      endforeach()
      # Set the binary paths:
      foreach( _path ${_binary_paths} )
         file( APPEND ${ARG_XENV_FILE}.in
            "   <env:prepend variable=\"PATH\">${_path}</env:prepend>\n" )
      endforeach()
      # Set the library paths:
      foreach( _path ${_library_paths} )
         file( APPEND ${ARG_XENV_FILE}.in
            "   <env:prepend variable=\"LD_LIBRARY_PATH\">${_path}"
            "</env:prepend>\n" )
         if( APPLE )
            file( APPEND ${ARG_XENV_FILE}.in
               "   <env:prepend variable=\"DYLD_LIBRARY_PATH\">${_path}"
               "</env:prepend>\n" )
         endif()
      endforeach()
      # Set the include paths:
      foreach( _path ${_include_paths} )
         file( APPEND ${ARG_XENV_FILE}.in
            "   <env:prepend variable=\"ROOT_INCLUDE_PATH\">${_path}"
            "</env:prepend>\n" )
      endforeach()

      # Write the file's footer:
      file( APPEND ${ARG_XENV_FILE}.in "</env:config>\n" )

      # Generate the proper file out of the created skeleton at the
      # generation step. (To resolve possible generator expressions
      # in the path names.)
      file( GENERATE OUTPUT ${ARG_XENV_FILE} INPUT ${ARG_XENV_FILE}.in )

   endif()

endfunction( lcg_generate_env )
