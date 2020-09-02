# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODTrigL1Calo )
                         
# External dependencies.
find_package( ROOT COMPONENTS Core GenVector )
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODTrigL1Calo
    xAODTrigL1Calo/*.h xAODTrigL1Calo/versions/*.h Root/*.cxx
    PUBLIC_HEADERS xAODTrigL1Calo
    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
    LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers xAODBase xAODCore )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODTrigL1Calo/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::JEMHitsContainer_v1" "xAOD::JEMEtSumsContainer_v1"
              "xAOD::JEMRoIContainer_v1" "xAOD::CPMHitsContainer_v1"
              "xAOD::CPMTowerContainer_v1" "xAOD::CPMRoIContainer_v1"
              "xAOD::CMMCPHitsContainer_v1" "xAOD::CMMEtSumsContainer_v1"
              "xAOD::CMMJetHitsContainer_v1" "xAOD::JetElementContainer_v1"
              "xAOD::RODHeaderContainer_v1" "xAOD::RODHeaderContainer_v2"
              "xAOD::TriggerTowerContainer_v1" "xAOD::TriggerTowerContainer_v2"
              "xAOD::CMXCPHitsContainer_v1" "xAOD::CMXCPTobContainer_v1"
              "xAOD::CMXJetHitsContainer_v1" "xAOD::CMXJetTobContainer_v1"
              "xAOD::CMXEtSumsContainer_v1" "xAOD::CMXRoIContainer_v1"
              "xAOD::CPMTowerContainer_v2" "xAOD::CPMTobRoIContainer_v1"
              "xAOD::JEMEtSumsContainer_v2" "xAOD::JEMTobRoIContainer_v1"
              "xAOD::JetElementContainer_v2" "xAOD::L1TopoRawDataContainer_v1"
   OBJECTS "xAOD::CMMRoI_v1" )

atlas_add_dictionary( xAODTrigL1CaloDict
    xAODTrigL1Calo/xAODTrigL1CaloDict.h
    ${_selectionFile}
    LINK_LIBRARIES xAODCore xAODTrigL1Calo
    EXTRA_FILES Root/dict/*.cxx )
