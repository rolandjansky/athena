################################################################################
# Package: TrigCaloEvent
################################################################################

# Declare the package name:
atlas_subdir( TrigCaloEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloGeoHelpers
                          Control/AthenaKernel
                          Control/AthLinks
                          Control/AthContainers
                          Control/Navigation
                          Event/EventKernel
                          Event/FourMom
                          GaudiKernel
                          Tools/AtlasDoxygen
                          Trigger/TrigEvent/TrigMissingEtEvent
                          PRIVATE
                          Calorimeter/CaloEvent
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODHIEvent
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODTrigL1Calo
                          Event/xAOD/xAODTrigMinBias
                          Event/xAOD/xAODTrigRinger
                          Trigger/TrigDataAccess/TrigSerializeCnvSvc
                          Trigger/TrigEvent/TrigNavigation )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigCaloEvent
                   src/*.cxx
                   src/components/*.cxx
                   PUBLIC_HEADERS TrigCaloEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloGeoHelpers AthLinks AthContainers Navigation AthenaKernel EventKernel FourMom GaudiKernel TrigMissingEtEvent TrigSerializeCnvSvcLib TrigNavigationLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent xAODCaloEvent xAODHIEvent xAODTrigCalo xAODTrigL1Calo xAODTrigMinBias xAODTrigRinger )

atlas_add_sercnv_library( TrigCaloEventSerCnv
                          FILES TrigCaloEvent/TrigCaloCluster.h TrigCaloEvent/TrigCaloClusterContainer.h TrigCaloEvent/TrigEMCluster.h TrigCaloEvent/TrigEMClusterContainer.h TrigCaloEvent/RingerRings.h TrigCaloEvent/RingerRingsContainer.h TrigCaloEvent/TrigTauCluster.h TrigCaloEvent/TrigTauClusterContainer.h TrigCaloEvent/TrigTauClusterDetails.h TrigCaloEvent/TrigTauClusterDetailsContainer.h TrigCaloEvent/TrigT2Jet.h TrigCaloEvent/TrigT2JetContainer.h TrigCaloEvent/Trig3Momentum.h TrigCaloEvent/TrigT2MbtsBits.h TrigCaloEvent/TrigT2MbtsBitsContainer.h TrigCaloEvent/TrigT2ZdcSignals.h TrigCaloEvent/TrigT2ZdcSignalsContainer.h TrigCaloEvent/TrigRNNOutput.h TrigCaloEvent/TrigRNNOutputContainer.h CaloEvent/CaloCellContainer.h CaloEvent/CaloClusterContainer.h CaloEvent/CaloShowerContainer.h CaloEvent/CaloTowerContainer.h CaloEvent/CaloCellLinkContainer.h
                          INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                          LINK_LIBRARIES ${ROOT_LIBRARIES} CaloGeoHelpers AthLinks AthContainers Navigation AthenaKernel EventKernel FourMom GaudiKernel TrigMissingEtEvent CaloEvent xAODCaloEvent xAODHIEvent xAODTrigCalo xAODTrigL1Calo xAODTrigMinBias xAODTrigRinger TrigSerializeCnvSvcLib TrigNavigationLib TrigCaloEvent )

atlas_add_sercnv_library( TrigCaloEventxAODSerCnv
                          FILES xAODTrigCalo/TrigCaloCluster.h xAODTrigCalo/TrigCaloClusterContainer.h xAODTrigCalo/TrigCaloClusterAuxContainer.h xAODTrigCalo/TrigEMCluster.h xAODTrigCalo/TrigEMClusterContainer.h xAODTrigCalo/TrigEMClusterAuxContainer.h xAODTrigCalo/CaloClusterTrigAuxContainer.h xAODTrigL1Calo/TriggerTower.h xAODTrigL1Calo/TriggerTowerContainer.h xAODTrigL1Calo/TriggerTowerAuxContainer.h xAODTrigRinger/TrigRingerRings.h xAODTrigRinger/TrigRingerRingsContainer.h xAODTrigRinger/TrigRingerRingsAuxContainer.h xAODTrigRinger/TrigRNNOutput.h xAODTrigRinger/TrigRNNOutputContainer.h xAODTrigRinger/TrigRNNOutputAuxContainer.h xAODTrigMinBias/TrigT2MbtsBits.h xAODTrigMinBias/TrigT2MbtsBitsContainer.h xAODTrigMinBias/TrigT2MbtsBitsAuxContainer.h xAODTrigMinBias/TrigT2ZdcSignals.h xAODTrigMinBias/TrigT2ZdcSignalsContainer.h xAODTrigMinBias/TrigT2ZdcSignalsAuxContainer.h xAODCaloEvent/CaloCluster.h xAODCaloEvent/CaloClusterContainer.h xAODCaloEvent/CaloClusterAuxContainer.h xAODHIEvent/HIEventShape.h xAODHIEvent/HIEventShapeContainer.h xAODHIEvent/HIEventShapeAuxContainer.h
                          TYPES_WITH_NAMESPACE xAOD::CaloCluster xAOD::CaloClusterContainer xAOD::CaloClusterAuxContainer xAOD::CaloClusterTrigAuxContainer xAOD::TrigCaloCluster xAOD::TrigCaloClusterContainer xAOD::TrigCaloClusterAuxContainer xAOD::TrigEMCluster xAOD::TrigEMClusterContainer xAOD::TrigEMClusterAuxContainer xAOD::TrigT2MbtsBits xAOD::TrigT2MbtsBitsContainer xAOD::TrigT2MbtsBitsAuxContainer xAOD::TrigRingerRings xAOD::TrigRingerRingsContainer xAOD::TrigRingerRingsAuxContainer xAOD::TrigRNNOutput xAOD::TrigRNNOutputContainer xAOD::TrigRNNOutputAuxContainer xAOD::TrigT2ZdcSignals xAOD::TrigT2ZdcSignalsContainer xAOD::TrigT2ZdcSignalsAuxContainer xAOD::TriggerTower xAOD::TriggerTowerContainer xAOD::TriggerTowerAuxContainer xAOD::HIEventShape xAOD::HIEventShapeContainer xAOD::HIEventShapeAuxContainer
                          CNV_PFX xAOD
                          INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                          LINK_LIBRARIES ${ROOT_LIBRARIES} CaloGeoHelpers AthLinks AthContainers Navigation AthenaKernel EventKernel FourMom GaudiKernel TrigMissingEtEvent CaloEvent xAODCaloEvent xAODHIEvent xAODTrigCalo xAODTrigL1Calo xAODTrigMinBias xAODTrigRinger TrigSerializeCnvSvcLib TrigNavigationLib TrigCaloEvent )

atlas_add_dictionary( TrigCaloEventDict
                      TrigCaloEvent/TrigCaloEventDict.h
                      TrigCaloEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloGeoHelpers AthLinks AthContainers Navigation AthenaKernel EventKernel FourMom GaudiKernel TrigMissingEtEvent CaloEvent xAODCaloEvent xAODHIEvent xAODTrigCalo xAODTrigL1Calo xAODTrigMinBias xAODTrigRinger TrigSerializeCnvSvcLib TrigNavigationLib TrigCaloEvent
                      DATA_LINKS TrigCaloCluster TrigEMCluster TrigT2Jet TrigTauCluster TrigTauClusterDetails RingerRings TrigRNNOutput
                      ELEMENT_LINKS TrigEMClusterContainer RingerRingsContainer TrigTauClusterContainer TrigTauClusterDetailsContainer TrigRNNOutputContainer )

