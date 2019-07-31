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
   SOURCES src/Timer.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} TrigHLTJetHypoLib )

atlas_add_test( TrigHLTJetHypoUnitTests
   SOURCES src/all_tests.cxx
   src/CombinationsGenTest.cxx
   src/DijetDEtaMassConditionTest.cxx
   src/EtaEtConditionTest.cxx
   src/FlowEdgeTest.cxx
   src/FlowNetworkTest.cxx
   src/LlpCleanerTest.cxx
   src/LooseCleanerTest.cxx
   src/MaximumBipartiteGroupsMatcherTest.cxx
   src/MaximumBipartiteGroupsMatcherMTTest.cxx
   src/MaximumBipartiteGroupsMatcherMTTest_Multijet.cxx
   src/MultiJetMatcherTest.cxx
   src/PartitionsGenTest.cxx
   src/PartitionsGroupsMatcherMTTest.cxx
   src/OrderedCollectionsMatcherTest.cxx
   src/SelectedJetsMatcherTest.cxx
   src/TLorentzVectorFactoryTest.cxx
   src/TightCleanerTest.cxx
   src/xAODJetCollectorTest.cxx
   src/PartitionsGrouperTest.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} GoogleTestTools
   TrigHLTJetHypoLib )
   
# Install files from the package:
atlas_install_python_modules( python/*.py ) 
