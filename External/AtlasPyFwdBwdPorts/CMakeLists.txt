# $Id: CMakeLists.txt 790212 2016-12-16 17:43:15Z ssnyder $

# The name of the package:
atlas_subdir( AtlasPyFwdBwdPorts )

# In "release mode" return right away:
if( ATLAS_RELEASE_MODE )
   return()
endif()

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

# Install Beaker:
_setup_python_package( Beaker
   ${CMAKE_CURRENT_SOURCE_DIR}/src/Beaker-1.5.4.tar.gz
   de84e7511119dc0b8eb4ac177d3e2512
   DEPENDS distribute )

# Install affinity:
_setup_python_package( affinity
   ${CMAKE_CURRENT_SOURCE_DIR}/src/affinity-0.1.0.tar.gz
   cc610cdb05ca675b4089ce2f05796f57
   DEPENDS distribute )

# Install bunch:
_setup_python_package( bunch
   ${CMAKE_CURRENT_SOURCE_DIR}/src/bunch-1.0.0.tar.gz
   944d2e8f222ed032961daa34c5b5151c
   DEPENDS distribute )

# Install datadiff:
_setup_python_package( datadiff
   ${CMAKE_CURRENT_SOURCE_DIR}/src/datadiff-1.1.1.tar.gz
   48a2e9b85332b8252c8401205ff02756
   DEPENDS distribute )

# Install distribute:
_setup_python_package( distribute
   ${CMAKE_CURRENT_SOURCE_DIR}/src/distribute-0.6.25.tar.gz 
   a690874b9964d958a3200485eb827b1d )

# Install extensions:
_setup_python_package( extensions
   ${CMAKE_CURRENT_SOURCE_DIR}/src/extensions-0.4.tar.gz
   e498fe6da146aefb677c3e8a210fbb29
   SINGLE_VERSION )

# Install futures:
_setup_python_package( futures
   ${CMAKE_CURRENT_SOURCE_DIR}/src/futures-2.1.2.tar.gz
   a6fa247e3c5fe3d60d8e12f1b873cc88
   DEPENDS distribute )

# Install grin:
_setup_python_package( grin
   ${CMAKE_CURRENT_SOURCE_DIR}/src/grin-1.2.1.tar.gz
   d894426dfbf70bc105388c2a51348199
   DEPENDS distribute )

# Install jsonpickle:
_setup_python_package( jsonpickle
   ${CMAKE_CURRENT_SOURCE_DIR}/src/jsonpickle-0.4.0.tar.gz
   63916228294218ca6e53fd1b16b626fa
   DEPENDS simplejson
   SINGLE_VERSION )

# Install keyring:
_setup_python_package( keyring
   ${CMAKE_CURRENT_SOURCE_DIR}/src/keyring-0.5.1.tar.gz
   a2f0dcea7185580c163ef2db1f4fbe0c
   DEPENDS distribute
   SINGLE_VERSION )

# Install pip:
_setup_python_package( pip
   ${CMAKE_CURRENT_SOURCE_DIR}/src/pip-1.1.tar.gz
   62a9f08dd5dc69d76734568a6c040508
   DEPENDS distribute )

# Install pycrypto:
_setup_python_package( pycrypto
   ${CMAKE_CURRENT_SOURCE_DIR}/src/pycrypto-2.6.1.tar.gz
   55a61a054aa66812daf5161a0d5d7eda
   DEPENDS distribute
   PATCH ${CMAKE_CURRENT_SOURCE_DIR}/src/pycrypto-warning.patch
   SINGLE_VERSION )

# Install pyflakes:
_setup_python_package( pyflakes
   ${CMAKE_CURRENT_SOURCE_DIR}/src/pyflakes-1.5.0.tar.gz
   84a99f05e5409f8196325dda3f5a1b9a
   SINGLE_VERSION )

# Install flake8:
_setup_python_package( flake8
   ${CMAKE_CURRENT_SOURCE_DIR}/src/flake8-3.3.0.tar.gz
   3df622aac9bad27c04f34164609bbed8
   DEPENDS pyflakes
   SINGLE_VERSION )

# Install configparser:
_setup_python_package( configparser
   ${CMAKE_CURRENT_SOURCE_DIR}/src/configparser-3.5.0.tar.gz
   cfdd915a5b7a6c09917a64a573140538
   DEPENDS distribute
   SINGLE_VERSION )

# Install enum34:
_setup_python_package( enum34
   ${CMAKE_CURRENT_SOURCE_DIR}/src/enum34-1.1.6.tar.gz
   5f13a0841a61f7fc295c514490d120d0
   DEPENDS distribute
   SINGLE_VERSION )

# Install mccabe:
_setup_python_package( mccabe
   ${CMAKE_CURRENT_SOURCE_DIR}/src/mccabe-0.6.1.tar.gz
   723df2f7b1737b8887475bac4c763e1e
   DEPENDS distribute
   SINGLE_VERSION )

# Install :
_setup_python_package( pycodestyle 
   ${CMAKE_CURRENT_SOURCE_DIR}/src/pycodestyle-2.3.1.tar.gz
   240e342756af30cae0983b16303a2055
   DEPENDS distribute
   SINGLE_VERSION )

# Install pyinotify:
_setup_python_package( pyinotify
   ${CMAKE_CURRENT_SOURCE_DIR}/src/pyinotify-0.9.1.tar.gz
   bd06a9feac312414e57d92c781bda539
   DEPENDS distribute
   SINGLE_VERSION )

# Install pyyaml:
_setup_python_package( pyyaml
   ${CMAKE_CURRENT_SOURCE_DIR}/src/pyyaml-3.10.tar.gz
   45e5b429265d7815699be439adc8b37e
   DEPENDS distribute
   EXTRA_ARGS --without-libyaml
   SINGLE_VERSION )

# Install simplejson:
_setup_python_package( simplejson
   ${CMAKE_CURRENT_SOURCE_DIR}/src/simplejson-2.1.6.tar.gz
   2f8351f6e6fe7ef25744805dfa56c0d5
   DEPENDS distribute )

# Clean up:
unset( _setup_python_package )

