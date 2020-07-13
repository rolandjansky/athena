# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHLTJetHypoUnitTests )

# External dependencies:
find_package( ROOT COMPONENTS Core Physics )
find_package( GMock )

atlas_add_library( TrigHLTJetHypoUnitTestsLib
                   src/*.cxx
                   exerciser/*.cxx
                   NO_PUBLIC_HEADERS
                   LINK_LIBRARIES TrigHLTJetHypoLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel xAODJet )

atlas_add_component( TrigHLTJetHypoUnitTests
                     exerciser/components/*.cxx
                     LINK_LIBRARIES TrigHLTJetHypoUnitTestsLib)
		     
# Test(s) in the package:
atlas_add_test( TrigHLTJetHypoTimerTest
   SOURCES src/Timer.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} TrigHLTJetHypoLib )

atlas_add_test( TrigHLTJetHypoUnitTests
   SOURCES tests/all_tests.cxx
   tests/CombinationsGenTest.cxx
   tests/DijetDEtaMassConditionTest.cxx
   tests/EtaEtConditionTest.cxx
   tests/FlowEdgeTest.cxx
   tests/FlowNetworkTest.cxx
   tests/LlpCleanerTest.cxx
   tests/LooseCleanerTest.cxx
   tests/MaximumBipartiteGroupsMatcherTest.cxx
   tests/MaximumBipartiteGroupsMatcherMTTest.cxx
   tests/MaximumBipartiteGroupsMatcherMTTest_Multijet.cxx
   tests/PartitionsGenTest.cxx
   tests/PartitionsGroupsMatcherMTTest.cxx
   tests/TLorentzVectorFactoryTest.cxx
   tests/TightCleanerTest.cxx
   tests/xAODJetCollectorTest.cxx
   tests/PartitionsGrouperTest.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} GoogleTestTools ${GMOCK_LIBRARIES} TrigHLTJetHypoLib TrigHLTJetHypoUnitTestsLib )
   
# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
