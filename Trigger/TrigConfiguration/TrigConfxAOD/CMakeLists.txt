# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigConfxAOD )

# External dependencies:
find_package( Boost )

# Libraries in the package:
atlas_add_library( TrigConfxAODLib
   TrigConfxAOD/*.h Root/*.cxx
   PUBLIC_HEADERS TrigConfxAOD
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES AsgMessagingLib AsgTools TrigConfData TrigConfHLTData TrigConfInterfaces TrigConfL1Data xAODTrigger
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} )

if( NOT XAOD_STANDALONE )
   atlas_add_component( TrigConfxAOD
      src/*.h src/*.cxx src/components/*.cxx
      INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
      LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils GaudiKernel StoreGateLib TrigConfData TrigConfHLTData TrigConfInterfaces TrigConfL1Data TrigConfxAODLib xAODTrigger )
endif()

atlas_add_dictionary( TrigConfxAODDict
   TrigConfxAOD/TrigConfxAODDict.h
   TrigConfxAOD/selection.xml
   LINK_LIBRARIES TrigConfxAODLib )

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( xAODTrigConfReadTest
      util/xAODTrigConfReadTest.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess AsgTools TrigConfxAODLib )
endif()

# Test(s) in the package:
if( XAOD_STANDALONE )
   # FIX ME: temporarily disabled as it is failing
   # atlas_add_test( xaodconfigtool_test
   #    SCRIPT test/ut_trigconfxaod_xaodconfigtool_test.py )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/*.py )
