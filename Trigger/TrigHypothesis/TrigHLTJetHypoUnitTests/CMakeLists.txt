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
find_package( GMock )

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
   src/MaximumBipartiteMatcherTest.cxx
   src/MultiJetMatcherTest.cxx
   src/OrderedCollectionsMatcherTest.cxx
   src/SelectedJetsMatcherTest.cxx
   src/TLorentzVectorFactoryTest.cxx
   src/TightCleanerTest.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${GMOCK_LIBRARIES}
   TrigHLTJetHypoLib )
