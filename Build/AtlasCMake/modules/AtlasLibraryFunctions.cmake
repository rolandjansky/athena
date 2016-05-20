# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: AtlasLibraryFunctions.cmake 744366 2016-05-03 07:50:36Z krasznaa $
#
# This file collects the ATLAS CMake helper functions that set up the build and
# installation of all the different kinds of libraries that we create in offline
# and analysis releases.
#
# This file should not be included directly, but through AtlasFunctions.cmake.
#

# The main function for creating a standard library in the analysis release.
# It creates a library that other components can link against (or not...),
# and installs it in the proper directory.
#
# One must use either NO_PUBLIC_HEADERS, or PUBLIC_HEADERS with a list of
# directories that contain the public headers. Not specifying either of them
# is an error.
#
# By default the function creates a shared library. If no source files are
# given to the function, or if INTERFACE is specified, an INTERFACE library
# is set up. If MODULE is specified, a MODULE (unlinkable) library is set up.
# Only one of these flags may be specified at a given time.
#
# INCLUDE_DIRS can be used to specify a list of directories that need to
# be seen by the compiler to build the library. And by all children of the
# library to compile against it.
#
# LINK_LIBRARIES can be used to specify library targets, or physical library
# files that need to be linked against when building the library. The
# INCLUDE_DIRS and LINK_LIBRARIES properties of the linked library targets
# are used transitively during the build.
#
# Usage: atlas_add_library( libraryName source1.cxx source2.cxx...
#                           [NO_PUBLIC_HEADERS | PUBLIC_HEADERS HeaderDir...]
#                           [INTERFACE | SHARED | MODULE]
#                           [INCLUDE_DIRS Include1...]
#                           [PRIVATE_INCLUDE_DIRS Include2...]
#                           [LINK_LIBRARIES Library1 Library2...]
#                           [PRIVATE_LINK_LIBRARIES Library3...]
#                           [DEFINITIONS -Done...]
#                           [PRIVATE_DEFINITIONS -Dtwo...] )
#
function( atlas_add_library libName )

   # Parse the options given to the function:
   set( _noParamArgs NO_PUBLIC_HEADERS INTERFACE SHARED MODULE )
   set( _multiParamArgs PUBLIC_HEADERS INCLUDE_DIRS PRIVATE_INCLUDE_DIRS
      LINK_LIBRARIES PRIVATE_LINK_LIBRARIES DEFINITIONS PRIVATE_DEFINITIONS )
   cmake_parse_arguments( ARG "${_noParamArgs}" "" "${_multiParamArgs}"
      ${ARGN} )
   unset( _noParamArgs )
   unset( _multiParamArgs )

   # Get the package/subdirectory name:
   atlas_get_package_name( pkgName )

   # Get the package directory:
   atlas_get_package_dir( pkgDir )

   # In release recompilation dryrun mode exit now:
   if( ATLAS_RELEASE_RECOMPILE_DRYRUN )
      return()
   endif()

   # If we are in "release mode" and the library doesn't need to be
   # recompiled, then import the existing library with the right name:
   if( ATLAS_RELEASE_MODE AND NOT ATLAS_PACKAGE_RECOMPILE )
      # If we don't need to recompile because this library is already
      # built in the work area, then exit now...
      if( TARGET ${libName} )
         return()
      endif()
      # Check that we have this library in the release area:
      if( NOT TARGET ${ATLAS_RELEASE_NAME}::${libName} )
         message( SEND_ERROR
            "Library ${libName} not available from release "
            "${ATLAS_RELEASE_NAME}" )
         return()
      endif()
      # Get the type of this library:
      get_property( _type TARGET ${ATLAS_RELEASE_NAME}::${libName}
         PROPERTY TYPE )
      # Set up the imported library, with the right name and type:
      if( "${_type}" STREQUAL "SHARED_LIBRARY" )
         add_library( ${libName} SHARED IMPORTED GLOBAL )
      elseif( "${_type}" STREQUAL "MODULE_LIBRARY" )
         add_library( ${libName} MODULE IMPORTED GLOBAL )
      elseif( "${_type}" STREQUAL "INTERFACE_LIBRARY" )
         add_library( ${libName} INTERFACE IMPORTED GLOBAL )
      endif()
      # Copy the properties available for all library types:
      get_property( _libSet TARGET ${ATLAS_RELEASE_NAME}::${libName}
         PROPERTY INTERFACE_LINK_LIBRARIES SET )
      if( _libSet )
         get_target_property( _libraries ${ATLAS_RELEASE_NAME}::${libName}
            INTERFACE_LINK_LIBRARIES )
         string( REPLACE "${ATLAS_RELEASE_NAME}::" "" _libraries
            "${_libraries}" )
         set_target_properties( ${libName} PROPERTIES
            INTERFACE_LINK_LIBRARIES "${_libraries}" )
         unset( _libraries )
      endif()
      unset( _libSet )
      get_property( _incSet TARGET ${ATLAS_RELEASE_NAME}::${libName}
         PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES SET )
      if( _incSet )
         get_target_property( _includes ${ATLAS_RELEASE_NAME}::${libName}
            INTERFACE_SYSTEM_INCLUDE_DIRECTORIES )
         string( REPLACE "${ATLAS_RELEASE_NAME}::" "" _includes
            "${_includes}" )
         set_target_properties( ${libName} PROPERTIES
            INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${_includes}" )
         unset( _includes )
      endif()
      get_property( _incSet TARGET ${ATLAS_RELEASE_NAME}::${libName}
         PROPERTY INTERFACE_INCLUDE_DIRECTORIES SET )
      if( _incSet )
         get_target_property( _includes ${ATLAS_RELEASE_NAME}::${libName}
            INTERFACE_INCLUDE_DIRECTORIES )
         string( REPLACE "${ATLAS_RELEASE_NAME}::" "" _includes
            "${_includes}" )
         set_target_properties( ${libName} PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${_includes}" )
         unset( _includes )
      endif()
      unset( _incSet )
      get_property( _defSet TARGET ${ATLAS_RELEASE_NAME}::${libName}
         PROPERTY INTERFACE_COMPILE_DEFINITIONS SET )
      if( _defSet )
         get_target_property( _definitions ${ATLAS_RELEASE_NAME}::${libName}
            INTERFACE_COMPILE_DEFINITIONS )
         set_target_properties( ${libName} PROPERTIES
            INTERFACE_COMPILE_DEFINITIONS "${_definitions}" )
         unset( _definitions )
      endif()
      unset( _defSet )
      # The rest is only applicable to "real" libraries:
      if( "${_type}" STREQUAL "SHARED_LIBRARY" OR
            "${_type}" STREQUAL "MODULE_LIBRARY" )
         # Check which builds it is available for:
         get_property( _builds TARGET ${ATLAS_RELEASE_NAME}::${libName}
            PROPERTY IMPORTED_CONFIGURATIONS )
         set_property( TARGET ${libName} PROPERTY
            IMPORTED_CONFIGURATIONS "${_builds}" )
         # For each of the available builds, set the properties of the target:
         foreach( _build ${_builds} )
            # Get the properties of the auto-generated, imported library:
            get_target_property( _location ${ATLAS_RELEASE_NAME}::${libName}
               IMPORTED_LOCATION_${_build} )
            get_target_property( _soname ${ATLAS_RELEASE_NAME}::${libName}
               IMPORTED_SONAME_${_build} )
            # Set the same properties on this hand-made, imported library:
            set_target_properties( ${libName} PROPERTIES
               IMPORTED_LOCATION_${_build} "${_location}"
               IMPORTED_SONAME_${_build} "${_soname}" )
            unset( _location )
            unset( _soname )
            # Copy the private library dependencies:
            get_property( _depLibSet TARGET ${ATLAS_RELEASE_NAME}::${libName}
               PROPERTY IMPORTED_LINK_DEPENDENT_LIBRARIES_${_build} SET )
            if( _depLibSet )
               get_target_property( _depLib
                  ${ATLAS_RELEASE_NAME}::${libName}
                  IMPORTED_LINK_DEPENDENT_LIBRARIES_${_build} )
               string( REPLACE "${ATLAS_RELEASE_NAME}::" ""
                  _depLib "${_depLib}" )
               set_target_properties( ${libName} PROPERTIES
                  IMPORTED_LINK_DEPENDENT_LIBRARIES_${_build}
                  "${_depLib}" )
               unset( _depLib )
            endif()
            unset( _depLibSet )
         endforeach()
         unset( _builds )
      endif()
      return()
   endif()

   # A sanity check:
   if( NOT ARG_NO_PUBLIC_HEADERS AND NOT ARG_PUBLIC_HEADERS )
      message( WARNING "Package ${pkgName} doesn't declare public headers for "
         "library ${libName}" )
   endif()

   # Make sure that only one of the library types are specified:
   if( ( ARG_INTERFACE AND ( ARG_SHARED OR ARG_MODULE ) ) OR
         ( ARG_SHARED AND ( ARG_INTERFACE OR ARG_MODULE ) ) OR
         ( ARG_MODULE AND ( ARG_INTERFACE OR ARG_SHARED ) ) )
      message( SEND_ERROR "Only one of INTERFACE, SHARED or MODULE may be "
         "specified on a given library" )
      return()
   endif()

   # Set common compiler options:
   atlas_set_compiler_flags()

   # Collect the source file names:
   set( _sources )
   foreach( _source ${ARG_UNPARSED_ARGUMENTS} )
      if( IS_ABSOLUTE ${_source} )
         list( APPEND _sources ${_source} )
      else()
         file( GLOB _files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
            ${_source} )
         foreach( _file ${_files} )
            list( APPEND _sources ${_file} )
         endforeach()
      endif()
   endforeach()

   # Put the files into source groups. So they would show up in a ~reasonable
   # way in an IDE like Xcode:
   atlas_group_source_files( ${_sources} )

   # Declare the library to CMake:
   if( _sources AND NOT ARG_INTERFACE )
      if( ARG_MODULE )
         add_library( ${libName} MODULE ${_sources} )
      else()
         add_library( ${libName} SHARED ${_sources} )
      endif()
   else()
      add_library( ${libName} INTERFACE )
   endif()

#   # Look up the backwards compatibility header:
#   find_file( _compHeader NAMES GaudiBackwardsCompatibility.h
#      PATHS ${CMAKE_MODULE_PATH} PATH_SUFFIXES skeletons )
#   mark_as_advanced( _compHeader )
#
#   # Use the backwars compatibility header with the _entries.cxx files:
#   foreach( _source ${_sources} )
#      if( "${_source}" MATCHES "_entries.cxx" )
#         set_source_files_properties( ${_source} PROPERTIES
#            COMPILE_FLAGS "-include ${_compHeader}" )
#      endif()
#   endforeach()

   # Now set all of its properties:
   if( _sources AND NOT ARG_INTERFACE )
      add_dependencies( ${libName} ${pkgName}Pkg )
   endif()
   add_dependencies( Package_${pkgName} ${libName} )
   if( _sources AND NOT ARG_INTERFACE )
      set_property( TARGET ${libName} PROPERTY LABELS ${pkgName} )
      set_property( TARGET ${libName} PROPERTY FOLDER ${pkgDir} )
   endif()
   if( ARG_LINK_LIBRARIES )
      list( REMOVE_DUPLICATES ARG_LINK_LIBRARIES )
      if( _sources AND NOT ARG_INTERFACE )
         target_link_libraries( ${libName} PUBLIC ${ARG_LINK_LIBRARIES} )
      else()
         target_link_libraries( ${libName} INTERFACE ${ARG_LINK_LIBRARIES} )
      endif()
   endif()
   if( ARG_PRIVATE_LINK_LIBRARIES )
      list( REMOVE_DUPLICATES ARG_PRIVATE_LINK_LIBRARIES )
      if( _sources AND NOT ARG_INTERFACE )
         target_link_libraries( ${libName} PRIVATE
            ${ARG_PRIVATE_LINK_LIBRARIES} )
      else()
         message( WARNING "Private link libraries are not meaningful for "
            "interface libraries" )
      endif()
   endif()
   if( ARG_DEFINITIONS )
      if( _sources AND NOT ARG_INTERFACE )
         target_compile_definitions( ${libName} PUBLIC ${ARG_DEFINITIONS} )
      else()
         target_compile_definitions( ${libName} INTERFACE ${ARG_DEFINITIONS} )
      endif()
   endif()
   if( ARG_PRIVATE_DEFINITIONS )
      if( _sources AND NOT ARG_INTERFACE )
         target_compile_definitions( ${libName} PRIVATE
            ${ARG_PRIVATE_DEFINITIONS} )
      else()
         message( WARNING "Private definitions are not meaningful for "
            "interface libraries" )
      endif()
   endif()
   if( APPLE AND _sources AND NOT ARG_INTERFACE )
      # To enable soft linking of some Athena symbols in trigger configuration
      # packages:
      set_target_properties( ${libName} PROPERTIES LINK_FLAGS
         "-undefined dynamic_lookup" )
   endif()
   if( _sources AND NOT ARG_INTERFACE )
      target_link_libraries( ${libName} PRIVATE dl )
      if( ARG_NO_PUBLIC_HEADERS )
         target_include_directories( ${libName} SYSTEM BEFORE PRIVATE
            ${ARG_INCLUDE_DIRS} ${ARG_PRIVATE_INCLUDE_DIRS} )
         target_include_directories( ${libName} BEFORE PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<TARGET_PROPERTY:${pkgName}Pkg,INTERFACE_INCLUDE_DIRECTORIES>
            $<TARGET_PROPERTY:${pkgName}PkgPrivate,INTERFACE_INCLUDE_DIRECTORIES> )
      else()
         target_include_directories( ${libName} SYSTEM BEFORE PUBLIC
            ${ARG_INCLUDE_DIRS}
            PRIVATE
            ${ARG_PRIVATE_INCLUDE_DIRS} )
         target_include_directories( ${libName} BEFORE PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<TARGET_PROPERTY:${pkgName}Pkg,INTERFACE_INCLUDE_DIRECTORIES>
            PRIVATE
            $<TARGET_PROPERTY:${pkgName}PkgPrivate,INTERFACE_INCLUDE_DIRECTORIES> )
      endif()
   else()
      target_include_directories( ${libName} SYSTEM BEFORE INTERFACE
         ${ARG_INCLUDE_DIRS} )
      target_include_directories( ${libName} BEFORE INTERFACE
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
         $<TARGET_PROPERTY:${pkgName}Pkg,INTERFACE_INCLUDE_DIRECTORIES> )
   endif()

   # Install the header files of the library if they exist:
   if( ARG_PUBLIC_HEADERS )
      atlas_install_headers( ${ARG_PUBLIC_HEADERS} )
   endif()

   # In case we are building optimised libraries with debug info, and we have
   # objcopy available, detach the debug information into a separate library
   # file.
   if( "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" AND CMAKE_OBJCOPY
         AND _sources AND NOT ARG_INTERFACE )
      if( ARG_MODULE )
         set( _libFileName "${CMAKE_SHARED_MODULE_PREFIX}${libName}" )
         set( _libFileName "${_libFileName}${CMAKE_SHARED_MODULE_SUFFIX}" )
      else()
         set( _libFileName "${CMAKE_SHARED_LIBRARY_PREFIX}${libName}" )
         set( _libFileName "${_libFileName}${CMAKE_SHARED_LIBRARY_SUFFIX}" )
      endif()
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

   # Declare how to install the library, and its associated files.
   if( ATLAS_RELEASE_MODE )
      install( TARGETS ${libName}
         DESTINATION ${CMAKE_INSTALL_LIBDIR}
         OPTIONAL )
   else()
      install( TARGETS ${libName}
         EXPORT ${CMAKE_PROJECT_NAME}Targets
         DESTINATION ${CMAKE_INSTALL_LIBDIR}
         OPTIONAL )
      set_property( GLOBAL PROPERTY ATLAS_EXPORTS ON )
      set_property( GLOBAL APPEND PROPERTY ATLAS_EXPORTED_TARGETS ${libName} )
   endif()

endfunction( atlas_add_library )

# This function takes care of building a component library for Athena/Gaudi.
# First it builds a library in a fairly ordinary way, and then builds
# configurables for it.
#
# Usage: atlas_add_component( libraryName source1.cxx source2.cxx...
#                             [NOCLIDDB]
#                             [GENCONF_PRELOAD preload]
#                             [INCLUDE_DIRS Include1...]
#                             [LINK_LIBRARIES Library1 Library2...] )
#
function( atlas_add_component libName )

   # In release recompilation dryrun mode exit now:
   if( ATLAS_RELEASE_RECOMPILE_DRYRUN )
      return()
   endif()

   # Parse the arguments:
   cmake_parse_arguments( ARG "NOCLIDDB" "GENCONF_PRELOAD"
      "LINK_LIBRARIES" ${ARGN} )

   # We need Gaudi for this:
   find_package( Gaudi REQUIRED )

   # Get this package's name:
   atlas_get_package_name( pkgName )

   # Get the package's directory:
   atlas_get_package_dir( pkgDir )

   # Build a library using the athena_add_library function:
   atlas_add_library( ${libName} ${ARG_UNPARSED_ARGUMENTS}
      NO_PUBLIC_HEADERS MODULE
      LINK_LIBRARIES ${ARG_LINK_LIBRARIES} GaudiPluginService )

   # If we are in "release mode" and the library doesn't need to be
   # recompiled, then let's not bother with the rest.
   if( ATLAS_RELEASE_MODE AND NOT ATLAS_PACKAGE_RECOMPILE )
      return()
   endif()

   # Generate a .components file from the library:
   atlas_generate_componentslist( ${libName} )

   # Generate configurables for the library:
   set( _configurable
      ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/${pkgName}/${libName}Conf.py )
   set( _initPy
      ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/${pkgName}/__init__.py )
   set( _confdbFile "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/" )
   set( _confdbFile "${_confdbFile}${CMAKE_SHARED_MODULE_PREFIX}" )
   set( _confdbFile "${_confdbFile}${libName}.confdb" )
   set( _fullLibName "${CMAKE_SHARED_MODULE_PREFIX}${libName}" )
   set( _fullLibName "${_fullLibName}${CMAKE_SHARED_MODULE_SUFFIX}" )
   set( _outdir ${CMAKE_CURRENT_BINARY_DIR}/genConf )
   get_target_property( _genconf_cmd genconf IMPORTED_LOCATION )
   add_custom_command( OUTPUT ${_configurable} ${_confdbFile}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${_outdir}
      COMMAND ${CMAKE_BINARY_DIR}/atlas_build_run.sh
      ${_genconf_cmd} -o ${_outdir} -p ${pkgName} --no-init
      -i ${_fullLibName}
      COMMAND ${CMAKE_COMMAND} -E copy ${_outdir}/${libName}.confdb
      ${_confdbFile}
      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/${pkgName}
      COMMAND ${CMAKE_COMMAND} -E copy ${_outdir}/${libName}Conf.py
      ${_configurable}
      # This is a tricky one. The __init__.py file may actually not
      # be writable. Since it may have been linked in from the base
      # release. So just swallow the output code of this command with
      # this trick found online:
      COMMAND ${CMAKE_COMMAND} -E touch ${_initPy} || :
      DEPENDS ${libName} )
   set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} APPEND PROPERTY
      ADDITIONAL_MAKE_CLEAN_FILES ${_initPy} )
   add_custom_target( ${libName}Configurables ALL
      DEPENDS ${_configurable} ${_confdbFile} )
   set_property( TARGET ${libName}Configurables PROPERTY FOLDER ${pkgDir} )
   add_dependencies( Package_${pkgName} ${libName}Configurables )
   install( FILES ${_configurable}
      DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/${pkgName}
      OPTIONAL )
   install( CODE "execute_process( COMMAND \${CMAKE_COMMAND} -E touch
      \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_PYTHONDIR}/${pkgName}/__init__.py )" )

   # Set up the merging of the confdb files:
   set_property( GLOBAL APPEND PROPERTY ATLAS_CONFDB_FILES ${_confdbFile} )
   set_property( GLOBAL APPEND PROPERTY ATLAS_CONFDB_TARGETS
      ${libName}Configurables )

   # Generate a CLID DB file:
   if( NOT ARG_NOCLIDDB )
      atlas_generate_cliddb( ${libName} )
   endif()

endfunction( atlas_add_component )

# This function generates a T/P converter library.
#
# T/P converter libraries are just simple (installed) libraries, with component
# lists getting generated for them. So this is mostly just a convenience
# function, calling on other functions to do the heavy lifting.
#
# Usage: atlas_add_tpcnv_library( libraryName source1.cxx source2.cxx...
#                                 PUBLIC_HEADERS HeaderDir...
#                                 [INCLUDE_DIRS Include1...]
#                                 [LINK_LIBRARIES Library1 Library2...] )
#
function( atlas_add_tpcnv_library libName )

   # Build the library:
   atlas_add_library( ${libName} ${ARGN} )

   # Generate a .components file for it:
   atlas_generate_componentslist( ${libName} )

endfunction( atlas_add_tpcnv_library )

# Generic function that creates both poolcnv and sercnv libraries during
# the build. Should not be used directly, but through one of the following
# functions:
#  - atlas_add_poolcnv_library
#  - atlas_add_sercnv_library
#
# The applicable parameters are defined for those functions.
#
function( _atlas_add_cnv_library libName )

   # In release recompilation dryrun mode exit now:
   if( ATLAS_RELEASE_RECOMPILE_DRYRUN )
      return()
   endif()

   # If we are in "release mode" and the package doesn't need to be
   # recompiled, then let's not bother with the rest.
   if( ATLAS_RELEASE_MODE AND NOT ATLAS_PACKAGE_RECOMPILE )
      return()
   endif()

   # We need Gaudi for this:
   find_package( Gaudi REQUIRED )

   # Parse the arguments:
   set( _singleParamArgs CNV_PFX SKELETON_PREFIX CONVERTER_PREFIX )
   set( _multiParamArgs FILES INCLUDE_DIRS LINK_LIBRARIES TYPES_WITH_NAMESPACE
      MULT_CHAN_TYPES )
   cmake_parse_arguments( ARG "" "${_singleParamArgs}" "${_multiParamArgs}"
      ${ARGN} )
   unset( _singleParamArgs )
   unset( _multiParamArgs )

   # Get this package's name:
   atlas_get_package_name( pkgName )
   # Get this package's directory:
   atlas_get_package_dir( pkgDir )

   # Find the types that we need to make converter(s) for:
   set( _typeCache )
   foreach( _file ${ARG_FILES} )
      # Get the type name from the file name:
      get_filename_component( _type ${_file} NAME_WE )
      # Set the fully qualified type name like this as a start:
      set( _fullTypeName ${_type} )
      # Now check in the TYPES_WITH_NAMESPACE argument whether this type
      # has a namespace in front of it:
      set( _namespace " " )
      foreach( _typeName ${ARG_TYPES_WITH_NAMESPACE} )
         # Extract the name and namespace out of the full type name:
         if( "${_typeName}" MATCHES "^(.*)::([^:]+)$" )
            # And check if they match:
            if( "${CMAKE_MATCH_2}" STREQUAL "${_type}" )
               set( _namespace "${CMAKE_MATCH_1}" )
               set( _fullTypeName "${_typeName}" )
               break()
            endif()
         else()
            message( WARNING "Name specified for TYPES_WITH_NAMESPACE ("
               "${_typeName}) is not namespaced" )
            continue()
         endif()
      endforeach()
      # Now check if it's a multi channel type or not:
      set( _isMultiChan FALSE )
      foreach( _multi ${ARG_MULT_CHAN_TYPES} )
         if( "${_multi}" STREQUAL "${_fullTypeName}" )
            set( _isMultiChan TRUE )
            break()
         endif()
      endforeach()
      # Now store the results:
      list( APPEND _typeCache ${_type} ${_namespace} ${_file} ${_isMultiChan} )
   endforeach()
   # Clean up:
   unset( _type )
   unset( _fullTypeName )
   unset( _namespace )
   unset( _typeName )
   unset( _isMultiChan )

   # Find the merge command:
   find_program( _mergeFilesCmd mergeFiles.sh
      PATH_SUFFIXES scripts PATHS ${CMAKE_MODULE_PATH} )

   # Find the skeleton files:
   find_file( _${ARG_SKELETON_PREFIX}CnvLoad ${ARG_SKELETON_PREFIX}Cnv_load.cxx.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   find_file( _${ARG_SKELETON_PREFIX}CnvEntriesHeader
      ${ARG_SKELETON_PREFIX}Cnv_entries.h.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   find_file( _${ARG_SKELETON_PREFIX}CnvEntriesImpl
      ${ARG_SKELETON_PREFIX}Cnv_entries.cxx.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   find_file( _${ARG_SKELETON_PREFIX}CnvSimple ${ARG_SKELETON_PREFIX}CnvSimple.h.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   find_file( _${ARG_SKELETON_PREFIX}CnvMultiChan
      ${ARG_SKELETON_PREFIX}CnvMultiChan.h.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   find_file( _${ARG_SKELETON_PREFIX}CnvItemListHeader
      ${ARG_SKELETON_PREFIX}CnvItemList_joboptionsHdr.py.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   find_file( _${ARG_SKELETON_PREFIX}CnvItemListItem
      ${ARG_SKELETON_PREFIX}CnvItemList_joboptions.py.in
      PATH_SUFFIXES skeletons PATHS ${CMAKE_MODULE_PATH} )
   mark_as_advanced( _${ARG_SKELETON_PREFIX}CnvLoad
      _${ARG_SKELETON_PREFIX}CnvEntriesHeader
      _${ARG_SKELETON_PREFIX}CnvEntriesImpl
      _${ARG_SKELETON_PREFIX}CnvSimple
      _${ARG_SKELETON_PREFIX}CnvMultiChan
      _${ARG_SKELETON_PREFIX}CnvItemListHeader
      _${ARG_SKELETON_PREFIX}CnvItemListItem )

   # Generate the headers of the files for the component generation:
   configure_file( ${_${ARG_SKELETON_PREFIX}CnvLoad}
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_load.cxx
      @ONLY )
   configure_file( ${_${ARG_SKELETON_PREFIX}CnvEntriesHeader}
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_entries_header.h
      @ONLY )
   configure_file( ${_${ARG_SKELETON_PREFIX}CnvEntriesImpl}
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_entries_header.cxx
      @ONLY )

   # Now generate the files that will extend these skeletons:
   set( _entriesHeaderComponents )
   set( _entriesImplComponents )
   set( _typeCacheCopy ${_typeCache} )
   while( _typeCacheCopy )
      # The type and header name:
      list( GET _typeCacheCopy 0 _type )
      list( REMOVE_AT _typeCacheCopy 0 1 2 3 )
      # Describe how to create the files that will be appended to the
      # headers:
      add_custom_command( OUTPUT
         ${CMAKE_CURRENT_BINARY_DIR}/${libName}_${_type}_entries.h
         COMMAND ${CMAKE_COMMAND} -E echo
         "#include \"${ARG_CNV_PFX}${_type}${ARG_CONVERTER_PREFIX}Cnv.h\"" >
         ${CMAKE_CURRENT_BINARY_DIR}/${libName}_${_type}_entries.h
         COMMAND ${CMAKE_COMMAND} -E echo
         "DECLARE_CONVERTER_FACTORY( ${ARG_CNV_PFX}${_type}${ARG_CONVERTER_PREFIX}Cnv )" >>
         ${CMAKE_CURRENT_BINARY_DIR}/${libName}_${_type}_entries.h
         VERBATIM )
      add_custom_command( OUTPUT
         ${CMAKE_CURRENT_BINARY_DIR}/${libName}_${_type}_entries.cxx
         COMMAND ${CMAKE_COMMAND} -E echo
         "DECLARE_CONVERTER( ${ARG_CNV_PFX}${_type}${ARG_CONVERTER_PREFIX}Cnv )" >
         ${CMAKE_CURRENT_BINARY_DIR}/${libName}_${_type}_entries.cxx
         VERBATIM )
      # Remember the names of these files:
      list( APPEND _entriesHeaderComponents
         ${CMAKE_CURRENT_BINARY_DIR}/${libName}_${_type}_entries.h )
      list( APPEND _entriesImplComponents
         ${CMAKE_CURRENT_BINARY_DIR}/${libName}_${_type}_entries.cxx )
   endwhile()

   # Generate a footer for the implementation file:
   add_custom_command( OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_entries_footer.cxx
      COMMAND ${CMAKE_COMMAND} -E echo "}" >
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_entries_footer.cxx )

   # Declare the rules merging these files:
   add_custom_command( OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_entries.h
      COMMAND ${_mergeFilesCmd}
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_entries.h
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_entries_header.h
      ${_entriesHeaderComponents}
      DEPENDS
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_entries_header.h
      ${_entriesHeaderComponents} )
   add_custom_command( OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_entries.cxx
      COMMAND ${_mergeFilesCmd}
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_entries.cxx
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_entries_header.cxx
      ${_entriesImplComponents}
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_entries_footer.cxx
      DEPENDS
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_entries_header.cxx
      ${_entriesImplComponents}
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_entries_footer.cxx
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_entries.h )

   # Create a header for the ItemList file:
   if( _${ARG_SKELETON_PREFIX}CnvItemListHeader )
      configure_file( ${_${ARG_SKELETON_PREFIX}CnvItemListHeader}
         ${CMAKE_CURRENT_BINARY_DIR}/${libName}ItemList_joboptionsHdr.py
         @ONLY )
      set( _itemListFiles
         ${CMAKE_CURRENT_BINARY_DIR}/${libName}ItemList_joboptionsHdr.py )
   endif()

   # Find/generate the converters:
   set( _sources )
   set( _typeCacheCopy ${_typeCache} )
   while( _typeCacheCopy )
      # The type and header name:
      list( GET _typeCacheCopy 0 type )
      list( GET _typeCacheCopy 1 namespace )
      list( GET _typeCacheCopy 2 header )
      list( GET _typeCacheCopy 3 isMultiChan )
      list( REMOVE_AT _typeCacheCopy 0 1 2 3 )
      # Check if there are custom sources for this converter:
      if( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/${ARG_CNV_PFX}${type}${ARG_CONVERTER_PREFIX}Cnv.cxx )
         list( APPEND _sources
            ${CMAKE_CURRENT_SOURCE_DIR}/src/${ARG_CNV_PFX}${type}${ARG_CONVERTER_PREFIX}Cnv.cxx )
      else()
         # No such converter is available, let's generate it:
         if( ${isMultiChan} )
            configure_file( ${_${ARG_SKELETON_PREFIX}CnvMultiChan}
               ${CMAKE_CURRENT_BINARY_DIR}/${ARG_CNV_PFX}${type}${ARG_CONVERTER_PREFIX}Cnv.h @ONLY )
         else()
            configure_file( ${_${ARG_SKELETON_PREFIX}CnvSimple}
               ${CMAKE_CURRENT_BINARY_DIR}/${ARG_CNV_PFX}${type}${ARG_CONVERTER_PREFIX}Cnv.h @ONLY )
         endif()
      endif()
      # Generate the ItemList file for this type:
      if( "${namespace}" STREQUAL " " )
         set( fullTypeName "${type}" )
      else()
         set( fullTypeName "${namespace}::${type}" )
      endif()
      if( _${ARG_SKELETON_PREFIX}CnvItemListItem )
         configure_file( ${_${ARG_SKELETON_PREFIX}CnvItemListItem}
            ${CMAKE_CURRENT_BINARY_DIR}/${pkgName}_${ARG_CNV_PFX}${type}_joboptions.py
            @ONLY )
         list( APPEND _itemListFiles
            ${CMAKE_CURRENT_BINARY_DIR}/${pkgName}_${ARG_CNV_PFX}${type}_joboptions.py )
      endif()
   endwhile()

   # Python code generated for the converter:
   set( _pythonFiles )

   # Merge the jobOptions files:
   if( _itemListFiles )
      add_custom_command( OUTPUT
         ${CMAKE_JOBOPT_OUTPUT_DIRECTORY}/${pkgName}/${pkgName}ItemList_joboptions.py
         COMMAND ${CMAKE_COMMAND} -E make_directory
         ${CMAKE_JOBOPT_OUTPUT_DIRECTORY}/${pkgName}
         COMMAND ${_mergeFilesCmd}
         ${CMAKE_JOBOPT_OUTPUT_DIRECTORY}/${pkgName}/${pkgName}ItemList_joboptions.py
         ${_itemListFiles} )
      install( FILES
         ${CMAKE_JOBOPT_OUTPUT_DIRECTORY}/${pkgName}/${pkgName}ItemList_joboptions.py
         DESTINATION ${CMAKE_INSTALL_JOBOPTDIR}/${pkgName} OPTIONAL )
      list( APPEND _pythonFiles
         ${CMAKE_JOBOPT_OUTPUT_DIRECTORY}/${pkgName}/${pkgName}ItemList_joboptions.py )
   endif()

   # Generate a dummy main jobO file:
   if( "${ARG_SKELETON_PREFIX}" STREQUAL "Pool" )
      add_custom_command( OUTPUT
         ${CMAKE_JOBOPT_OUTPUT_DIRECTORY}/${pkgName}/${pkgName}_joboptions.py
         COMMAND ${CMAKE_COMMAND} -E make_directory
         ${CMAKE_JOBOPT_OUTPUT_DIRECTORY}/${pkgName}
         COMMAND ${CMAKE_COMMAND} -E echo "# Dummy file" >
         ${CMAKE_JOBOPT_OUTPUT_DIRECTORY}/${pkgName}/${pkgName}_joboptions.py
         VERBATIM )
      install( FILES
         ${CMAKE_JOBOPT_OUTPUT_DIRECTORY}/${pkgName}/${pkgName}_joboptions.py
         DESTINATION ${CMAKE_INSTALL_JOBOPTDIR}/${pkgName} OPTIONAL )
      list( APPEND _pythonFiles
         ${CMAKE_JOBOPT_OUTPUT_DIRECTORY}/${pkgName}/${pkgName}_joboptions.py )
   endif()

   # Make sure that the python files are made:
   if( _pythonFiles )
      add_custom_target( ${pkgName}JobOptGen ALL SOURCES ${_pythonFiles} )
      add_dependencies( Package_${pkgName} ${pkgName}JobOptGen )
      set_property( TARGET ${pkgName}JobOptGen PROPERTY LABELS ${pkgName} )
      set_property( TARGET ${pkgName}JobOptGen PROPERTY FOLDER ${pkgDir} )
   endif()

   # Compile a library with the collected/generated sources:
   atlas_add_library( ${libName} ${_sources}
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_load.cxx
      ${CMAKE_CURRENT_BINARY_DIR}/${libName}_entries.cxx
      ${ARG_UNPARSED_ARGUMENTS}
      NO_PUBLIC_HEADERS MODULE
      INCLUDE_DIRS ${ARG_INCLUDE_DIRS}
      LINK_LIBRARIES ${ARG_LINK_LIBRARIES} GaudiKernel )
   target_include_directories( ${libName} PRIVATE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> )

   # Generate a .components file from the library:
   atlas_generate_componentslist( ${libName} )

endfunction( _atlas_add_cnv_library )

# This function is used to generate POOL converter libraries in the offline
# build.
#
# Usage: atlas_add_poolcnv_library( EventInfoAthenaPool source1.cxx...
#           FILES xAODEventInfo/EventInfo.h...
#           [INCLUDE_DIRS Include1...]
#           [LINK_LIBRARIES Library1...]
#           [TYPES_WITH_NAMESPACE xAOD::EventInfo...]
#           [MULT_CHAN_TYPES My::SuperType...]
#           [CNV_PFX xAOD] )
#
function( atlas_add_poolcnv_library libName )

   # Call the generic function with the right arguments:
   _atlas_add_cnv_library( ${libName} ${ARGN} SKELETON_PREFIX Pool )

endfunction( atlas_add_poolcnv_library )

# This function is used to generate serialiser converter libraries in the
# offline build.
#
# Usage: atlas_add_sercnv_library( TrigMuonEventSerCnv source1.cxx...
#           FILES TrigMuonEvent/MuonFeature.h...
#           [INCLUDE_DIRS Include1...]
#           [LINK_LIBRARIES Library1...]
#           [TYPES_WITH_NAMESPACE xAOD::MuonContainer...]
#           [CNV_PFX xAOD] )
#
function( atlas_add_sercnv_library libName )

   # Call the generic function with the right arguments:
   _atlas_add_cnv_library( ${libName} ${ARGN} SKELETON_PREFIX Ser
      CONVERTER_PREFIX Ser )

endfunction( atlas_add_sercnv_library )

# Hide the internal function from outside users:
unset( _atlas_add_cnv_library )

# Function taking care of generating .components files from the built libraries.
# Which are in the end used by Athena to find components.
#
# Usage: atlas_generate_componentslist( MyLibrary )
#
function( atlas_generate_componentslist libName )

   # In release recompilation dryrun mode exit now:
   if( ATLAS_RELEASE_RECOMPILE_DRYRUN )
      return()
   endif()

   # If we are in "release mode" and the package doesn't need to be
   # recompiled, then let's not bother with the rest.
   if( ATLAS_RELEASE_MODE AND NOT ATLAS_PACKAGE_RECOMPILE )
      return()
   endif()

   # We need Gaudi for this:
   find_package( Gaudi REQUIRED )

   # Get this package's name:
   atlas_get_package_name( pkgName )

   # Get the package's directory:
   atlas_get_package_dir( pkgDir )

   # Generate a .components file from the library:
   set( _componentsfile "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/" )
   set( _componentsfile "${_componentsfile}${CMAKE_SHARED_MODULE_PREFIX}" )
   set( _componentsfile "${_componentsfile}${libName}.components" )
   set( _fullLibName "${CMAKE_SHARED_MODULE_PREFIX}${libName}" )
   set( _fullLibName "${_fullLibName}${CMAKE_SHARED_MODULE_SUFFIX}" )
   get_target_property( _listcomponents_cmd listcomponents IMPORTED_LOCATION )
   add_custom_command( OUTPUT ${_componentsfile}
      COMMAND ${CMAKE_BINARY_DIR}/atlas_build_run.sh
      ${_listcomponents_cmd} --output ${_componentsfile} ${_fullLibName}
      DEPENDS ${libName} )
   add_custom_target( ${libName}ComponentsList ALL DEPENDS
      ${_componentsfile} ${_confdbfile} )
   add_dependencies( Package_${pkgName} ${libName}ComponentsList )
   set_property( TARGET ${libName}ComponentsList PROPERTY FOLDER ${pkgDir} )

   # Set up the merging of the components files:
   set_property( GLOBAL APPEND PROPERTY ATLAS_COMPONENTS_FILES
      ${_componentsfile} )
   set_property( GLOBAL APPEND PROPERTY ATLAS_COMPONENTS_TARGETS
      ${libName}ComponentsList )

endfunction( atlas_generate_componentslist )

# Function generating CLID files for the libraries. It takes a library compiled
# in some way (it doesn't necessarily have to be a component library), and
# generates a CLID DB file from it.
#
# Usage: atlas_generate_cliddb( MyLibrary )
#
function( atlas_generate_cliddb libName )

   # Get this package's name:
   atlas_get_package_name( pkgName )

   # Get the package's directory:
   atlas_get_package_dir( pkgDir )

   # The name of a possible input CLID file:
   set( _inputClidFile ${CMAKE_CURRENT_BINARY_DIR}/dummy_clid.db )
   # The name of the output CLID file:
   set( _clidFile ${CMAKE_CURRENT_BINARY_DIR}/${libName}_clid.db )

   # Generate the file:
   add_custom_command( OUTPUT ${_clidFile}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SHARE_OUTPUT_DIRECTORY}
      COMMAND ${CMAKE_COMMAND} -E touch ${_inputClidFile}
      COMMAND ${CMAKE_BINARY_DIR}/atlas_build_run.sh
      genCLIDDB -p ${libName} -i ${_inputClidFile} -o ${_clidFile}
      DEPENDS genCLIDDB CLIDComps ${libName} )

   # Create a target generating the file:
   add_custom_target( ${libName}ClidGen ALL DEPENDS ${_clidFile} )
   add_dependencies( Package_${pkgName} ${libName}ClidGen )
   set_property( TARGET ${libName}ClidGen PROPERTY FOLDER ${pkgDir} )

   # Set up the merging of the CLID files:
   set_property( GLOBAL APPEND PROPERTY ATLAS_CLID_FILES ${_clidFile} )
   set_property( GLOBAL APPEND PROPERTY ATLAS_CLID_TARGETS ${libName}ClidGen )

endfunction( atlas_generate_cliddb )
