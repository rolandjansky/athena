# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.

# Declare the package name.
atlas_subdir( TrigCompositeUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigEvent/TrigDecisionInterface 
                        )

# Set up the (non-)standalone compilation.
set( extra_srcs )
set( extra_libs )
if( NOT XAOD_STANDALONE )
  set( extra_srcs src/*.cxx )
  set( extra_libs GaudiKernel AthenaKernel AthLinks StoreGateLib AthContainers
    xAODTrigger )
endif()

# Add the package's dual use library.
atlas_add_library( TrigCompositeUtilsLib
  TrigCompositeUtils/*.h TrigCompositeUtils/*.icc Root/*.cxx ${extra_srcs}
  PUBLIC_HEADERS TrigCompositeUtils
  LINK_LIBRARIES TrigConfHLTUtilsLib CxxUtils AsgMessagingLib TrigDecisionInterface ${extra_libs} )

# Install files from the package.
atlas_install_python_modules( python/*.py )

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
