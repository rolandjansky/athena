# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.

# Declare the package name.
atlas_subdir( TrigCompositeUtils )

find_package( Boost )

# Set up the (non-)standalone compilation.
set( extra_libs )
if( NOT XAOD_STANDALONE )
  set( extra_libs GaudiKernel AthenaKernel AthLinks StoreGateLib AthContainers AthenaBaseComps TrigConfData )
endif()

# Add the package's dual use library.
atlas_add_library( TrigCompositeUtilsLib
  TrigCompositeUtils/*.h TrigCompositeUtils/*.icc Root/*.cxx
  PUBLIC_HEADERS TrigCompositeUtils
  INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES TrigConfHLTUtilsLib CxxUtils AsgMessagingLib AsgDataHandlesLib AsgTools TrigDecisionInterface xAODBase xAODTrigger TrigConfHLTData ${extra_libs}
  PRIVATE_LINK_LIBRARIES TrigSteeringEvent )

if( NOT XAOD_STANDALONE )
  atlas_add_component( TrigCompositeUtils
    TrigCompositeUtils/components/*.cxx
    LINK_LIBRARIES TrigCompositeUtilsLib )
endif()

# Install files from the package.
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Unit test(s).
if( NOT XAOD_STANDALONE )
  atlas_add_test( TrigCompositeUtils_test
    SOURCES test/TrigCompositeUtils_test.cxx
    LINK_LIBRARIES TestTools AthenaKernel GaudiKernel SGTools StoreGateLib
    CxxUtils xAODTrigger TrigCompositeUtilsLib AthContainers
    LOG_IGNORE_PATTERN "@0x[0-9a-f]{4,}" )

  atlas_add_test( TrigTraversal_test
    SOURCES test/TrigTraversal_test.cxx
    LINK_LIBRARIES TestTools StoreGateLib AthenaKernel GaudiKernel SGTools
    CxxUtils xAODTrigger xAODEgamma xAODMuon xAODBase TrigCompositeUtilsLib )
endif()

atlas_add_test( Combinators_test
  SOURCES test/Combinators_test.cxx
  LINK_LIBRARIES TestTools TrigCompositeUtilsLib )

atlas_add_test( KFromNItr_test
  SOURCES test/KFromNItr_test.cxx
  LINK_LIBRARIES TestTools TrigCompositeUtilsLib )

atlas_add_test( IPartCombItr_test
  SOURCES test/IPartCombItr_test.cxx
  LINK_LIBRARIES TestTools TrigCompositeUtilsLib xAODBase xAODTrigger)

atlas_add_test( ChainNameParser_test
  SOURCES test/ChainNameParser_test.cxx
  LINK_LIBRARIES TestTools TrigCompositeUtilsLib xAODBase xAODTrigger
  POST_EXEC_SCRIPT nopost.sh)
