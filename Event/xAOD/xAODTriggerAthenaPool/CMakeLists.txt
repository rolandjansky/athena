# $Id: CMakeLists.txt 755768 2016-06-17 13:43:10Z krasznaa $
################################################################################
# Package: xAODTriggerAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTriggerAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Trigger/TrigEvent/TrigNavTools
   PRIVATE
   Control/AthContainers
   Control/AthenaKernel
   Database/AthenaPOOL/AthenaPoolCnvSvc
   Database/AthenaPOOL/AthenaPoolUtilities
   Event/xAOD/xAODTrigger )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTriggerAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODTrigger/MuonRoIContainer.h xAODTrigger/MuonRoIAuxContainer.h
   xAODTrigger/JetRoIContainer.h xAODTrigger/JetRoIAuxContainer.h
   xAODTrigger/EmTauRoIContainer.h xAODTrigger/EmTauRoIAuxContainer.h
   xAODTrigger/JetEtRoI.h xAODTrigger/JetEtRoIAuxInfo.h
   xAODTrigger/EnergySumRoI.h xAODTrigger/EnergySumRoIAuxInfo.h
   xAODTrigger/TrigCompositeContainer.h xAODTrigger/TrigCompositeAuxContainer.h
   xAODTrigger/TriggerMenuContainer.h xAODTrigger/TriggerMenuAuxContainer.h
   xAODTrigger/TrigConfKeys.h
   xAODTrigger/TrigDecision.h xAODTrigger/TrigDecisionAuxInfo.h
   xAODTrigger/TrigNavigation.h xAODTrigger/TrigNavigationAuxInfo.h
   xAODTrigger/RoiDescriptorStore.h xAODTrigger/RoiDescriptorStoreAuxInfo.h
   xAODTrigger/BunchConfKey.h
   xAODTrigger/BunchConfContainer.h xAODTrigger/BunchConfAuxContainer.h
   xAODTrigger/TrigPassBitsContainer.h xAODTrigger/TrigPassBitsAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::MuonRoIContainer xAOD::MuonRoIAuxContainer
   xAOD::JetRoIContainer xAOD::JetRoIAuxContainer
   xAOD::EmTauRoIContainer xAOD::EmTauRoIAuxContainer
   xAOD::JetEtRoI xAOD::JetEtRoIAuxInfo
   xAOD::EnergySumRoI xAOD::EnergySumRoIAuxInfo
   xAOD::TrigCompositeContainer xAOD::TrigCompositeAuxContainer
   xAOD::TriggerMenuContainer xAOD::TriggerMenuAuxContainer
   xAOD::TrigConfKeys
   xAOD::TrigDecision xAOD::TrigDecisionAuxInfo
   xAOD::TrigNavigation xAOD::TrigNavigationAuxInfo
   xAOD::RoiDescriptorStore xAOD::RoiDescriptorStoreAuxInfo
   xAOD::BunchConfKey
   xAOD::BunchConfContainer xAOD::BunchConfAuxContainer
   xAOD::TrigPassBitsContainer xAOD::TrigPassBitsAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES TrigNavToolsLib AthContainers AthenaKernel AthenaPoolCnvSvcLib
   AthenaPoolUtilities xAODTrigger )
