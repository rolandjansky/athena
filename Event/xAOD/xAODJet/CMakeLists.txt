# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODJet )

# Optional dependencies:
set( extra_libs )
set( extra_test_libs )

# Extra dependencies, when we are not in a standalone build:
if( NOT XAOD_STANDALONE )
   set( extra_test_libs SGTools )
else()
   set( extra_test_libs xAODRootAccess )
endif()

set( extra_srcs )
if( NOT SIMULATIONBASE AND NOT GENERATIONBASE )
   set( extra_srcs Root/JetTrigAuxContainer*.cxx )
   set( extra_libs xAODBTagging xAODPFlow xAODTrigger )
endif()

# External dependencies:
find_package( ROOT COMPONENTS Core GenVector )
find_package( xAODUtilities )

# Component(s) in the package:
atlas_add_library( xAODJet
   xAODJet/*.h xAODJet/*.icc xAODJet/versions/*.h xAODJet/versions/*.icc
   Root/JetA*.cxx Root/JetC*.cxx Root/Jet_v*.cxx Root/xAODJetCLIDs.cxx
   ${extra_srcs}
   PUBLIC_HEADERS xAODJet
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks
   xAODBase xAODCore ${extra_libs}
   PRIVATE_LINK_LIBRARIES CxxUtils xAODCaloEvent )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODJet/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::JetContainer_v1" )

atlas_add_dictionary( xAODJetDict
   xAODJet/xAODJetDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODJet
   EXTRA_FILES Root/dict/*.cxx )

# Test(s) in the package:
atlas_add_test( xAODJet_Jet_test
   SOURCES test/xAODJet_Jet_test.cxx test/JetFactory.h test/JetFactory.cxx
   LINK_LIBRARIES xAODCore xAODCaloEvent xAODJet CxxUtils ${extra_test_libs}
   LOG_IGNORE_PATTERN "xAOD::Init" )
