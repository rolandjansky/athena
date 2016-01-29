# $Id: CMakeLists.txt 720369 2016-01-28 15:58:07Z krasznaa $

# The name of the package:
atlas_subdir( pyAMI )

# In "release mode" return right away:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Helper macro for building and installing the package(s). Documentation
# to be written later...
function( _setup_python_package name file )

   # Parse the optional argument(s):
   cmake_parse_arguments( ARG "" "" "" ${ARGN} )

   # Build the package with the help of python's distutils:
   ExternalProject_Add( ${name}
      PREFIX ${CMAKE_BINARY_DIR}
      URL ${file}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${CMAKE_COMMAND} -E echo
      "Configuring the build of ${name}"
      BUILD_COMMAND ${CMAKE_BINARY_DIR}/atlas_build_run.sh
      python setup.py build
      INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_PYTHON_OUTPUT_DIRECTORY}
      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
      COMMAND ${CMAKE_BINARY_DIR}/atlas_build_run.sh
      python setup.py install --prefix ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
      --exec-prefix ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
      --root /
      --install-lib ${CMAKE_PYTHON_OUTPUT_DIRECTORY}
      --install-scripts ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )

   # Make the package target depend on this one:
   add_dependencies( Package_pyAMI ${name} )

   # Get the package directory:
   atlas_get_package_dir( pkgDir )

   # Add some metadata to the target:
   set_property( TARGET ${name} PROPERTY LABEL pyAMI )
   set_property( TARGET ${name} PROPERTY FOLDER ${pkgDir} )

   # Generate the package installer script:
   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/pkgbuildInstall.cmake.in
      ${CMAKE_CURRENT_BINARY_DIR}/${name}PkgbuildInstall.cmake
      @ONLY )

   # Use this script for installing the package:
   install( SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/${name}PkgbuildInstall.cmake )

endfunction( _setup_python_package )

# Install pyAMI:
_setup_python_package( pyAMI
   ${CMAKE_CURRENT_SOURCE_DIR}/src/pyAMI-5.0.3.2.tar.gz )

# Clean up:
unset( _setup_python_package )
