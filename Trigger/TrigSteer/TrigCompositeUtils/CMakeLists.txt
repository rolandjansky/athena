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

# Add the package's dual use library.
atlas_add_library( TrigCompositeUtilsLib
  TrigCompositeUtils/*.h TrigCompositeUtils/*.icc Root/*.cxx ${extra_srcs}
  PUBLIC_HEADERS TrigCompositeUtils
  LINK_LIBRARIES TrigConfHLTUtilsLib CxxUtils AsgMessagingLib AsgDataHandlesLib TrigDecisionInterface xAODTrigger ${extra_libs} )

# Install files from the package.
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

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
