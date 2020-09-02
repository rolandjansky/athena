# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODTau )

# External dependencies.
find_package( ROOT COMPONENTS Core GenVector )
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODTau
   xAODTau/*.h xAODTau/*.icc xAODTau/versions/*.h
   Root/*.h Root/*.icc Root/*.cxx
   PUBLIC_HEADERS xAODTau
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODBase xAODCore
   xAODJet xAODPFlow xAODTracking xAODTruth )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODTau/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::TauJetContainer_v1" "xAOD::TauJetContainer_v2"
              "xAOD::TauJetContainer_v3" "xAOD::DiTauJetContainer_v1"
              "xAOD::TauTrackContainer_v1" )

atlas_add_dictionary( xAODTauDict
   xAODTau/xAODTauDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODTau
   EXTRA_FILES Root/dict/*.cxx )

atlas_add_dictionary( xAODTau_cDict
   xAODTau/xAODTau_cDict.h
   xAODTau/selection_c.xml
   LINK_LIBRARIES xAODTau )

# Test(s) in the package.
atlas_add_test( xAODTau_TauJet_test
   SOURCES test/xAODTau_TauJet_test.cxx
   LINK_LIBRARIES xAODTau )
