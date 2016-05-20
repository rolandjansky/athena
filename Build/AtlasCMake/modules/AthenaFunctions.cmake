# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: AthenaFunctions.cmake 717517 2016-01-13 13:06:59Z krasznaa $
#
# This file collects the functions forming a bridge between the calls made in
# the offline packages at the time of writing, and the functions implemented
# in AtlasFunctions.cmake.
#

# Include the required module(s):
include( AtlasFunctions )

# A very simple call forwarding to atlas_subdir.
#
# Note  that one actually has to use a macro here, and not a function. As the
# atlas_subdir function makes the assumption that when it sets some variables
# in its parents' scope, that is the scope of the package's CMakeLists.txt
# file.
#
# Usage: athena_subdir( MyPackage None )
#
macro( athena_subdir name version )
   atlas_subdir( ${name} ${version} )
endmacro( athena_subdir )

# Hide the function when building Gaudi itself...
if( NOT GAUDI_ATLAS )
   # A very simple call forwarding to atlas_depends_on_subdirs.
   #
   # In this case we may have used either a function or a macro. I just chose
   # a function because I thoguht that it would result in cleaner code.
   #
   # Usage: gaudi_depends_on_subdirs( Control/AthContainers ... )
   #
   function( gaudi_depends_on_subdirs )
      atlas_depends_on_subdirs( ${ARGN} )
   endfunction( gaudi_depends_on_subdirs )
endif()

# Provide a function for setting up the NightlyBuild.cmake and
# ExperimentalBuild.cmake scripts for building Gaudi.
#
# Usage: gaudi_ctest_setup()
#
macro( gaudi_ctest_setup )

   # First call the basic setup:
   atlas_ctest_setup()

   # Get the number of processor cores to use for the compilation.
   # Used by the release building script(s).
   atlas_cpu_cores( PROCESSOR_COUNT )

   # Get the name of the running machine. Needed for the CTest script(s).
   site_name( SITE_NAME )

   # Construct a platform name:
   atlas_platform_id( BUILDNAME )

   # Generate the build control script(s):
   find_file( _expBuild ExperimentalBuild.cmake.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   configure_file( ${_expBuild}
      ${CMAKE_BINARY_DIR}/ExperimentalBuild.cmake @ONLY )
   unset( _expBuild )
   find_file( _nightlyBuild NightlyBuild.cmake.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   configure_file( ${_nightlyBuild}
      ${CMAKE_BINARY_DIR}/NightlyBuild.cmake @ONLY )
   unset( _nightlyBuild )

   # Unset the now unnecessary variables:
   unset( PROCESSOR_COUNT )
   unset( SITE_NAME )

endmacro( gaudi_ctest_setup )

# A very simple call forwarding to atlas_install_headers.
#
# The function just provides an alias for the atlas prefixed function behaving
# the same way as the offline code expects it to work.
#
# Usage: athena_install_headers( HeaderDir )
#
function( athena_install_headers )
   atlas_install_headers( ${ARGN} )
endfunction( athena_install_headers )

# A very simple call forwarding to atlas_install_python_modules.
#
# The function just provides an alias for the atlas prefixed function behaving
# the same way as the offline code expects it to work.
#
# Usage: athena_install_python_modules( python/*.py )
#
function( athena_install_python_modules )
   atlas_install_python_modules( ${ARGN} )
endfunction( athena_install_python_modules )

# A very simple call forwarding to atlas_install_joboptions.
#
# The function just provides an alias for the atlas prefixed function behaving
# the same way as the offline code expects it to work.
#
# Usage: athena_install_joboptions( share/*.py )
#
function( athena_install_joboptions )
   atlas_install_joboptions( ${ARGN} )
endfunction( athena_install_joboptions )

# A very simple call forwarding to atlas_install_docs.
#
# The function just provides an alias for the atlas prefixed function behaving
# the same way as the offline code expects it to work.
#
# Usage: athena_install_docs( doc/*.html )
#
function( athena_install_docs )
   atlas_install_docs( ${ARGN} )
endfunction( athena_install_docs )

# A very simple call forwarding to atlas_add_alias.
#
# The function just provides an alias for the atlas prefixed function behaving
# the same way as the offline code expects it to work.
#
# Usage: athena_alias( newName command1 command2... )
#
function( athena_alias name )
   atlas_add_alias( ${name} ${ARGN} )
endfunction( athena_alias )

# A very simple call forwarding to atlas_install_runtime.
#
# The function just provides an alias for the atlas prefixed function behaving
# the same way as the offline code expects it to work.
#
# Usage: athena_install_runtime( share/*.sh )
#
function( athena_install_runtime )
   atlas_install_runtime( ${ARGN} )
endfunction( athena_install_runtime )

# A very simple call forwarding to atlas_install_xmls.
#
# The function just provides an alias for the atlas prefixed function behaving
# the same way as the offline code expects it to work.
#
# Usage: athena_install_xmls( data/*.xml )
#
function( athena_install_xmls )
   atlas_install_xmls( ${ARGN} )
endfunction( athena_install_xmls )

# This is a convenience function to translate between the offline script
# installation mechanism and the RootCore one. On this function I'm actually
# not sure that my interface would be the superior one. :-/ So maybe I'll just
# introduce a function called atlas_install_scripts later on...
#
function( athena_install_scripts )

   # This rule is ignored in release mode:
   if( ATLAS_RELEASE_MODE )
      return()
   endif()

   # Call atlas_add_executable with the right options for all the arguments
   # received:
   foreach( _file ${ARGN} )
      # Find all such files:
      file( GLOB _scripts RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${_file} )
      foreach( _script ${_scripts} )
         # Get the name of the script file:
         get_filename_component( _scriptFile ${_script} NAME )
         # Call atlas_add_executable:
         atlas_add_executable( ${_scriptFile} SCRIPT ${_script} )
      endforeach()
   endforeach()

endfunction( athena_install_scripts )

# This function adapts the slightly weird function calls of the offline
# release to the atlas_add_library function. Eventually the offline code
# should migrate to use atlas_add_library directly.
#
# The offline code declares include and library dependencies on external
# projects using the project names. And not the actual include directories
# or library names. This code makes sure that those declarations are
# interpreted in the way it was intended.
#
# Usage: athena_add_library( libraryName source1.cxx source2.cxx...
#                            [NO_PUBLIC_HEADERS | PUBLIC_HEADERS HeaderDir...]
#                            [INCLUDE_DIRS ROOT...]
#                            [LINK_LIBRARIES Library1 Library2...] )
#
function( athena_add_library libName )

   # We tweak the INCLUDE_DIRS and LINK_LIBRARIES parameters...
   cmake_parse_arguments( ARG "" "" "INCLUDE_DIRS;LINK_LIBRARIES" ${ARGN} )

   # Construct the arguments for the atlas_add_library call:
   set( _args ${ARG_UNPARSED_ARGUMENTS} )

   # Massage the include declarations:
   if( ARG_INCLUDE_DIRS )
      # Translate the specified "includes":
      atlas_get_includes( ARG_INCLUDE_DIRS _includes )
      # Now add them to the arguments:
      list( APPEND _args INCLUDE_DIRS )
      list( APPEND _args ${_includes} )
   endif()

   # Massage the library declarations:
   if( ARG_LINK_LIBRARIES )
      # Translate the specified "libraries":
      atlas_get_libraries( ARG_LINK_LIBRARIES _libraries )
      # Now add them to the arguments:
      list( APPEND _args LINK_LIBRARIES )
      list( APPEND _args ${_libraries} )
   endif()

   # Now call the underlying function:
   atlas_add_library( ${libName} ${_args} )

endfunction( athena_add_library )

# Just like athena_add_library, this function takes care of interpreting
# the includes and libraries coming from external packages correctly for the
# offline build.
#
# Usage: athena_add_component( libraryName source1.cxx source2.cxx...
#                              [NOCLIDDB]
#                              [GENCONF_PRELOAD preload]
#                              [INCLUDE_DIRS ROOT...]
#                              [LINK_LIBRARIES Library1 Library2...] )
#
function( athena_add_component libName )

   # We tweak the INCLUDE_DIRS and LINK_LIBRARIES parameters...
   cmake_parse_arguments( ARG "" "" "INCLUDE_DIRS;LINK_LIBRARIES" ${ARGN} )

   # Construct the arguments for the atlas_add_library call:
   set( _args ${ARG_UNPARSED_ARGUMENTS} )

   # Massage the include declarations:
   if( ARG_INCLUDE_DIRS )
      # Translate the specified "includes":
      atlas_get_includes( ARG_INCLUDE_DIRS _includes )
      # Now add them to the arguments:
      list( APPEND _args INCLUDE_DIRS )
      list( APPEND _args ${_includes} )
   endif()

   # Massage the library declarations:
   if( ARG_LINK_LIBRARIES )
      # Translate the specified "libraries":
      atlas_get_libraries( ARG_LINK_LIBRARIES _libraries )
      # Now add them to the arguments:
      list( APPEND _args LINK_LIBRARIES )
      list( APPEND _args ${_libraries} )
   endif()

   # Now call the underlying function:
   atlas_add_component( ${libName} ${_args} )

endfunction( athena_add_component )

# A very simple call forwarding to atlas_generate_cliddb.
#
# The function just provides an alias for the atlas prefixed function behaving
# the same way as the offline code expects it to work.
#
# Usage: athena_generate_cliddb( MyLibrary )
#
function( athena_generate_cliddb )

   # Forward the call:
   atlas_generate_cliddb( ${ARGN} )

endfunction( athena_generate_cliddb )

# Just like athena_add_library, this function takes care of interpreting
# the includes and libraries coming from external packages correctly for the
# offline build.
#
# Usage: athena_add_tpcnv_library( libraryName source1.cxx source2.cxx...
#                                  PUBLIC_HEADERS HeaderDir...
#                                  [INCLUDE_DIRS ROOT...]
#                                  [LINK_LIBRARIES Library1 Library2...] )
#
function( athena_add_tpcnv_library libName )

   # We tweak the INCLUDE_DIRS and LINK_LIBRARIES parameters...
   cmake_parse_arguments( ARG "" "" "INCLUDE_DIRS;LINK_LIBRARIES" ${ARGN} )

   # Construct the arguments for the atlas_add_library call:
   set( _args ${ARG_UNPARSED_ARGUMENTS} )

   # Massage the include declarations:
   if( ARG_INCLUDE_DIRS )
      # Translate the specified "includes":
      atlas_get_includes( ARG_INCLUDE_DIRS _includes )
      # Now add them to the arguments:
      list( APPEND _args INCLUDE_DIRS )
      list( APPEND _args ${_includes} )
   endif()

   # Massage the library declarations:
   if( ARG_LINK_LIBRARIES )
      # Translate the specified "libraries":
      atlas_get_libraries( ARG_LINK_LIBRARIES _libraries )
      # Now add them to the arguments:
      list( APPEND _args LINK_LIBRARIES )
      list( APPEND _args ${_libraries} )
   endif()

   # Now call the underlying function:
   atlas_add_tpcnv_library( ${libName} ${_args} )

endfunction( athena_add_tpcnv_library )

# Just like athena_add_library, this function takes care of interpreting
# the includes and libraries coming from external packages correctly for the
# offline build.
#
# Usage: athena_add_poolcnv_library( libraryName source1.cxx source2.cxx...
#                                    FILES EventInfo/EventInfo.h...
#                                    [INCLUDE_DIRS ROOT...]
#                                    [LINK_LIBRARIES Library1 Library2...] )
#
function( athena_add_poolcnv_library libName )

   # We tweak the INCLUDE_DIRS and LINK_LIBRARIES parameters...
   cmake_parse_arguments( ARG "" "" "INCLUDE_DIRS;LINK_LIBRARIES" ${ARGN} )

   # Construct the arguments for the atlas_add_library call:
   set( _args ${ARG_UNPARSED_ARGUMENTS} )

   # Massage the include declarations:
   if( ARG_INCLUDE_DIRS )
      # Translate the specified "includes":
      atlas_get_includes( ARG_INCLUDE_DIRS _includes )
      # Now add them to the arguments:
      list( APPEND _args INCLUDE_DIRS )
      list( APPEND _args ${_includes} )
   endif()

   # Massage the library declarations:
   if( ARG_LINK_LIBRARIES )
      # Translate the specified "libraries":
      atlas_get_libraries( ARG_LINK_LIBRARIES _libraries )
      # Now add them to the arguments:
      list( APPEND _args LINK_LIBRARIES )
      list( APPEND _args ${_libraries} )
   endif()

   # Now call the underlying function:
   atlas_add_poolcnv_library( ${libName}PoolCnv ${_args} )

endfunction( athena_add_poolcnv_library )

# This function adapts the slightly weird function calls of the offline
# release to the atlas_add_dictionary function. Eventually the offline code
# should migrate to use atlas_add_library directly.
#
# Usage: athena_add_dictionary( LibName Lib/LibDict.h Lib/selection.xml
#                               [INCLUDE_DIRS ROOT...]
#                               [LINK_LIBRARIES Library1 Library2...]
#                               [EXTRA_FILES Root/dict/*.cxx] )
#
function( athena_add_dictionary libName )

   # A convenience variable:
   set( _argDefs INCLUDE_DIRS LINK_LIBRARIES )

   # We tweak the INCLUDE_DIRS and LINK_LIBRARIES parameters...
   cmake_parse_arguments( ARG "" "" "${_argDefs}" ${ARGN} )

   # Construct the arguments for the atlas_add_dictionary call:
   set( _args ${ARG_UNPARSED_ARGUMENTS} )

   # Massage the include declarations:
   if( ARG_INCLUDE_DIRS )
      # Translate the specified "includes":
      atlas_get_includes( ARG_INCLUDE_DIRS _includes )
      # Now add them to the arguments:
      list( APPEND _args INCLUDE_DIRS )
      list( APPEND _args ${_includes} )
   endif()

   # Massage the library declarations:
   if( ARG_LINK_LIBRARIES )
      # Translate the specified "libraries":
      atlas_get_libraries( ARG_LINK_LIBRARIES _libraries )
      # Now add them to the arguments:
      list( APPEND _args LINK_LIBRARIES )
      list( APPEND _args ${_libraries} )
   endif()

   # Now call the underlying function. Note the addition of the "Dict" postfix.
   # Which is just a different naming convention than what atlas_add_dictionary
   # uses...
   atlas_add_dictionary( ${libName}Dict ${_args} )

endfunction( athena_add_dictionary )

# Function adapting the "proper" function to work in the current
# Athena environment.
#
# The Athena code just uses a naming convention that we need to adapt to...
#
# Usage: athena_add_root_dictionary( mainLibName dictFileNameVar
#                                    [ROOT_HEADERS Header1.h LinkDef.h]
#                                    [EXTERNAL_PACKAGES ROOT]
#                                    [INCLUDE_PATHS /some/path...] )
#
function( athena_add_root_dictionary name )

   # Remove a possible "Dict" postfix from the name to get the name of the
   # main library that this file is being made for:
   string( REPLACE "Dict" "" _libName ${name} )

   # Call the "normal" function:
   atlas_add_root_dictionary( ${_libName} _cxxFileName ${ARGN} )

   # Set up a rule that makes a copy of this file with the name that the
   # Athena code expects at the moment:
   add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dict/${name}.cxx
      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_CURRENT_BINARY_DIR}/dict
      COMMAND ${CMAKE_COMMAND} -E copy ${_cxxFileName}
      ${CMAKE_CURRENT_BINARY_DIR}/dict/${name}.cxx
      DEPENDS ${_cxxFileName} )

   # Create a dummy target that the current CMake code depends on:
   add_custom_target( ${name} )

endfunction( athena_add_root_dictionary )

# This function adapts the atlas_add_executable function to work in the
# offline environment. It massages the LINK_LIBRARIES argument so that it
# would swallow external package names, and then calls atlas_add_executable
# with the right options.
#
# Usage: athena_add_executable( ExecutableName util/source1.cxx...
#                               [LINK_LIBRARIES Pkg1...] )
#
function( athena_add_executable exeName )

   # Parse the options given to the function:
   cmake_parse_arguments( ARG "" "" "INCLUDE_DIRS;LINK_LIBRARIES" ${ARGN} )

   # Construct the arguments for the atlas_add_executable call:
   set( _args ${ARG_UNPARSED_ARGUMENTS} )

   # Massage the include declarations:
   if( ARG_INCLUDE_DIRS )
      # Translate the specified "includes":
      atlas_get_includes( ARG_INCLUDE_DIRS _includes )
      # Now add them to the arguments:
      list( APPEND _args INCLUDE_DIRS )
      list( APPEND _args ${_includes} )
   endif()

   # Massage the library declarations:
   if( ARG_LINK_LIBRARIES )
      # Translate the specified "libraries":
      atlas_get_libraries( ARG_LINK_LIBRARIES _libraries )
      # Now add them to the arguments:
      list( APPEND _args LINK_LIBRARIES )
      list( APPEND _args ${_libraries} )
   endif()

   # Call the underlying function with the right options:
   atlas_add_executable( ${exeName} ${_args} SOURCES ${ARG_UNPARSED_ARGUMENTS} )

endfunction( athena_add_executable )

# Translate the settings made here for the atlas_add_env_setting function.
# This is not a too much used function in the offline build, so not much
# clue on how it should really work...
#
function( athena_env )

endfunction( athena_env )

# This is a no-op function for us, as the normal atlas_install_headers function
# picks up the files that apparently athena_install_headers doesn't.
#
function( athena_install_nonstandard_headers )

endfunction( athena_install_nonstandard_headers )

# This function just takes care of compiling the L1Common package in the
# DetCommon project. So eventually all this code should just be put into
# that particular package, and not live with all the generic release building
# code...
#
function( athena_add_l1common_files l1name )

   # This rule is ignored in release mode:
   if( ATLAS_RELEASE_MODE )
      return()
   endif()

   find_package( PythonInterp )

   atlas_get_package_name( package )
   set( generator XMLtoHeader.py )

   CMAKE_PARSE_ARGUMENTS( ARG "" "SCHEMA" "" ${ARGN} )

   find_file( genfile ${generator}
      PATHS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )

   if( NOT genfile )
      if( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${generator} )
         file( INSTALL ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${generator}
            DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
            GROUP_EXECUTE GROUP_READ
            WORLD_EXECUTE WORLD_READ )
         install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${generator}
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
            GROUP_EXECUTE GROUP_READ
            WORLD_EXECUTE WORLD_READ)
      endif()
   endif()

   set_directory_properties( PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${generator} )

   find_file( genfile ${generator}
      PATHS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )

   if( genfile )
      set( out_dir ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/${package} )
      set( l1out ${out_dir}/${l1name} )

      add_custom_command( OUTPUT ${l1out}
         COMMAND ${CMAKE_COMMAND} -E make_directory ${out_dir}
         COMMAND ${PYTHON_EXECUTABLE} ${genfile} -o ${l1out}
         -i ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_SCHEMA} )

      add_custom_target( ${l1name} ALL DEPENDS ${l1out} )
      install( DIRECTORY ${out_dir} DESTINATION include
         FILES_MATCHING PATTERN "*.h" )
      install( DIRECTORY ${out_dir} DESTINATION ${CMAKE_INSTALL_PYTHONDIR} 
         FILES_MATCHING PATTERN "*.py" )
   endif()
endfunction()
