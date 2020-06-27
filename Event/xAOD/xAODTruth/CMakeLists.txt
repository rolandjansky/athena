# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODTruth )

# External dependencies.
find_package( ROOT COMPONENTS Core GenVector )
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODTruth
   xAODTruth/*.h xAODTruth/versions/*.h Root/*.h Root/*.cxx
   PUBLIC_HEADERS xAODTruth
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODBase xAODCore
   PRIVATE_LINK_LIBRARIES TruthUtils )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODTruth/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::TruthParticleContainer_v1" "xAOD::TruthVertexContainer_v1"
              "xAOD::TruthEventBaseContainer_v1" "xAOD::TruthEventContainer_v1"
              "xAOD::TruthPileupEventContainer_v1"
              "xAOD::TruthMetaDataContainer_v1" )

atlas_add_dictionary( xAODTruthDict
   xAODTruth/xAODTruthDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODTruth
   EXTRA_FILES Root/dict/*.cxx )

atlas_generate_cliddb( xAODTruth )

# Test(s) in the package.
atlas_add_test( ut_xaodtruth_particle_test
   SOURCES test/ut_xaodtruth_particle_test.cxx
   LINK_LIBRARIES xAODTruth )

if( XAOD_STANDALONE )
   atlas_add_test( ut_xaodtruth_helpers_test
      SCRIPT test/ut_xaodtruth_helpers_test.py )
endif()
