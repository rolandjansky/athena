# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Module defining helper CMake code for the build of the xAOD packages.
#

# Include the required module(s).
include( CMakeParseArguments )

# Function setting up dictionaries for smart pointers.
#
# It can be used to generate the necessary XML lines for the selection.xml file
# of an xAOD package for generating the dictionaries for "standard" smart
# pointers pointing at the types defined in that package.
#
# Note that the <PkgName>Dict.h file still needs to be set up "by hand",
# preferably using the helper code from "xAODCore/tools/DictHelpers.h".
#
# Usage: atlas_add_xaod_smart_pointer_dicts(
#           INPUT <selection.xml in the package>
#           OUTPUT <variable name for the decorated XML file>
#           CONTAINERS [Type name(s) of xAOD containers]
#           OBJECTS [Type name(s) of standalone xAOD objects] )
#
function( atlas_add_xaod_smart_pointer_dicts )

   # Parse the options given to the function:
   cmake_parse_arguments( ARG "" "INPUT;OUTPUT" "CONTAINERS;OBJECTS" ${ARGN} )

   # Find all the files required by the function.
   find_program( _mergeSelectionsCmd mergeSelections.sh
      PATH_SUFFIXES scripts PATHS ${CMAKE_MODULE_PATH} )
   find_file( _xaodContainerDictSkeleton "xAODUtilitiesContainerDicts.xml.in"
      PATH_SUFFIXES "skeletons"
      PATHS "${xAODUtilities_DIR}"
            "${CMAKE_SOURCE_DIR}/../../Event/xAOD/xAODCore/cmake"
            ${CMAKE_MODULE_PATH} )
   find_file( _xaodObjectDictSkeleton "xAODUtilitiesObjectDicts.xml.in"
      PATH_SUFFIXES "skeletons"
      PATHS "${xAODUtilities_DIR}"
            "${CMAKE_SOURCE_DIR}/../../Event/xAOD/xAODCore/cmake"
            ${CMAKE_MODULE_PATH} )
   mark_as_advanced( _mergeSelectionsCmd _xaodContainerDictSkeleton
      _xaodObjectDictSkeleton )

   # List of the XML file fragments.
   set( _fragments )

   # Generate the XML file(s) for the container smart pointers.
   foreach( _type ${ARG_CONTAINERS} )

      # Sanitise the type name.
      string( REPLACE ":" "_" _typeSanitised ${_type} )
      string( REPLACE "<" "_" _typeSanitised ${_typeSanitised} )
      string( REPLACE ">" "_" _typeSanitised ${_typeSanitised} )
      string( REPLACE " " "_" _typeSanitised ${_typeSanitised} )

      # Set up the XML file fragment for this type.
      set( _fname
         "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/xAODLinks_${_typeSanitised}_selection.xml" )
      configure_file( "${_xaodContainerDictSkeleton}" "${_fname}" @ONLY )
      list( APPEND _fragments "${_fname}" )

      # Clean up.
      unset( _typeSanitised )
      unset( _fname )

   endforeach()

   # Generate the XML file(s) for the single-object smart pointers.
   foreach( _type ${ARG_OBJECTS} )

      # Sanitise the type name.
      string( REPLACE ":" "_" _typeSanitised ${_type} )
      string( REPLACE "<" "_" _typeSanitised ${_typeSanitised} )
      string( REPLACE ">" "_" _typeSanitised ${_typeSanitised} )
      string( REPLACE " " "_" _typeSanitised ${_typeSanitised} )

      # Set up the XML file fragment for this type.
      set( _fname
         "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/xAODLinks_${_typeSanitised}_selection.xml" )
      configure_file( "${_xaodObjectDictSkeleton}" "${_fname}" @ONLY )
      list( APPEND _fragments "${_fname}" )

      # Clean up.
      unset( _typeSanitised )
      unset( _fname )

   endforeach()

   # Remove the possible duplicates.
   list( REMOVE_DUPLICATES _fragments )

   # The name of the file to produce. It is done in such a complicated way
   # because atlas_add_dictionary(...) expects the selection XML file name to
   # be specified wrt. CMAKE_CURRENT_SOURCE_DIR.
   get_filename_component( _selName "${ARG_INPUT}" NAME_WE )
   set( _output
      "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/xAODLinks_${_selName}.xml" )
   file( RELATIVE_PATH _result "${CMAKE_CURRENT_SOURCE_DIR}" "${_output}" )
   set( ${ARG_OUTPUT} "${_result}" PARENT_SCOPE )
   unset( _selName )
   unset( _result )

   # Describe how to produce this merged selection file.
   add_custom_command( OUTPUT ${_output}
      COMMAND ${_mergeSelectionsCmd} "${_output}"
      "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_INPUT}" ${_fragments}
      DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_INPUT}" ${_fragments} )

   # Clean up.
   unset( _output )
   unset( _fragments )

endfunction()

# Mark the module as found.
set( xAODUtilities_FOUND TRUE )
