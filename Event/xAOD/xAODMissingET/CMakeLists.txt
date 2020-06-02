# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODMissingET )

# External dependencies.
find_package( Boost COMPONENTS unit_test_framework )
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODMissingET
   xAODMissingET/*.h xAODMissingET/versions/*.h xAODMissingET/versions/*.icc
   Root/*.cxx
   PUBLIC_HEADERS xAODMissingET
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES AthContainers AthLinks xAODBase xAODCore xAODJet
   PRIVATE_LINK_LIBRARIES xAODCaloEvent xAODTracking xAODTruth )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODMissingET/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::MissingETContainer_v1" "xAOD::MissingETComponentMap_v1"
              "xAOD::MissingETAssociationMap_v1" )

atlas_add_dictionary( xAODMissingETDict
   xAODMissingET/xAODMissingETDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODMissingET
   EXTRA_FILES Root/dict/*.cxx )

atlas_add_executable( xAODMissingET_testMETHash
   util/testMETHash.cxx
   LINK_LIBRARIES xAODMissingET )

# Test(s) in the package.
atlas_add_test( edm_test
   SOURCES test/edm_test.cxx
   LINK_LIBRARIES xAODMissingET )

atlas_add_test( ut_edmTest
   SOURCES test/ut_edmTest.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} xAODMissingET CxxUtils)
