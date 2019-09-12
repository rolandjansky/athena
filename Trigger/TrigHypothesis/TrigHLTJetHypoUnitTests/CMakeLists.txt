# $Id: CMakeLists.txt 727053 2016-03-01 14:24:32Z krasznaa $
################################################################################
# Package: TrigHLTJetHypoUnitTests
################################################################################

# Declare the package name:
atlas_subdir( TrigHLTJetHypoUnitTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Trigger/TrigHypothesis/TrigHLTJetHypo )

# External dependencies:
find_package( ROOT COMPONENTS Core Physics )

atlas_add_library( TrigHLTJetHypoUnitTestsLib
                   src/*.cxx
                   exerciser/*.cxx
                   PUBLIC_HEADERS TrigHLTJetHypoUnitTests
                   # PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES xAODJet GaudiKernel TrigParticle TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib DecisionHandlingLib TrigHLTJetHypoLib 
                   # PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )
		   )

atlas_add_component( TrigHLTJetHypoUnitTests
                     exerciser/components/*.cxx
                     LINK_LIBRARIES  TrigHLTJetHypoUnitTestsLib)
		     
atlas_install_headers( TrigHLTJetHypoUnitTests )

# Test(s) in the package:
atlas_add_test( TrigHLTJetHypoTimerTest
   SOURCES tests/Timer.cxx
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
   tests/UnifiedFlowNetworkMatcherTest.cxx
   tests/MultiJetMatcherTest.cxx
   tests/PartitionsGenTest.cxx
   tests/PartitionsGroupsMatcherMTTest.cxx
   tests/OrderedCollectionsMatcherTest.cxx
   tests/SelectedJetsMatcherTest.cxx
   tests/TLorentzVectorFactoryTest.cxx
   tests/TightCleanerTest.cxx
   tests/xAODJetCollectorTest.cxx
   tests/PartitionsGrouperTest.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} GoogleTestTools
   TrigHLTJetHypoLib
   TrigHLTJetHypoUnitTestsLib)
   
# Install files from the package:
atlas_install_python_modules( python/*.py ) 
