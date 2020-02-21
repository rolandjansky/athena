# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.

# Declare the package name.
atlas_subdir( TrigCompositeUtils )

# Set up the (non-)standalone compilation.
set( extra_srcs )
set( extra_libs )
if( NOT XAOD_STANDALONE )
  set( extra_srcs src/*.cxx )
  set( extra_libs GaudiKernel AthenaKernel AthLinks StoreGateLib AthContainers
    xAODTrigger )
endif()

# Add the package's stand alone library
atlas_add_library( HLTUtilsLib
  TrigCompositeUtils/HLTUtils.h Root/HLTUtils.cxx
  PUBLIC_HEADERS TrigCompositeUtils
  LINK_LIBRARIES CxxUtils )

# Add the package's dual use library.
atlas_add_library( TrigCompositeUtilsLib
  TrigCompositeUtils/HLTIdentifier.h TrigCompositeUtils/Combinators.h TrigCompositeUtils/TrigCompositeUtils.h TrigCompositeUtils/TrigCompositeUtils.icc Root/HLTIdentifier.cxx Root/Combinators.cxx ${extra_srcs}
  PUBLIC_HEADERS TrigCompositeUtils
  LINK_LIBRARIES HLTUtilsLib CxxUtils AsgMessagingLib ${extra_libs} )

# Install files from the package.
atlas_install_python_modules( python/*.py )

# Executable(s).
atlas_add_executable( trigconf_string2hash
  util/trigconf_string2hash.cxx
  LINK_LIBRARIES HLTUtilsLib )

# Unit test(s).
if( NOT XAOD_STANDALONE )
  atlas_add_test( TrigCompositeUtils_test
    SOURCES test/TrigCompositeUtils_test.cxx
    LINK_LIBRARIES TestTools AthenaKernel GaudiKernel SGTools StoreGateLib
    CxxUtils xAODTrigger TrigCompositeUtilsLib AthContainers )

  atlas_add_test( TrigTraversal_test
    SOURCES test/TrigTraversal_test.cxx
    LINK_LIBRARIES TestTools StoreGateLib AthenaKernel GaudiKernel SGTools
    CxxUtils xAODTrigger xAODEgamma xAODMuon xAODBase TrigCompositeUtilsLib )
endif()

atlas_add_test( Combinators_test
  SOURCES test/Combinators_test.cxx
  LINK_LIBRARIES TestTools TrigCompositeUtilsLib )

if( NOT XAOD_STANDALONE )
  atlas_add_test( flake8
    SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python/*.py
    POST_EXEC_SCRIPT nopost.sh )
endif()
