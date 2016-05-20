# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: AtlasInstallFunctions.cmake 732980 2016-03-30 16:06:43Z krasznaa $
#
# This file collects the ATLAS CMake helper functions that are used to install
# various types of files/directories from a package.
#
# This file should not be included directly, but through AtlasFunctions.cmake.
#

# This function installs the headers from one or many of the package's
# directories.
#
# Usage: atlas_install_headers( CxxUtils )
#
function( atlas_install_headers )

   # This rule is ignored in release mode:
   if( ATLAS_RELEASE_MODE )
      return()
   endif()

   # Parse the options (header directories) given to the function:
   cmake_parse_arguments( ARG "" "" "" ${ARGN} )

   # If there are no arguments given to the function, return now:
   if( NOT ARG_UNPARSED_ARGUMENTS )
      message( WARNING "Function received no arguments" )
      return()
   endif()

   # Get the package/subdirectory name:
   atlas_get_package_name( pkgName )

   # Get the package directory:
   atlas_get_package_dir( pkgDir )

   # Create the header installation target of the package if it doesn't exist
   # yet:
   if( NOT TARGET ${pkgName}HeaderInstall )
      add_custom_target( ${pkgName}HeaderInstall ALL SOURCES
         $<TARGET_PROPERTY:${pkgName}HeaderInstall,ROOT_HEADER_DIRS> )
      add_dependencies( Package_${pkgName} ${pkgName}HeaderInstall )
      set_property( TARGET ${pkgName}HeaderInstall PROPERTY LABELS ${pkgName} )
      set_property( TARGET ${pkgName}HeaderInstall PROPERTY FOLDER ${pkgDir} )
   endif()

   # Create an installation rule setting up the include directory in the
   # install area:
   install( CODE "execute_process( COMMAND \${CMAKE_COMMAND}
      -E make_directory
      \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/include )" )

   # Loop over the specified directories:
   foreach( dir ${ARG_UNPARSED_ARGUMENTS} )
      # Set up the installation of the header directory into the build area:
      file( RELATIVE_PATH _target ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}
         ${CMAKE_CURRENT_SOURCE_DIR}/${dir} )
      add_custom_command( OUTPUT ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/${dir}
         COMMAND ${CMAKE_COMMAND} -E make_directory
         ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}
         COMMAND ${CMAKE_COMMAND} -E create_symlink ${_target}
         ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/${dir} )
      # Clean up on "make clean":
      set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} APPEND PROPERTY
         ADDITIONAL_MAKE_CLEAN_FILES
         ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/${dir} )
      # Add it to the header installation target:
      set_property( TARGET ${pkgName}HeaderInstall APPEND PROPERTY
         ROOT_HEADER_DIRS ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/${dir} )
      # Set up the installation of this folder:
      install( CODE "execute_process( COMMAND \${CMAKE_COMMAND}
         -E create_symlink ../src/${pkgDir}/${dir}
         \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/include/${dir} )" )
   endforeach()

endfunction( atlas_install_headers )

# Helper macro setting up installation targets. Not for use outside of this
# file.
#
macro( _atlas_create_install_target tgtName )

   if( NOT TARGET ${tgtName} )
      add_custom_target( ${tgtName} ALL SOURCES
         $<TARGET_PROPERTY:${tgtName},INSTALLED_FILES> )
   endif()

endmacro( _atlas_create_install_target )

# This is a generic function for installing practically any type of file
# from a package into both the build and the install areas. Behind the scenes
# it is used by most of the functions of this file.
#
# Usage: atlas_install_generic( dir/file1 dir/dir2...
#                               DESTINATION dir
#                               [BUILD_DESTINATION dir]
#                               [TYPENAME type]
#                               [EXECUTABLE]
#                               [PKGNAME_SUBDIR] )
#
function( atlas_install_generic )

   # In release recompilation dryrun mode exit now:
   if( ATLAS_RELEASE_RECOMPILE_DRYRUN )
      return()
   endif()

   # Also exit if the package is not being recompiled:
   if( ATLAS_RELEASE_MODE AND NOT ATLAS_PACKAGE_RECOMPILE )
      return()
   endif()
   # But if it *is* being recompiled, then do the installation. This is mainly
   # to address packages building component libraries, and also having python
   # modules. In this case the python modules of the base release would become
   # invisible because of the configurable created in the test area, if the
   # modules are not installed into the test area as well.

   # Parse the options given to the function:
   cmake_parse_arguments( ARG "EXECUTABLE;PKGNAME_SUBDIR"
      "TYPENAME;DESTINATION;BUILD_DESTINATION" "" ${ARGN} )

   # If there are no file/directory names given to the function, return now:
   if( NOT ARG_UNPARSED_ARGUMENTS )
      message( WARNING "Function received no file/directory arguments" )
      return()
   endif()
   if( NOT ARG_DESTINATION )
      message( WARNING "No destination was specified" )
      return()
   endif()

   # Get the package/subdirectory name:
   atlas_get_package_name( pkgName )

   # Get the package directory:
   atlas_get_package_dir( pkgDir )

   # Create an installation target for the package, for this type:
   if( ARG_TYPENAME )
      set( _tgtName ${pkgName}${ARG_TYPENAME}Install )
   else()
      set( _tgtName ${pkgName}GenericInstall )
   endif()
   _atlas_create_install_target( ${_tgtName} )
   add_dependencies( Package_${pkgName} ${_tgtName} )
   set_property( TARGET ${_tgtName} PROPERTY LABELS ${pkgName} )
   set_property( TARGET ${_tgtName} PROPERTY FOLDER ${pkgDir} )

   # Expand possible wildcards:
   file( GLOB _files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
      ${ARG_UNPARSED_ARGUMENTS} )

   # Decide what the build area destination should be:
   if( ARG_BUILD_DESINTATION )
      if( ARG_PKGNAME_SUBDIR )
         set( _buildDest ${ARG_BUILD_DESTINATION}/${pkgName} )
      else()
         set( _buildDest ${ARG_BUILD_DESTINATION} )
      endif()
   else()
      if( ARG_PKGNAME_SUBDIR )
         set( _buildDest
            ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/${ARG_DESTINATION}/${pkgName} )
      else()
         set( _buildDest
            ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/${ARG_DESTINATION} )
      endif()
   endif()

   # Decide what the installation area destination should be:
   if( ARG_PKGNAME_SUBDIR )
      set( _installDest ${ARG_DESTINATION}/${pkgName} )
   else()
      set( _installDest ${ARG_DESTINATION} )
   endif()

   # Now loop over all file names:
   foreach( _file ${_files} )
      # Set up its installation into the build area:
      file( RELATIVE_PATH _target
         ${_buildDest} ${CMAKE_CURRENT_SOURCE_DIR}/${_file} )
      get_filename_component( _filename ${_file} NAME )
      add_custom_command( OUTPUT ${_buildDest}/${_filename}
         COMMAND ${CMAKE_COMMAND} -E make_directory ${_buildDest}
         COMMAND ${CMAKE_COMMAND} -E create_symlink ${_target}
         ${_buildDest}/${_filename} )
      set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} APPEND PROPERTY
         ADDITIONAL_MAKE_CLEAN_FILES
         ${_buildDest}/${_filename} )
      # Add it to the installation target:
      set_property( TARGET ${_tgtName} APPEND PROPERTY
         INSTALLED_FILES ${_buildDest}/${_filename} )
      # Set up its installation into the install area:
      if( IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${_file} )
         install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${_file}
            DESTINATION ${_installDest}
            USE_SOURCE_PERMISSIONS
            PATTERN ".svn" EXCLUDE )
      else()
         # In case this turns out to be a symbolic link, install the actual
         # file that it points to, using the name of the symlink.
         get_filename_component( _realpath
            ${CMAKE_CURRENT_SOURCE_DIR}/${_file} REALPATH )
         if( ARG_EXECUTABLE )
            install( PROGRAMS ${_realpath}
               DESTINATION ${_installDest}
               RENAME ${_filename} )
         else()
            install( FILES ${_realpath}
               DESTINATION ${_installDest}
               RENAME ${_filename} )
         endif()
      endif()
   endforeach()

endfunction( atlas_install_generic )

# This function installs python modules from the package into the
# right place in both the build and the install directories.
#
# Usage: atlas_install_python_modules( python/*.py )
#
function( atlas_install_python_modules )

   # Call the generic function:
   atlas_install_generic( ${ARGN}
      DESTINATION ${CMAKE_INSTALL_PYTHONDIR}
      BUILD_DESTINATION ${CMAKE_PYTHON_OUTPUT_DIRECTORY}
      TYPENAME Python
      PKGNAME_SUBDIR )

endfunction( atlas_install_python_modules )

# This function installs data files from the packages into the data directory
# of the build/install area. It can be given wildcarded file names, in which
# case the code will try to look up all the specified files.
#
# Usage: atlas_install_data( data/*.root )
#
function( atlas_install_data )

   # Call the generic function:
   atlas_install_generic( ${ARGN}
      DESTINATION ${CMAKE_INSTALL_DATADIR}
      BUILD_DESTINATION ${CMAKE_DATA_OUTPUT_DIRECTORY}
      TYPENAME Data
      PKGNAME_SUBDIR )

endfunction( atlas_install_data )

# This function installs jobOptions from the package into the
# right place in both the build and the install directories.
#
# Usage: atlas_install_joboptions( share/*.py )
#
function( atlas_install_joboptions )

   # Call the generic function:
   atlas_install_generic( ${ARGN}
      DESTINATION ${CMAKE_INSTALL_JOBOPTDIR}
      BUILD_DESTINATION ${CMAKE_JOBOPT_OUTPUT_DIRECTORY}
      TYPENAME JobOpt
      PKGNAME_SUBDIR )

endfunction( atlas_install_joboptions )

# This function installs documentation files from the package into the right
# place in both the build and install directories.
#
# Usage: atlas_install_docs( doc/*.html )
#
function( atlas_install_docs )

   # Call the generic function:
   atlas_install_generic( ${ARGN}
      DESTINATION ${CMAKE_INSTALL_DOCDIR}
      BUILD_DESTINATION ${CMAKE_DOC_OUTPUT_DIRECTORY}
      TYPENAME Doc
      PKGNAME_SUBDIR )

endfunction( atlas_install_docs )

# Function installing "executable files" during the build.
#
# Usage: atlas_install_runtime( share/*.py test/someFile.xml )
#
function( atlas_install_runtime )

   # Call the generic function:
   atlas_install_generic( ${ARGN}
      DESTINATION ${CMAKE_INSTALL_SHAREDIR}
      BUILD_DESTINATION ${CMAKE_SHARE_OUTPUT_DIRECTORY}
      TYPENAME Runtime
      EXECUTABLE )

endfunction( atlas_install_runtime )

# This function installs XML files from the package into the
# right place in both the build and the install directories.
#
# Usage: atlas_install_xmls( share/*.xml )
#
function( atlas_install_xmls )

   # Call the generic function:
   atlas_install_generic( ${ARGN}
      DESTINATION ${CMAKE_INSTALL_XMLDIR}
      BUILD_DESTINATION ${CMAKE_XML_OUTPUT_DIRECTORY}
      TYPENAME Xml
      PKGNAME_SUBDIR )

endfunction( atlas_install_xmls )

# This function installs executable script files from the package into the
# right place in both the build and the install directories.
#
# Usage: atlas_install_scripts( scripts/*.py )
#
function( atlas_install_scripts )

   # Call the generic function:
   atlas_install_generic( ${ARGN}
      DESTINATION ${CMAKE_INSTALL_BINDIR}
      BUILD_DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
      TYPENAME Scripts
      EXECUTABLE )

endfunction( atlas_install_scripts )

# Hide the private code from the outside:
unset( _atlas_create_install_target )
