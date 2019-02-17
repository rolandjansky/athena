# $Id: CMakeLists.txt 766460 2016-08-04 14:17:04Z khoo $
################################################################################
# Package: xAODMissingET
################################################################################

# Declare the package name:
atlas_subdir( xAODMissingET )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   Event/xAOD/xAODBase
   Event/xAOD/xAODCore
   Event/xAOD/xAODJet
   PRIVATE
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODTracking
   Event/xAOD/xAODTruth )

# External dependencies:
find_package( Boost COMPONENTS unit_test_framework )

# Component(s) in the package:
atlas_add_library( xAODMissingET
   xAODMissingET/*.h xAODMissingET/versions/*.h xAODMissingET/versions/*.icc
   Root/*.cxx
   PUBLIC_HEADERS xAODMissingET
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES AthContainers AthLinks xAODBase xAODCore xAODJet
   PRIVATE_LINK_LIBRARIES xAODCaloEvent xAODTracking xAODTruth )

atlas_add_dictionary( xAODMissingETDict
   xAODMissingET/xAODMissingETDict.h
   xAODMissingET/selection.xml
   LINK_LIBRARIES xAODMissingET
   EXTRA_FILES Root/dict/*.cxx )

atlas_add_executable( xAODMissingET_testMETHash
   util/testMETHash.cxx
   LINK_LIBRARIES xAODMissingET )

# Test(s) in the package:
atlas_add_test( edm_test
   SOURCES test/edm_test.cxx
   LINK_LIBRARIES xAODMissingET )

atlas_add_test( ut_edmTest
   SOURCES test/ut_edmTest.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} xAODMissingET CxxUtils)

