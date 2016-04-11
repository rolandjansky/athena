# $Id: CMakeLists.txt 739538 2016-04-11 23:15:41Z ssnyder $

# The name of the package:
atlas_subdir( AtlasPyFwdBwdPorts )

# In "release mode" return right away:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Helper macro for building and installing the packages. Documentation
# to be written later...
function( _setup_python_package name file )

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

# Install Beaker:
_setup_python_package( Beaker
   ${CMAKE_CURRENT_SOURCE_DIR}/src/Beaker-1.5.4.tar.gz
   DEPENDS distribute )

# Install abc:
_setup_python_package( abc
   ${CMAKE_CURRENT_SOURCE_DIR}/src/abc-2.6.1.0.tar.gz
   SINGLE_VERSION )

# Install affinity:
_setup_python_package( affinity
   ${CMAKE_CURRENT_SOURCE_DIR}/src/affinity-0.1.0.tar.gz
   DEPENDS distribute )

# Install argparse:
_setup_python_package( argparse
   ${CMAKE_CURRENT_SOURCE_DIR}/src/argparse-1.2.1.tar.gz
   DEPENDS distribute )

# Install bunch:
_setup_python_package( bunch
   ${CMAKE_CURRENT_SOURCE_DIR}/src/bunch-1.0.0.tar.gz
   DEPENDS distribute )

# Install datadiff:
_setup_python_package( datadiff
   ${CMAKE_CURRENT_SOURCE_DIR}/src/datadiff-1.1.1.tar.gz
   DEPENDS distribute )

# Install distribute:
_setup_python_package( distribute
   ${CMAKE_CURRENT_SOURCE_DIR}/src/distribute-0.6.25.tar.gz )

# Install extensions:
_setup_python_package( extensions
   ${CMAKE_CURRENT_SOURCE_DIR}/src/extensions-0.4.tar.gz
   SINGLE_VERSION )

# Install futures:
_setup_python_package( futures
   ${CMAKE_CURRENT_SOURCE_DIR}/src/futures-2.1.2.tar.gz
   DEPENDS distribute )

# Install grin:
_setup_python_package( grin
   ${CMAKE_CURRENT_SOURCE_DIR}/src/grin-1.2.1.tar.gz
   DEPENDS argparse )

# Install hgsvn:
_setup_python_package( hgsvn
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hgsvn-0.1.9.tar.gz
   DEPENDS distribute )

# Install iterutils:
_setup_python_package( iterutils
   ${CMAKE_CURRENT_SOURCE_DIR}/src/iterutils-0.1.6.tar.gz
   DEPENDS distribute
   SINGLE_VERSION )

# Install jsonpickle:
_setup_python_package( jsonpickle
   ${CMAKE_CURRENT_SOURCE_DIR}/src/jsonpickle-0.4.0.tar.gz
   DEPENDS simplejson
   SINGLE_VERSION )

# Install keyring:
_setup_python_package( keyring
   ${CMAKE_CURRENT_SOURCE_DIR}/src/keyring-0.5.1.tar.gz
   DEPENDS distribute
   SINGLE_VERSION )

# Install mercurial:
_setup_python_package( mercurial
   ${CMAKE_CURRENT_SOURCE_DIR}/src/mercurial-2.1.2.tar.gz
   SINGLE_VERSION )

# Install nested_dict:
_setup_python_package( nested_dict
   ${CMAKE_CURRENT_SOURCE_DIR}/src/nested_dict-1.0.9.tar.gz )

# Install ordereddict:
_setup_python_package( ordereddict
   ${CMAKE_CURRENT_SOURCE_DIR}/src/ordereddict-1.1.tar.gz
   SINGLE_VERSION )

# Install paramiko:
_setup_python_package( paramiko
   ${CMAKE_CURRENT_SOURCE_DIR}/src/paramiko-1.7.6.tar.gz
   DEPENDS pycrypto )

# Install pip:
_setup_python_package( pip
   ${CMAKE_CURRENT_SOURCE_DIR}/src/pip-1.1.tar.gz
   DEPENDS distribute )

# Install pycrypto:
_setup_python_package( pycrypto
   ${CMAKE_CURRENT_SOURCE_DIR}/src/pycrypto-2.1.0.tar.gz
   DEPENDS distribute
   PATCH ${CMAKE_CURRENT_SOURCE_DIR}/src/pycrypto.patch
   SINGLE_VERSION )

# Install pyflakes:
_setup_python_package( pyflakes
   ${CMAKE_CURRENT_SOURCE_DIR}/src/pyflakes-1.1.0.tar.gz
   SINGLE_VERSION )

# Install pyinotify:
_setup_python_package( pyinotify
   ${CMAKE_CURRENT_SOURCE_DIR}/src/pyinotify-0.9.1.tar.gz
   DEPENDS distribute
   SINGLE_VERSION )

# Install pyyaml:
_setup_python_package( pyyaml
   ${CMAKE_CURRENT_SOURCE_DIR}/src/pyyaml-3.10.tar.gz
   DEPENDS distribute
   EXTRA_ARGS --without-libyaml
   SINGLE_VERSION )

# Install ruffus:
_setup_python_package( ruffus
   ${CMAKE_CURRENT_SOURCE_DIR}/src/ruffus-2.2.tar.gz
   DEPENDS simplejson )

# Install simplejson:
_setup_python_package( simplejson
   ${CMAKE_CURRENT_SOURCE_DIR}/src/simplejson-2.1.6.tar.gz
   DEPENDS distribute )

# Install virtualenv:
_setup_python_package( virtualenv
   ${CMAKE_CURRENT_SOURCE_DIR}/src/virtualenv-1.6.4.tar.gz
   DEPENDS pip )

# Clean up:
unset( _setup_python_package )
