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

# Install abc:
_setup_python_package( abc
   ${CMAKE_CURRENT_SOURCE_DIR}/src/abc-2.6.1.0.tar.gz
   a0851484aa93cb8eac113fbaf1238414
   SINGLE_VERSION )

# Install affinity:
_setup_python_package( affinity
   ${CMAKE_CURRENT_SOURCE_DIR}/src/affinity-0.1.0.tar.gz
   cc610cdb05ca675b4089ce2f05796f57
   DEPENDS distribute )

# Install argparse:
_setup_python_package( argparse
   ${CMAKE_CURRENT_SOURCE_DIR}/src/argparse-1.2.1.tar.gz
   2fbef8cb61e506c706957ab6e135840c
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
   DEPENDS argparse )

# Install hgsvn:
_setup_python_package( hgsvn
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hgsvn-0.1.9.tar.gz
   901f2fb04faad5a1e7cf84abfad564fd
   DEPENDS distribute )

# Install iterutils:
_setup_python_package( iterutils
   ${CMAKE_CURRENT_SOURCE_DIR}/src/iterutils-0.1.6.tar.gz
   e293044ec65d757ce075ac170ad9ada8
   DEPENDS distribute
   SINGLE_VERSION )

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

# Install mercurial:
_setup_python_package( mercurial
   ${CMAKE_CURRENT_SOURCE_DIR}/src/mercurial-2.1.2.tar.gz
   15354d739504ec46f68a0fee2ecfbf25
   SINGLE_VERSION )

# Install nested_dict:
#_setup_python_package( nested_dict
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/nested_dict-1.0.9.tar.gz 
#   aeb571306af08142d11fee99ff569019 )

# Install ordereddict:
_setup_python_package( ordereddict
   ${CMAKE_CURRENT_SOURCE_DIR}/src/ordereddict-1.1.tar.gz
   7ff95dba551c563ad04ba7076b16607f
   SINGLE_VERSION )

# Install paramiko:
_setup_python_package( paramiko
   ${CMAKE_CURRENT_SOURCE_DIR}/src/paramiko-1.7.6.tar.gz
   fa144ab46f1dc639b05ab948c30efac4
   DEPENDS pycrypto )

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
   ${CMAKE_CURRENT_SOURCE_DIR}/src/pyflakes-1.1.0.tar.gz
   e0bf854cd5abd4527e149692925b82eb
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

# Install ruffus:
_setup_python_package( ruffus
   ${CMAKE_CURRENT_SOURCE_DIR}/src/ruffus-2.2.tar.gz
   d96fc1b90ad58367f04538e7c9b07c95
   DEPENDS simplejson )

# Install simplejson:
_setup_python_package( simplejson
   ${CMAKE_CURRENT_SOURCE_DIR}/src/simplejson-2.1.6.tar.gz
   2f8351f6e6fe7ef25744805dfa56c0d5
   DEPENDS distribute )

# Install virtualenv:
_setup_python_package( virtualenv
   ${CMAKE_CURRENT_SOURCE_DIR}/src/virtualenv-1.6.4.tar.gz
   1072b66d53c24e019a8f1304ac9d9fc5
   DEPENDS pip )

# Clean up:
unset( _setup_python_package )

