# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: AtlasDictionaryFunctions.cmake 744739 2016-05-04 08:23:56Z krasznaa $
#
# This file collects the ATLAS CMake helper functions that set up the build and
# installation of ROOT dictionaries in an offline or analysis build.
#
# This file should not be included directly, but through AtlasFunctions.cmake.
#

# Function generating a Reflex dictionary library that is linked against the
# library holding the object code of the types described by the library.
#
# The OPTIONS argument is just ignored for now. As none of its uses seems
# legitim in the code at the moment...
#
# Usage: atlas_add_dictionary( LibName Lib/LibDict.h Lib/selection.xml
#                              [INCLUDE_DIRS Include1...]
#                              [LINK_LIBRARIES Library1 Library2...]
#                              [EXTRA_FILES Root/dict/*.cxx]
#                              [NAVIGABLES type1...]
#                              [DATA_LINKS type2...]
#                              [ELEMENT_LINKS type3...]
#                              [ELEMENT_LINK_VECTORS type4...]
#                              [NO_ROOTMAP_MERGE]
#                              [OPTIONS option1 option2...] )
#
function( atlas_add_dictionary libName libHeader libSelection )

   # In release recompilation dryrun mode exit now:
   if( ATLAS_RELEASE_RECOMPILE_DRYRUN )
      return()
   endif()

   # Skip the library building if we are in release mode, but the package is not
   # getting recompiled:
   if( ATLAS_RELEASE_MODE AND NOT ATLAS_PACKAGE_RECOMPILE )
      return()
   endif()

   # A convenience variable:
   set( _argDefs INCLUDE_DIRS LINK_LIBRARIES EXTRA_FILES NAVIGABLES DATA_LINKS
      ELEMENT_LINKS ELEMENT_LINK_VECTORS OPTIONS )

   # Parse the options given to the function:
   cmake_parse_arguments( ARG "NO_ROOTMAP_MERGE" "" "${_argDefs}" ${ARGN} )

   # Warn the users about the ELEMENT_LINK_VECTORS arguments:
   if( ARG_ELEMENT_LINK_VECTORS )
      message( WARNING "ELEMENT_LINK_VECTORS is depreacted. "
         "Please use ELEMENT_LINKS instead." )
   endif()

   # Get the package/subdirectory name:
   atlas_get_package_name( pkgName )

   # Get the package directory:
   atlas_get_package_dir( pkgDir )

   # Set common compiler options:
   atlas_set_compiler_flags()

   # Header file and selection file components:
   set( _headerComponents )
   set( _selectionComponents )

   # Find the skeleton files:
   find_file( _dataLinkHeader DataLinkDict.h.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   find_file( _dataLinkSelection DataLink_selection.xml.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   find_file( _elementLinkHeader ElementLinkDict.h.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   find_file( _elementLinkSelection ElementLink_selection.xml.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   find_file( _elementLinkVectorHeader ElementLinkVectorDict.h.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   find_file( _elementLinkVectorSelection ElementLinkVector_selection.xml.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   find_file( _navigableHeader NavigableDict.h.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   find_file( _navigableSelection Navigable_selection.xml.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   mark_as_advanced( _dataLinkHeader _dataLinkSelection _elementLinkHeader
      _elementLinkSelection _elementLinkVectorHeader _elementLinkVectorSelection
      _navigableHeader _navigableSelection )

   # Find the merge commands:
   find_program( _mergeFilesCmd mergeFiles.sh
      PATH_SUFFIXES scripts PATHS ${CMAKE_MODULE_PATH} )
   find_program( _mergeSelectionsCmd mergeSelections.sh
      PATH_SUFFIXES scripts PATHS ${CMAKE_MODULE_PATH} )
   mark_as_advanced( _mergeFilesCmd _mergeSelectionsCmd )

   # If the user asked for additional DataLink<T> types to generate dictionaries
   # for, set these up now:
   foreach( type ${ARG_DATA_LINKS} )
      # Sanitise the type name:
      string( REPLACE ":" "_" typeSanitised ${type} )
      string( REPLACE "<" "_" typeSanitised ${typeSanitised} )
      string( REPLACE ">" "_" typeSanitised ${typeSanitised} )
      string( REPLACE " " "_" typeSanitised ${typeSanitised} )
      # Generate the header file:
      configure_file( ${_dataLinkHeader}
         ${CMAKE_CURRENT_BINARY_DIR}/DataLink${typeSanitised}.h @ONLY )
      # Generate the selection file:
      configure_file( ${_dataLinkSelection}
         ${CMAKE_CURRENT_BINARY_DIR}/DataLink${typeSanitised}_selection.xml
         @ONLY )
      # And now remember their names:
      list( APPEND _headerComponents
         ${CMAKE_CURRENT_BINARY_DIR}/DataLink${typeSanitised}.h )
      list( APPEND _selectionComponents
         ${CMAKE_CURRENT_BINARY_DIR}/DataLink${typeSanitised}_selection.xml )
   endforeach()

   # If the user asked for additional ElementLink<T> types to generate
   # dictionaries for, set these up now:
   foreach( type ${ARG_ELEMENT_LINKS} )
      # Sanitise the type name:
      string( REPLACE ":" "_" typeSanitised ${type} )
      string( REPLACE "<" "_" typeSanitised ${typeSanitised} )
      string( REPLACE ">" "_" typeSanitised ${typeSanitised} )
      string( REPLACE " " "_" typeSanitised ${typeSanitised} )
      # Generate the header files:
      configure_file( ${_dataLinkHeader}
         ${CMAKE_CURRENT_BINARY_DIR}/DataLink${typeSanitised}.h @ONLY )
      configure_file( ${_elementLinkHeader}
         ${CMAKE_CURRENT_BINARY_DIR}/ElementLink${typeSanitised}.h @ONLY )
      configure_file( ${_elementLinkVectorHeader}
         ${CMAKE_CURRENT_BINARY_DIR}/ElementLinkVector${typeSanitised}.h @ONLY )
      # Generate the selection file:
      configure_file( ${_dataLinkSelection}
         ${CMAKE_CURRENT_BINARY_DIR}/DataLink${typeSanitised}_selection.xml
         @ONLY )
      configure_file( ${_elementLinkSelection}
         ${CMAKE_CURRENT_BINARY_DIR}/ElementLink${typeSanitised}_selection.xml
         @ONLY )
      configure_file( ${_elementLinkVectorSelection}
         ${CMAKE_CURRENT_BINARY_DIR}/ElementLinkVector${typeSanitised}_selection.xml
         @ONLY )
      # And now remember their names:
      list( APPEND _headerComponents
         ${CMAKE_CURRENT_BINARY_DIR}/DataLink${typeSanitised}.h
         ${CMAKE_CURRENT_BINARY_DIR}/ElementLink${typeSanitised}.h
         ${CMAKE_CURRENT_BINARY_DIR}/ElementLinkVector${typeSanitised}.h )
      list( APPEND _selectionComponents
         ${CMAKE_CURRENT_BINARY_DIR}/DataLink${typeSanitised}_selection.xml
         ${CMAKE_CURRENT_BINARY_DIR}/ElementLink${typeSanitised}_selection.xml
         ${CMAKE_CURRENT_BINARY_DIR}/ElementLinkVector${typeSanitised}_selection.xml )
   endforeach()

   # If the user asked for additional Navigable<T> types to generate dictionaries
   # for, set these up now:
   foreach( type ${ARG_NAVIGABLES} )
      # Sanitise the type name:
      string( REPLACE ":" "_" typeSanitised ${type} )
      string( REPLACE "<" "_" typeSanitised ${typeSanitised} )
      string( REPLACE ">" "_" typeSanitised ${typeSanitised} )
      string( REPLACE " " "_" typeSanitised ${typeSanitised} )
      # Generate the header file:
      configure_file( ${_navigableHeader}
         ${CMAKE_CURRENT_BINARY_DIR}/Navigable${typeSanitised}.h @ONLY )
      # Generate the selection file:
      configure_file( ${_navigableSelection}
         ${CMAKE_CURRENT_BINARY_DIR}/Navigable${typeSanitised}_selection.xml
         @ONLY )
      # And now remember their names:
      list( APPEND _headerComponents
         ${CMAKE_CURRENT_BINARY_DIR}/Navigable${typeSanitised}.h )
      list( APPEND _selectionComponents
         ${CMAKE_CURRENT_BINARY_DIR}/Navigable${typeSanitised}_selection.xml )
   endforeach()

   # Prepare the header file:
   set( _dictHeader ${CMAKE_CURRENT_SOURCE_DIR}/${libHeader} )
   if( _headerComponents )
      # Remove the possible duplicates:
      list( REMOVE_DUPLICATES _headerComponents )
      # The name of the file to produce:
      get_filename_component( _headerName ${libHeader} NAME )
      set( _dictHeader ${CMAKE_CURRENT_BINARY_DIR}/${_headerName} )
      unset( _headerName )
      # Describe how to produce this merged header:
      add_custom_command( OUTPUT ${_dictHeader}
         COMMAND ${_mergeFilesCmd} ${_dictHeader}
         ${CMAKE_CURRENT_SOURCE_DIR}/${libHeader}
         ${_headerComponents}
         DEPENDS ${_headerComponents} )
   endif()

   # Prepare the selection file:
   set( _dictSelection ${CMAKE_CURRENT_SOURCE_DIR}/${libSelection} )
   if( _selectionComponents )
      # Remove the possible duplicates:
      list( REMOVE_DUPLICATES _selectionComponents )
      # The name of the file to produce:
      get_filename_component( _selectionName ${libSelection} NAME )
      set( _dictSelection ${CMAKE_CURRENT_BINARY_DIR}/${_selectionName} )
      unset( _selectionName )
      # Describe how to produce this merged selection file:
      add_custom_command( OUTPUT ${_dictSelection}
         COMMAND ${_mergeSelectionsCmd} ${_dictSelection}
         ${CMAKE_CURRENT_SOURCE_DIR}/${libSelection}
         ${_selectionComponents}
         DEPENDS ${_selectionComponents} )
   endif()

   # Add the directory holding the header file to the header search path,
   # if it is different from the header directory.
   set( _extraInclude )
   get_filename_component( _headerDir ${libHeader} PATH )
   get_filename_component( _headerDirName ${_headerDir} NAME )
   get_filename_component( _headerDir ${CMAKE_CURRENT_SOURCE_DIR}/${_headerDir}
      ABSOLUTE )
   if( NOT ${_headerDirName} STREQUAL ${pkgName} )
      set( _extraInclude $<BUILD_INTERFACE:${_headerDir}> )
   endif()
   unset( _headerDir )
   unset( _headerDirName )

   # Set up the options(s) for propagation:
   set( _rootmapOpt )
   if( ARG_NO_ROOTMAP_MERGE )
      set( _rootmapOpt NO_ROOTMAP_MERGE )
   endif()

   # Generate a dictionary file:
   atlas_generate_reflex_dictionary( _reflexSource ${libName}
      HEADER ${_dictHeader} SELECTION ${_dictSelection}
      LINK_LIBRARIES ${pkgName}Pkg ${ARG_LINK_LIBRARIES}
      INCLUDE_DIRS ${ARG_INCLUDE_DIRS}
      ${_rootmapOpt} )

   # The library needs to be linked to the core ROOT libraries:
   find_package( ROOT QUIET REQUIRED COMPONENTS Core )

   # Resolve possible wildcards in the extra source file names:
   set( _sources )
   foreach( _extra ${ARG_EXTRA_FILES} )
      file( GLOB _files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
         ${_extra} )
      foreach( _file ${_files} )
         list( APPEND _sources ${_file} )
      endforeach()
   endforeach()

   # Put the files into source groups. So they would show up in a ~reasonable
   # way in an IDE like Xcode:
   atlas_group_source_files( ${_sources} )

   # Create a library from all the sources:
   add_library( ${libName} SHARED ${_reflexSource} ${_sources} )

   # Set up its properties:
   add_dependencies( ${libName} ${pkgName}Pkg )
   add_dependencies( Package_${pkgName} ${libName} )
   set_property( TARGET ${libName} PROPERTY LABELS ${pkgName} )
   set_property( TARGET ${libName} PROPERTY FOLDER ${pkgDir} )
   target_include_directories( ${libName} PRIVATE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
      ${_extraInclude}
      $<TARGET_PROPERTY:${pkgName}Pkg,INTERFACE_INCLUDE_DIRECTORIES>
      $<TARGET_PROPERTY:${pkgName}PkgPrivate,INTERFACE_INCLUDE_DIRECTORIES>
      ${ARG_INCLUDE_DIRS}
      ${ROOT_INCLUDE_DIRS} )
   unset( _extraInclude )
   if( NOT APPLE )
      # Turn off the --as-needed flag in this ugly way. As CMake doesn't seem
      # to provide any more elegant way of overriding the default linker flags
      # library-by-library.
      target_link_libraries( ${libName} "-Wl,--no-as-needed" )
   endif()
   target_link_libraries( ${libName} ${ROOT_LIBRARIES} ${ARG_LINK_LIBRARIES} )

   # In case we are building optimised libraries with debug info, and we have
   # objcopy available, detach the debug information into a separate library
   # file.
   if( "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" AND CMAKE_OBJCOPY )
      set( _libFileName "${CMAKE_SHARED_LIBRARY_PREFIX}${libName}" )
      set( _libFileName "${_libFileName}${CMAKE_SHARED_LIBRARY_SUFFIX}" )
      add_custom_command( TARGET ${libName} POST_BUILD
         COMMAND ${CMAKE_OBJCOPY} --only-keep-debug ${_libFileName}
         ${_libFileName}.dbg
         COMMAND ${CMAKE_OBJCOPY} --strip-debug ${_libFileName}
         COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=${_libFileName}.dbg
         ${_libFileName}
         WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
         COMMENT
         "Detaching debug info of ${_libFileName} into ${_libFileName}.dbg" )
      install( FILES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${_libFileName}.dbg
         DESTINATION ${CMAKE_INSTALL_LIBDIR}
         COMPONENT "Debug"
         OPTIONAL )
   endif()

   # On MacOS X make sure that the dictionary library is soft linked
   # with a .so postfix as well. Because genreflex can't make rootmap
   # files that would find libraries with a .dylib extension. And we
   # must create the dictionary as a shared library, not a module
   # library.
   if( APPLE )
      set( _linkname "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/" )
      set( _linkname "${_linkname}${CMAKE_SHARED_MODULE_PREFIX}${libName}" )
      set( _linkname "${_linkname}${CMAKE_SHARED_MODULE_SUFFIX}" )
      add_custom_target( ${libName}Link
         ALL SOURCES ${_linkname} )
      add_dependencies( ${libName}Link ${libName} )
      add_dependencies( Package_${pkgName} ${libName}Link )
      set_property( TARGET ${libName}Link PROPERTY LABELS ${pkgName} )
      set_property( TARGET ${libName}Link PROPERTY FOLDER ${pkgDir} )
      add_custom_command( OUTPUT ${_linkname}
         COMMAND ${CMAKE_COMMAND} -E create_symlink
         ${CMAKE_SHARED_LIBRARY_PREFIX}${libName}${CMAKE_SHARED_LIBRARY_SUFFIX}
         ${_linkname} )
      set_directory_properties( PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
         ${_linkname} )
      install( FILES ${_linkname}
         DESTINATION ${CMAKE_INSTALL_LIBDIR} )
   endif()

   # Declare how to install the library:
   install( TARGETS ${libName}
      DESTINATION ${CMAKE_INSTALL_LIBDIR} OPTIONAL )

endfunction( atlas_add_dictionary )

# This function can be used by the packages to generate a CINT dictionary
# source file, that can be built into a "normal" library in the package.
#
# In case the user only specifies a LinkDef.h file without any other header
# file names, the code attempts to extract the relevant header file names
# from the LinkDef.h file itself. But this should just be used as a stopgap
# solution until all RootCore packages are properly migrated to the new
# build system.
#
# The EXTERNAL_PACKAGES option can be used to define packages that the
# dictionary generation depends on. Typically this will just be ROOT.
#
# Usage: atlas_add_root_dictionary( mainLibName dictFileNameVar
#                                   [ROOT_HEADERS Header1.h LinkDef.h]
#                                   [EXTERNAL_PACKAGES ROOT]
#                                   [INCLUDE_PATHS /some/path...] )
#
function( atlas_add_root_dictionary libName dictfile )

   # In release recompilation dryrun mode exit now:
   if( ATLAS_RELEASE_RECOMPILE_DRYRUN )
      return()
   endif()

   # Skip the dictionary building if we are in release mode, but the package is
   # not getting recompiled:
   if( ATLAS_RELEASE_MODE AND NOT ATLAS_PACKAGE_RECOMPILE )
      return()
   endif()

   # Parse the options given to the function:
   cmake_parse_arguments( ARG "" ""
      "ROOT_HEADERS;EXTERNAL_PACKAGES;INCLUDE_PATHS" ${ARGN} )

   # Some sanity checks:
   if( NOT ARG_ROOT_HEADERS )
      message( SEND_ERROR "No ROOT_HEADERS option specified!" )
      return()
   endif()

   # Set common compiler options:
   atlas_set_compiler_flags()

   # Get the package/subdirectory name:
   atlas_get_package_name( pkgName )

   # Get the package directory:
   atlas_get_package_dir( pkgDir )

   # Find ROOT, as we need to get the dictionary generator's location from it:
   find_package( ROOT QUIET REQUIRED )

   # If the user just specified "Root/LinkDef.h" as the header, then extract
   # the list of all the needed headers from that file. Otherwise just take
   # the specified headers as they are.
   set( headers )
   list( LENGTH ARG_ROOT_HEADERS _nHeaders )
   if( _nHeaders EQUAL 1 )
      if( ${ARG_ROOT_HEADERS} STREQUAL "Root/LinkDef.h" )
         atlas_get_linkdef_includes( ${ARG_ROOT_HEADERS} _headerstemp )
         set( headers )
         foreach( _header ${_headerstemp} )
            if( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_header} )
               list( APPEND headers ${_header} )
            else()
               # Try to find the header file somewhere in the source tree:
               get_filename_component( _headerFileName ${_header} NAME )
               file( GLOB_RECURSE _foundHeaders RELATIVE ${CMAKE_SOURCE_DIR}
                  "${CMAKE_SOURCE_DIR}/*/${_headerFileName}" )
               list( LENGTH _foundHeaders _nFoundHeaders )
               if( _nFoundHeaders EQUAL 1 )
                  list( APPEND headers ${CMAKE_SOURCE_DIR}/${_foundHeaders} )
               else()
                  # If we still haven't found it, look in all of the base
                  # projects as well:
                  set( _headerWasFound FALSE )
                  foreach( _path ${CMAKE_PREFIX_PATH} $ENV{CMAKE_PREFIX_PATH} )
                     file( GLOB_RECURSE _foundHeaders RELATIVE ${_path}
                        "${_path}/*/${_header}" )
                     list( LENGTH _foundHeaders _nFoundHeaders )
                     if( _nFoundHeaders EQUAL 1 )
                        list( APPEND headers ${_path}/${_foundHeaders} )
                        set( _headerWasFound TRUE )
                        break()
                     endif()
                  endforeach()
                  if( NOT _headerWasFound )
                     message( SEND_ERROR
                        "Couldn't find ${_header} for CINT dictionary "
                        "generation. Please specify the header file "
                        "names in the CMakeLists.txt file explicitly." )
                  endif()
                  unset( _headerWasFound )
               endif()
            endif()
         endforeach()
      endif()
   endif()
   foreach( _header ${ARG_ROOT_HEADERS} )
      file( GLOB _files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
         ${_header} )
      foreach( _file ${_files} )
         list( APPEND headers ${_file} )
      endforeach()
   endforeach()

   # The name of the files that we'll be generating:
   set( dictsource ${CMAKE_CURRENT_BINARY_DIR}/${libName}CintDict.cxx )
   set( pcm_name "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/" )
   set( pcm_name "${pcm_name}${CMAKE_SHARED_LIBRARY_PREFIX}" )
   set( pcm_name "${pcm_name}${libName}_rdict.pcm" )
   set( rootmap_name ${CMAKE_CURRENT_BINARY_DIR}/${libName}.dsomap )

   # The library's physical name:
   set( library_name
      ${CMAKE_SHARED_LIBRARY_PREFIX}${libName}${CMAKE_SHARED_LIBRARY_SUFFIX} )

   # Get the compiler definitions:
   get_directory_property( definitions COMPILE_DEFINITIONS )

   # Unfortunately at this point the escape characters are gone from the
   # definitions. Most notably for the PACKAGE_VERSION macro. So let's add
   # the escapes back.
   set( defCopy ${definitions} )
   set( definitions )
   foreach( _def ${defCopy} )
      string( REPLACE "\"" "\\\"" _newDef ${_def} )
      list( APPEND definitions ${_newDef} )
   endforeach()

   # Get the include directories:
   get_directory_property( incdirs INCLUDE_DIRECTORIES )

   # Look for extra packages if necessary, and set up their include
   # paths:
   foreach( _extpkg ${ARG_EXTERNAL_PACKAGES} )
      find_package( ${_extpkg} )
      string( TOUPPER ${_extpkg} _pkgup )
      if( ${_pkgup}_FOUND )
         list( APPEND incdirs ${${_pkgup}_INCLUDES} )
      endif()
   endforeach()

   # Add the include directories needed for the main library:
   list( INSERT incdirs 0
      $<TARGET_PROPERTY:${libName},INTERFACE_INCLUDE_DIRECTORIES> )

   # Add the package's directory as an include directory. This is needed
   # explicitly for the case when we generate a dictionary for a library that
   # doesn't provide public headers. And hence doesn't publish its source
   # in INTERFACE_INCLUDE_DIRECTORIES.
   list( INSERT incdirs 0 $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
      $<TARGET_PROPERTY:${pkgName}Pkg,INTERFACE_INCLUDE_DIRECTORIES>
      $<TARGET_PROPERTY:${pkgName}PkgPrivate,INTERFACE_INCLUDE_DIRECTORIES> )

   # The path to rootcling:
   set( rootcling ${ROOT_BINARY_PATH}/rootcling )

   # The full command to generate the root dictionary:
   set( cmd "set -e\n" )
   set( cmd "${cmd}tmpdir=`mktemp -d make${libName}CintDict.XXXXXX`\n" )
   set( cmd "${cmd}cd \${tmpdir}\n" )
   set( cmd "${cmd}${rootcling} -f ${libName}CintDict.cxx" )
   set( cmd "${cmd} -s ${library_name}" )
   set( cmd "${cmd} -rml ${library_name} -rmf ${libName}.dsomap" )
   set( cmd "${cmd} -c -D$<JOIN:${definitions}, -D>" )
   set( cmd "${cmd} -I\"$<JOIN:${incdirs},\" -I\">\"" )
   set( cmd "${cmd} \"$<JOIN:${headers},\" \">\"\n" )
   set( cmd "${cmd}${CMAKE_COMMAND} -E copy ${libName}CintDict.cxx " )
   set( cmd "${cmd} \"${dictsource}\"\n" )
   set( cmd "${cmd}${CMAKE_COMMAND} -E copy ${libName}.dsomap" )
   set( cmd "${cmd} \"${rootmap_name}\"\n" )
   set( cmd "${cmd}${CMAKE_COMMAND} -E copy" )
   set( cmd "${cmd} ${CMAKE_SHARED_LIBRARY_PREFIX}${libName}_rdict.pcm" )
   set( cmd "${cmd} \"${pcm_name}\"\n" )

   # Generate a script that will run rootcling. This is necessary because
   # there are some bugs currently (3.3.2) in how add_custom_command handles
   # generator expressions. But file(GENERATE...) does do this correctly.
   file( GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/make${libName}CintDict.sh
      CONTENT "${cmd}" )

   # Call the generated script to create the dictionary files:
   add_custom_command( OUTPUT ${dictsource} ${pcm_name} ${rootmap_name}
      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
      COMMAND sh ${CMAKE_CURRENT_BINARY_DIR}/make${libName}CintDict.sh
      DEPENDS ${headers} ${pkgName}Pkg )
   set_property( SOURCE ${dictsource} ${pcm_name} ${rootmap_name}
      PROPERTY LABELS ${pkgName} )

   # Install the generated auxiliary files:
   install( FILES ${pcm_name}
      DESTINATION ${CMAKE_INSTALL_LIBDIR} OPTIONAL )

   # Set up the merging of the rootmap files:
   set_property( GLOBAL APPEND PROPERTY ATLAS_ROOTMAP_FILES ${rootmap_name} )
   set_property( GLOBAL APPEND PROPERTY ATLAS_ROOTMAP_TARGETS ${libName} )

   # Set the return argument:
   set( ${dictfile} ${dictsource} PARENT_SCOPE )

endfunction( atlas_add_root_dictionary )

# Function used instead of REFLEX_GENERATE_DICTIONARY when building a
# Reflex library, to overcome the shortcomings of the ROOT built-in
# function.
#
# This is a fairly complicated function. It needs to figure out the include
# paths required for the dictionary generation based on the include directories
# specified for the current directory, and the properties of the libraries
# that the dictionary library will eventually be linked against. (Doing the
# latter with generator expressions.)
#
# Usage: atlas_generate_reflex_dictionary( dictFileNameVar
#                                          dictNamePrefix
#                                          HEADER <pkgName>/<pkgName>Dict.h
#                                          SELECTION <pkgName>/selection.xml
#                                          [LINK_LIBRARIES Library1...]
#                                          [INCLUDE_DIRS Include1...]
#                                          [NO_ROOTMAP_MERGE] )
#
function( atlas_generate_reflex_dictionary dictfile dictname )

   # Parse all options:
   cmake_parse_arguments( ARG "NO_ROOTMAP_MERGE" "HEADER;SELECTION"
      "LINK_LIBRARIES;INCLUDE_DIRS" ${ARGN} )

   # A security check:
   if( NOT ARG_HEADER )
      message( FATAL_ERROR
         "No header file specified using the HEADER option" )
   endif()
   if( NOT ARG_SELECTION )
      message( FATAL_ERROR
         "No selection file specified using the SELECTION option" )
   endif()

   # Get the package/subdirectory name:
   atlas_get_package_name( pkgName )

   # Get the package directory:
   atlas_get_package_dir( pkgDir )

   # Find ROOT, as we need to get the dictionary generator's location from it:
   find_package( ROOT QUIET REQUIRED )

   # The name of the files that we'll be generating:
   set( dictsource ${CMAKE_CURRENT_BINARY_DIR}/${dictname}ReflexDict.cxx )
   set( pcm_name "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/" )
   set( pcm_name "${pcm_name}${CMAKE_SHARED_LIBRARY_PREFIX}" )
   set( pcm_name "${pcm_name}${dictname}_rdict.pcm" )
   set( rootmap_name ${CMAKE_CURRENT_BINARY_DIR}/${dictname}.dsomap )

   # The library's physical name:
   if( APPLE )
      set( library_name "${CMAKE_SHARED_MODULE_PREFIX}${dictname}" )
      set( library_name "${library_name}${CMAKE_SHARED_MODULE_SUFFIX}" )
   else()
      set( library_name "${CMAKE_SHARED_LIBRARY_PREFIX}${dictname}" )
      set( library_name "${library_name}${CMAKE_SHARED_LIBRARY_SUFFIX}" )
   endif()

   # Set some global options:
   add_definitions( ${ATLAS_DEFINITIONS} )

   # Get the compiler definitions:
   get_directory_property( definitions COMPILE_DEFINITIONS )

   # Simply always use the definition needed to build dictionaries for Eigen
   # classes. As the definition doesn't hurt when Eigen is not used anyway...
   list( APPEND definitions EIGEN_DONT_VECTORIZE )

   # Unfortunately at this point the escape characters are gone from the
   # definitions. Most notably for the PACKAGE_VERSION macro. So let's add
   # the escapes back.
   set( defCopy ${definitions} )
   set( definitions )
   foreach( _def ${defCopy} )
      string( REPLACE "\"" "\\\"" _newDef ${_def} )
      list( APPEND definitions ${_newDef} )
   endforeach()

   # Get the include directories:
   get_directory_property( incdirs INCLUDE_DIRECTORIES )
   # Add the package directory to it:
   list( INSERT incdirs 0 ${CMAKE_CURRENT_SOURCE_DIR} )
   # Add the header's directory to it:
   get_filename_component( _headerDir ${ARG_HEADER} PATH )
   list( APPEND incdirs ${_headerDir} )
   unset( _headerDir )
   # Add all the explicitly defined header directories:
   if( ARG_INCLUDE_DIRS )
      list( APPEND incdirs ${ARG_INCLUDE_DIRS} )
   endif()

   # Get the interface directories and definitions of the linked libraries:
   if( ARG_LINK_LIBRARIES )
      # First declare an interface library that will help us collect the
      # include paths from them:
      set( _genLibName ${dictname}GeneratorLib )
      add_library( ${_genLibName} INTERFACE )
      target_link_libraries( ${_genLibName} INTERFACE ${ARG_LINK_LIBRARIES} )
      # Now use this interface library to get the include paths:
      list( APPEND incdirs
         "$<TARGET_PROPERTY:${_genLibName},INTERFACE_INCLUDE_DIRECTORIES>" )
      # And then the definitions:
      list( APPEND definitions
         "$<TARGET_PROPERTY:${_genLibName},INTERFACE_COMPILE_DEFINITIONS>" )
      # Clean up:
      unset( _genLibName )
   endif()

   # The path to the genreflex executable:
   set( genreflex ${ROOT_BINARY_PATH}/genreflex )

   # The full command to generate the reflex dictionary:
   set( cmd "set -e\n" )
   set( cmd "${cmd}tmpdir=`mktemp -d make${dictname}ReflexDict.XXXXXX`\n" )
   set( cmd "${cmd}cd \${tmpdir}\n" )
   set( cmd "${cmd}${genreflex} ${ARG_HEADER}" )
   set( cmd "${cmd} -o ${dictname}ReflexDict.cxx" )
   set( cmd "${cmd} --noIncludePaths" )
   set( cmd "${cmd} --rootmap=${dictname}.dsomap" )
   set( cmd "${cmd} --rootmap-lib=${library_name}" )
   set( cmd "${cmd} --library=${library_name}" )
   set( cmd "${cmd} --select=\"${ARG_SELECTION}\"" )
   set( cmd "${cmd} -I\"$<JOIN:${incdirs},\" -I\">\"" )
   set( cmd "${cmd} -D$<JOIN:${definitions}, -D>\n" )
   set( cmd "${cmd}${CMAKE_COMMAND} -E copy ${dictname}ReflexDict.cxx" )
   set( cmd "${cmd} \"${dictsource}\"\n" )
   set( cmd "${cmd}${CMAKE_COMMAND} -E copy ${dictname}.dsomap" )
   set( cmd "${cmd} \"${rootmap_name}\"\n" )
   set( cmd "${cmd}${CMAKE_COMMAND} -E copy" )
   set( cmd "${cmd} ${CMAKE_SHARED_LIBRARY_PREFIX}${dictname}_rdict.pcm" )
   set( cmd "${cmd} \"${pcm_name}\"\n" )

   # Generate a script that will run genreflex. This is necessary because
   # there are some bugs currently (3.3.2) in how add_custom_command handles
   # generator expressions. But file(GENERATE...) does do this correctly.
   file( GENERATE OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/make${dictname}ReflexDict.sh
      CONTENT "${cmd}" )

   # Call the generated script to create the dictionary files:
   add_custom_command( OUTPUT ${dictsource} ${pcm_name} ${rootmap_name}
      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
      COMMAND sh ${CMAKE_CURRENT_BINARY_DIR}/make${dictname}ReflexDict.sh
      DEPENDS ${ARG_HEADER} ${ARG_SELECTION} ${pkgName}Pkg )
   set_property( SOURCE ${dictsource} ${pcm_name} ${rootmap_name}
      PROPERTY LABELS ${pkgName} )

   # Install the generated auxiliary files:
   install( FILES ${pcm_name}
      DESTINATION ${CMAKE_INSTALL_LIBDIR} OPTIONAL )

   # Set up the merging of the rootmap files:
   if( NOT ARG_NO_ROOTMAP_MERGE )
      set_property( GLOBAL APPEND PROPERTY ATLAS_ROOTMAP_FILES ${rootmap_name} )
      set_property( GLOBAL APPEND PROPERTY ATLAS_ROOTMAP_TARGETS ${dictname} )
   endif()

   # Set the return argument:
   set( ${dictfile} ${dictsource} PARENT_SCOPE )

endfunction( atlas_generate_reflex_dictionary )
