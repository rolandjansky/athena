# $Id: CMakeLists.txt 790212 2016-12-16 17:43:15Z ssnyder $

# The name of the package:
atlas_subdir( AtlasPyFwdBwdPorts )

# In "release mode" return right away:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# for setuptools
find_package( pytools )

# Helper macro for building and installing the packages. Documentation
# to be written later...
function( _setup_python_package name file md5 )

   # Parse the optional argument(s):
   cmake_parse_arguments( ARG "SINGLE_VERSION" ""
      "DEPENDS;EXTRA_ARGS;PATCH" ${ARGN} )

   # Extra argument(s) for the installation:
   if( ARG_SINGLE_VERSION )
      set( _extraArg )
   else()
      set( _extraArg --single-version-externally-managed )
   endif()

   if ( ARG_PATCH )
     set( _patchCmd PATCH_COMMAND patch -p1 < ${ARG_PATCH} )
   endif()

   # Build the package with the help of python's distutils:
   ExternalProject_Add( ${name}
      PREFIX ${CMAKE_BINARY_DIR}
      URL ${file}
      URL_MD5 ${md5}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${CMAKE_COMMAND} -E echo
      "Configuring the build of ${name}"
      ${_patchCmd}
      BUILD_COMMAND ${CMAKE_BINARY_DIR}/atlas_build_run.sh
      python setup.py ${ARG_EXTRA_ARGS} build
      INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_PYTHON_OUTPUT_DIRECTORY}
      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
      COMMAND ${CMAKE_BINARY_DIR}/atlas_build_run.sh
      python setup.py ${ARG_EXTRA_ARGS} install
      --prefix ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
      --exec-prefix ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
      --root /
      --install-lib ${CMAKE_PYTHON_OUTPUT_DIRECTORY}
      --install-scripts ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
      ${_extraArg} )

   # Make the package target depend on this one:
   add_dependencies( Package_AtlasPyFwdBwdPorts ${name} )

   # Add possible extra dependencies:
   if( ARG_DEPENDS )
      add_dependencies( ${name} ${ARG_DEPENDS} )
   endif()

   # Get the package directory:
   atlas_get_package_dir( pkgDir )

   # Add some metadata to the target:
   set_property( TARGET ${name} PROPERTY LABEL AtlasPyFwdBwdPorts )
   set_property( TARGET ${name} PROPERTY FOLDER ${pkgDir} )

   # Generate the package installer script:
   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/pkgbuildInstall.cmake.in
      ${CMAKE_CURRENT_BINARY_DIR}/${name}PkgbuildInstall.cmake
      @ONLY )

   # Use this script for installing the package:
   install( SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/${name}PkgbuildInstall.cmake )

endfunction( _setup_python_package )

# Install extensions:
_setup_python_package( extensions
   ${CMAKE_CURRENT_SOURCE_DIR}/src/extensions-0.4.tar.gz
   e498fe6da146aefb677c3e8a210fbb29
   SINGLE_VERSION )

# Install futures:
_setup_python_package( futures
   ${CMAKE_CURRENT_SOURCE_DIR}/src/futures-2.1.2.tar.gz
   a6fa247e3c5fe3d60d8e12f1b873cc88
   )

# Install pyflakes:
_setup_python_package( pyflakes
   ${CMAKE_CURRENT_SOURCE_DIR}/src/pyflakes-2.0.0.tar.gz
   06310b7c7a288c6c8e8f955da5f985ca
   SINGLE_VERSION )

# Install flake8:
_setup_python_package( flake8
   ${CMAKE_CURRENT_SOURCE_DIR}/src/flake8-3.6.0.tar.gz
   178485aed0799655d0cbf2e3bdcfaddc
   DEPENDS pyflakes pycodestyle mccabe enum34
   SINGLE_VERSION )

# Install enum34:
_setup_python_package( enum34
   ${CMAKE_CURRENT_SOURCE_DIR}/src/enum34-1.1.6.tar.gz
   5f13a0841a61f7fc295c514490d120d0
   SINGLE_VERSION )

# Install mccabe:
_setup_python_package( mccabe
   ${CMAKE_CURRENT_SOURCE_DIR}/src/mccabe-0.6.1.tar.gz
   723df2f7b1737b8887475bac4c763e1e
   SINGLE_VERSION )

# Install pycodestyle:
_setup_python_package( pycodestyle 
   ${CMAKE_CURRENT_SOURCE_DIR}/src/pycodestyle-2.4.0.tar.gz
   85bbebd2c90d2f833c1db467d4d0e9a3
   SINGLE_VERSION )

# Install pyyaml:
_setup_python_package( pyyaml
   ${CMAKE_CURRENT_SOURCE_DIR}/src/pyyaml-3.10.tar.gz
   45e5b429265d7815699be439adc8b37e
   EXTRA_ARGS --without-libyaml
   SINGLE_VERSION )

# Install scandir:
_setup_python_package( scandir
   ${CMAKE_CURRENT_SOURCE_DIR}/src/scandir-1.6.tar.gz
   0180ddb97c96cbb2d4f25d2ae11c64ac
   )

# Clean up:
unset( _setup_python_package )
