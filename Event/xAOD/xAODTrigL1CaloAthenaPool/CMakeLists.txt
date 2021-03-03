################################################################################
# Package: xAODTrigL1CaloAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigL1CaloAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODTrigL1Calo )
	
# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
	
# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigL1CaloAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTrigL1Calo/JEMHitsContainer.h xAODTrigL1Calo/JEMHitsAuxContainer.h xAODTrigL1Calo/JEMEtSumsContainer.h xAODTrigL1Calo/JEMEtSumsAuxContainer.h xAODTrigL1Calo/JEMRoIContainer.h xAODTrigL1Calo/JEMRoIAuxContainer.h xAODTrigL1Calo/CPMHitsContainer.h xAODTrigL1Calo/CPMHitsAuxContainer.h xAODTrigL1Calo/CPMTowerContainer.h xAODTrigL1Calo/CPMTowerAuxContainer.h xAODTrigL1Calo/CPMRoIContainer.h xAODTrigL1Calo/CPMRoIAuxContainer.h xAODTrigL1Calo/CMMCPHitsContainer.h xAODTrigL1Calo/CMMCPHitsAuxContainer.h xAODTrigL1Calo/CMMEtSumsContainer.h xAODTrigL1Calo/CMMEtSumsAuxContainer.h xAODTrigL1Calo/CMMJetHitsContainer.h xAODTrigL1Calo/CMMJetHitsAuxContainer.h xAODTrigL1Calo/CMMRoI.h xAODTrigL1Calo/CMMRoIAuxInfo.h xAODTrigL1Calo/JetElementContainer.h xAODTrigL1Calo/JetElementAuxContainer.h xAODTrigL1Calo/RODHeaderContainer.h xAODTrigL1Calo/RODHeaderAuxContainer.h xAODTrigL1Calo/JGTowerContainer.h xAODTrigL1Calo/JGTowerAuxContainer.h xAODTrigL1Calo/GBlockContainer.h xAODTrigL1Calo/GBlockAuxContainer xAODTrigL1Calo/TriggerTowerContainer.h xAODTrigL1Calo/TriggerTowerAuxContainer.h xAODTrigL1Calo/CMXCPHitsContainer.h xAODTrigL1Calo/CMXCPHitsAuxContainer.h xAODTrigL1Calo/CMXCPTobContainer.h xAODTrigL1Calo/CMXCPTobAuxContainer.h xAODTrigL1Calo/CMXJetHitsContainer.h xAODTrigL1Calo/CMXJetHitsAuxContainer.h xAODTrigL1Calo/CMXJetTobContainer.h xAODTrigL1Calo/CMXJetTobAuxContainer.h xAODTrigL1Calo/CMXEtSumsContainer.h xAODTrigL1Calo/CMXEtSumsAuxContainer.h xAODTrigL1Calo/CMXRoIContainer.h xAODTrigL1Calo/CMXRoIAuxContainer.h xAODTrigL1Calo/CPMTobRoIContainer.h xAODTrigL1Calo/CPMTobRoIAuxContainer.h xAODTrigL1Calo/JEMTobRoIContainer.h xAODTrigL1Calo/JEMTobRoIAuxContainer.h xAODTrigL1Calo/L1TopoRawDataContainer.h xAODTrigL1Calo/L1TopoRawDataAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::JEMHitsContainer xAOD::JEMHitsAuxContainer xAOD::JEMEtSumsContainer xAOD::JEMEtSumsAuxContainer xAOD::JEMRoIContainer xAOD::JEMRoIAuxContainer xAOD::CPMHitsContainer xAOD::CPMHitsAuxContainer xAOD::CPMTowerContainer xAOD::CPMTowerAuxContainer xAOD::CPMRoIContainer xAOD::CPMRoIAuxContainer xAOD::CMMCPHitsContainer xAOD::CMMCPHitsAuxContainer xAOD::CMMEtSumsContainer xAOD::CMMEtSumsAuxContainer xAOD::CMMJetHitsContainer xAOD::CMMJetHitsAuxContainer xAOD::CMMRoI xAOD::CMMRoIAuxInfo xAOD::JetElementContainer xAOD::JetElementAuxContainer xAOD::RODHeaderContainer xAOD::RODHeaderAuxContainer xAOD::JGTowerContainer xAOD::JGTowerAuxContainer xAOD::GBlockContainer xAOD::GBlockAuxContainer xAOD::TriggerTowerContainer xAOD::TriggerTowerAuxContainer xAOD::CMXCPHitsContainer xAOD::CMXCPHitsAuxContainer xAOD::CMXCPTobContainer xAOD::CMXCPTobAuxContainer xAOD::CMXJetHitsContainer xAOD::CMXJetHitsAuxContainer xAOD::CMXJetTobContainer xAOD::CMXJetTobAuxContainer xAOD::CMXEtSumsContainer xAOD::CMXEtSumsAuxContainer xAOD::CMXRoIContainer xAOD::CMXRoIAuxContainer xAOD::CPMTobRoIContainer xAOD::CPMTobRoIAuxContainer xAOD::JEMTobRoIContainer xAOD::JEMTobRoIAuxContainer xAOD::L1TopoRawDataContainer xAOD::L1TopoRawDataAuxContainer
                           CNV_PFX xAOD
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
26	                           LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTrigL1Calo )
