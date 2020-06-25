# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODTrigMuon )

# External dependencies.
find_package( ROOT COMPONENTS Core GenVector )
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODTrigMuon
    xAODTrigMuon/*.h xAODTrigMuon/versions/*.h Root/*.cxx
    PUBLIC_HEADERS xAODTrigMuon
    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
    LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODBase xAODCore
    xAODTracking )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODTrigMuon/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::L2StandAloneMuonContainer_v1"
              "xAOD::L2StandAloneMuonContainer_v2" "xAOD::L2IsoMuonContainer_v1"
              "xAOD::L2CombinedMuonContainer_v1" )

atlas_add_dictionary( xAODTrigMuonDict
    xAODTrigMuon/xAODTrigMuonDict.h
    ${_selectionFile}
    LINK_LIBRARIES xAODCore xAODTrigMuon
    EXTRA_FILES Root/dict/*.cxx )
