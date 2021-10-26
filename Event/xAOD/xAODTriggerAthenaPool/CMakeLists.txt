# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTriggerAthenaPool )

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
   xAODTrigger/TriggerMenuJsonContainer.h xAODTrigger/TriggerMenuJsonAuxContainer.h
   xAODTrigger/TrigConfKeys.h
   xAODTrigger/TrigDecision.h xAODTrigger/TrigDecisionAuxInfo.h
   xAODTrigger/TrigNavigation.h xAODTrigger/TrigNavigationAuxInfo.h
   xAODTrigger/RoiDescriptorStore.h xAODTrigger/RoiDescriptorStoreAuxInfo.h
   xAODTrigger/BunchConfKey.h
   xAODTrigger/BunchConfContainer.h xAODTrigger/BunchConfAuxContainer.h
   xAODTrigger/TrigPassBitsContainer.h xAODTrigger/TrigPassBitsAuxContainer.h
   xAODTrigger/eFexEMRoIContainer.h xAODTrigger/eFexEMRoIAuxContainer.h
   xAODTrigger/eFexTauRoIContainer.h xAODTrigger/eFexTauRoIAuxContainer.h
   xAODTrigger/jFexSRJetRoIContainer.h xAODTrigger/jFexSRJetRoIAuxContainer.h
   xAODTrigger/jFexLRJetRoIContainer.h xAODTrigger/jFexLRJetRoIAuxContainer.h
   xAODTrigger/jFexTauRoIContainer.h xAODTrigger/jFexTauRoIAuxContainer.h
   xAODTrigger/jFexSumETRoIContainer.h xAODTrigger/jFexSumETRoIAuxContainer.h
   xAODTrigger/jFexMETRoIContainer.h xAODTrigger/jFexMETRoIAuxContainer.h
   xAODTrigger/L1TopoSimResultsContainer.h xAODTrigger/L1TopoSimResultsAuxContainer.h
   xAODTrigger/gFexJetRoIContainer.h xAODTrigger/gFexJetRoIAuxContainer.h
   xAODTrigger/gFexGlobalRoIContainer.h xAODTrigger/gFexGlobalRoIAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::MuonRoIContainer xAOD::MuonRoIAuxContainer
   xAOD::JetRoIContainer xAOD::JetRoIAuxContainer
   xAOD::EmTauRoIContainer xAOD::EmTauRoIAuxContainer
   xAOD::JetEtRoI xAOD::JetEtRoIAuxInfo
   xAOD::EnergySumRoI xAOD::EnergySumRoIAuxInfo
   xAOD::TrigCompositeContainer xAOD::TrigCompositeAuxContainer
   xAOD::TriggerMenuContainer xAOD::TriggerMenuAuxContainer
   xAOD::TriggerMenuJsonContainer xAOD::TriggerMenuJsonAuxContainer
   xAOD::TrigConfKeys
   xAOD::TrigDecision xAOD::TrigDecisionAuxInfo
   xAOD::TrigNavigation xAOD::TrigNavigationAuxInfo
   xAOD::RoiDescriptorStore xAOD::RoiDescriptorStoreAuxInfo
   xAOD::BunchConfKey
   xAOD::BunchConfContainer xAOD::BunchConfAuxContainer
   xAOD::TrigPassBitsContainer xAOD::TrigPassBitsAuxContainer
   xAOD::eFexEMRoIContainer xAOD::eFexEMRoIAuxContainer
   xAOD::eFexTauRoIContainer xAOD::eFexTauRoIAuxContainer
   xAOD::jFexSRJetRoIContainer xAOD::jFexSRJetRoIAuxContainer
   xAOD::jFexLRJetRoIContainer xAOD::jFexLRJetRoIAuxContainer 
   xAOD::jFexTauRoIContainer xAOD::jFexTauRoIAuxContainer
   xAOD::jFexSumETRoIContainer xAOD::jFexSumETRoIAuxContainer
   xAOD::jFexMETRoIContainer xAOD::jFexMETRoIAuxContainer
   xAOD::L1TopoSimResultsContainer xAOD::L1TopoSimResultsAuxContainer
   xAOD::gFexJetRoIContainer xAOD::gFexJetRoIAuxContainer
   xAOD::gFexGlobalRoIContainer xAOD::gFexGlobalRoIAuxContainer

   CNV_PFX xAOD
   LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib
   AthenaPoolUtilities xAODTrigger )

# Install files from the package.
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODTRIGGERATHENAPOOL_REFERENCE_TAG
       xAODTriggerAthenaPoolReference-01-00-00 )
  run_tpcnv_test( xAODTriggerAthenaPool_master-20190911   AOD-master-20190911-full
                   REQUIRED_LIBRARIES xAODTriggerAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRIGGERATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( xAODTriggerAthenaPool_20.1.7.2   AOD-20.1.7.2-full
                   REQUIRED_LIBRARIES xAODTriggerAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRIGGERATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
