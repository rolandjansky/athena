// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTRIGL1CALO_XAODTRIGL1CALOCALOEVENTDICT_H
#define XAODTRIGL1CALO_XAODTRIGL1CALOCALOEVENTDICT_H

// Local include(s):
// Run 1
#include "xAODTrigL1Calo/JEMHits.h"
#include "xAODTrigL1Calo/JEMHitsContainer.h"
#include "xAODTrigL1Calo/JEMHitsAuxContainer.h"
#include "xAODTrigL1Calo/versions/JEMHits_v1.h"
#include "xAODTrigL1Calo/versions/JEMHitsContainer_v1.h"
#include "xAODTrigL1Calo/versions/JEMHitsAuxContainer_v1.h"

#include "xAODTrigL1Calo/JEMEtSums.h"
#include "xAODTrigL1Calo/JEMEtSumsContainer.h"
#include "xAODTrigL1Calo/JEMEtSumsAuxContainer.h"
#include "xAODTrigL1Calo/versions/JEMEtSums_v1.h"
#include "xAODTrigL1Calo/versions/JEMEtSumsContainer_v1.h"
#include "xAODTrigL1Calo/versions/JEMEtSumsAuxContainer_v1.h"

#include "xAODTrigL1Calo/JEMRoI.h"
#include "xAODTrigL1Calo/JEMRoIContainer.h"
#include "xAODTrigL1Calo/JEMRoIAuxContainer.h"
#include "xAODTrigL1Calo/versions/JEMRoI_v1.h"
#include "xAODTrigL1Calo/versions/JEMRoIContainer_v1.h"
#include "xAODTrigL1Calo/versions/JEMRoIAuxContainer_v1.h"

#include "xAODTrigL1Calo/CPMHits.h"
#include "xAODTrigL1Calo/CPMHitsContainer.h"
#include "xAODTrigL1Calo/CPMHitsAuxContainer.h"
#include "xAODTrigL1Calo/versions/CPMHits_v1.h"
#include "xAODTrigL1Calo/versions/CPMHitsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CPMHitsAuxContainer_v1.h"

#include "xAODTrigL1Calo/CPMTower.h"
#include "xAODTrigL1Calo/CPMTowerContainer.h"
#include "xAODTrigL1Calo/CPMTowerAuxContainer.h"
#include "xAODTrigL1Calo/versions/CPMTower_v1.h"
#include "xAODTrigL1Calo/versions/CPMTowerContainer_v1.h"
#include "xAODTrigL1Calo/versions/CPMTowerAuxContainer_v1.h"

#include "xAODTrigL1Calo/CPMRoI.h"
#include "xAODTrigL1Calo/CPMRoIContainer.h"
#include "xAODTrigL1Calo/CPMRoIAuxContainer.h"
#include "xAODTrigL1Calo/versions/CPMRoI_v1.h"
#include "xAODTrigL1Calo/versions/CPMRoIContainer_v1.h"
#include "xAODTrigL1Calo/versions/CPMRoIAuxContainer_v1.h"

#include "xAODTrigL1Calo/CMMCPHits.h"
#include "xAODTrigL1Calo/CMMCPHitsContainer.h"
#include "xAODTrigL1Calo/CMMCPHitsAuxContainer.h"
#include "xAODTrigL1Calo/versions/CMMCPHits_v1.h"
#include "xAODTrigL1Calo/versions/CMMCPHitsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMMCPHitsAuxContainer_v1.h"

#include "xAODTrigL1Calo/CMMEtSums.h"
#include "xAODTrigL1Calo/CMMEtSumsContainer.h"
#include "xAODTrigL1Calo/CMMEtSumsAuxContainer.h"
#include "xAODTrigL1Calo/versions/CMMEtSums_v1.h"
#include "xAODTrigL1Calo/versions/CMMEtSumsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMMEtSumsAuxContainer_v1.h"

#include "xAODTrigL1Calo/CMMJetHits.h"
#include "xAODTrigL1Calo/CMMJetHitsContainer.h"
#include "xAODTrigL1Calo/CMMJetHitsAuxContainer.h"
#include "xAODTrigL1Calo/versions/CMMJetHits_v1.h"
#include "xAODTrigL1Calo/versions/CMMJetHitsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMMJetHitsAuxContainer_v1.h"

#include "xAODTrigL1Calo/CMMRoI.h"
#include "xAODTrigL1Calo/CMMRoIAuxInfo.h"
#include "xAODTrigL1Calo/versions/CMMRoI_v1.h"
#include "xAODTrigL1Calo/versions/CMMRoIAuxInfo_v1.h"

#include "xAODTrigL1Calo/JetElement.h"
#include "xAODTrigL1Calo/JetElementContainer.h"
#include "xAODTrigL1Calo/JetElementAuxContainer.h"
#include "xAODTrigL1Calo/versions/JetElement_v1.h"
#include "xAODTrigL1Calo/versions/JetElementContainer_v1.h"
#include "xAODTrigL1Calo/versions/JetElementAuxContainer_v1.h"

#include "xAODTrigL1Calo/RODHeader.h"
#include "xAODTrigL1Calo/RODHeaderContainer.h"
#include "xAODTrigL1Calo/RODHeaderAuxContainer.h"
#include "xAODTrigL1Calo/versions/RODHeader_v1.h"
#include "xAODTrigL1Calo/versions/RODHeaderContainer_v1.h"
#include "xAODTrigL1Calo/versions/RODHeaderAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/RODHeader_v2.h"
#include "xAODTrigL1Calo/versions/RODHeaderContainer_v2.h"
#include "xAODTrigL1Calo/versions/RODHeaderAuxContainer_v2.h"

#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"
#include "xAODTrigL1Calo/versions/TriggerTower_v1.h"
#include "xAODTrigL1Calo/versions/TriggerTowerContainer_v1.h"
#include "xAODTrigL1Calo/versions/TriggerTowerAuxContainer_v1.h"

// Run 2
#include "xAODTrigL1Calo/versions/TriggerTower_v2.h"
#include "xAODTrigL1Calo/versions/TriggerTowerContainer_v2.h"
#include "xAODTrigL1Calo/versions/TriggerTowerAuxContainer_v2.h"

#include "xAODTrigL1Calo/CMXCPHits.h"
#include "xAODTrigL1Calo/CMXCPHitsContainer.h"
#include "xAODTrigL1Calo/CMXCPHitsAuxContainer.h"
#include "xAODTrigL1Calo/versions/CMXCPHits_v1.h"
#include "xAODTrigL1Calo/versions/CMXCPHitsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMXCPHitsAuxContainer_v1.h"

#include "xAODTrigL1Calo/CMXCPTob.h"
#include "xAODTrigL1Calo/CMXCPTobContainer.h"
#include "xAODTrigL1Calo/CMXCPTobAuxContainer.h"
#include "xAODTrigL1Calo/versions/CMXCPTob_v1.h"
#include "xAODTrigL1Calo/versions/CMXCPTobContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMXCPTobAuxContainer_v1.h"

#include "xAODTrigL1Calo/CMXJetHits.h"
#include "xAODTrigL1Calo/CMXJetHitsContainer.h"
#include "xAODTrigL1Calo/CMXJetHitsAuxContainer.h"
#include "xAODTrigL1Calo/versions/CMXJetHits_v1.h"
#include "xAODTrigL1Calo/versions/CMXJetHitsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMXJetHitsAuxContainer_v1.h"

#include "xAODTrigL1Calo/CMXJetTob.h"
#include "xAODTrigL1Calo/CMXJetTobContainer.h"
#include "xAODTrigL1Calo/CMXJetTobAuxContainer.h"
#include "xAODTrigL1Calo/versions/CMXJetTob_v1.h"
#include "xAODTrigL1Calo/versions/CMXJetTobContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMXJetTobAuxContainer_v1.h"

#include "xAODTrigL1Calo/CMXEtSums.h"
#include "xAODTrigL1Calo/CMXEtSumsContainer.h"
#include "xAODTrigL1Calo/CMXEtSumsAuxContainer.h"
#include "xAODTrigL1Calo/versions/CMXEtSums_v1.h"
#include "xAODTrigL1Calo/versions/CMXEtSumsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMXEtSumsAuxContainer_v1.h"

#include "xAODTrigL1Calo/CMXRoI.h"
#include "xAODTrigL1Calo/CMXRoIContainer.h"
#include "xAODTrigL1Calo/CMXRoIAuxContainer.h"
#include "xAODTrigL1Calo/versions/CMXRoI_v1.h"
#include "xAODTrigL1Calo/versions/CMXRoIContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMXRoIAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/CPMTower_v2.h"
#include "xAODTrigL1Calo/versions/CPMTowerContainer_v2.h"
#include "xAODTrigL1Calo/versions/CPMTowerAuxContainer_v2.h"

#include "xAODTrigL1Calo/CPMTobRoI.h"
#include "xAODTrigL1Calo/CPMTobRoIContainer.h"
#include "xAODTrigL1Calo/CPMTobRoIAuxContainer.h"
#include "xAODTrigL1Calo/versions/CPMTobRoI_v1.h"
#include "xAODTrigL1Calo/versions/CPMTobRoIContainer_v1.h"
#include "xAODTrigL1Calo/versions/CPMTobRoIAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/JEMEtSums_v2.h"
#include "xAODTrigL1Calo/versions/JEMEtSumsContainer_v2.h"
#include "xAODTrigL1Calo/versions/JEMEtSumsAuxContainer_v2.h"

#include "xAODTrigL1Calo/JEMTobRoI.h"
#include "xAODTrigL1Calo/JEMTobRoIContainer.h"
#include "xAODTrigL1Calo/JEMTobRoIAuxContainer.h"
#include "xAODTrigL1Calo/versions/JEMTobRoI_v1.h"
#include "xAODTrigL1Calo/versions/JEMTobRoIContainer_v1.h"
#include "xAODTrigL1Calo/versions/JEMTobRoIAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/JetElement_v2.h"
#include "xAODTrigL1Calo/versions/JetElementContainer_v2.h"
#include "xAODTrigL1Calo/versions/JetElementAuxContainer_v2.h"

#include "xAODTrigL1Calo/L1TopoRawData.h"
#include "xAODTrigL1Calo/L1TopoRawDataContainer.h"
#include "xAODTrigL1Calo/L1TopoRawDataAuxContainer.h"
#include "xAODTrigL1Calo/versions/L1TopoRawData_v1.h"
#include "xAODTrigL1Calo/versions/L1TopoRawDataContainer_v1.h"
#include "xAODTrigL1Calo/versions/L1TopoRawDataAuxContainer_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
  struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGL1CALO {

    // Run 1

    // JEMHits
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, JEMHitsContainer_v1 );
    // JEMEtSums
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, JEMEtSumsContainer_v1 );
    // JEMRoI
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, JEMRoIContainer_v1 );
    // CPMHits
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CPMHitsContainer_v1 );
    // CPMTower
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CPMTowerContainer_v1 );
    // CPMRoI
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CPMRoIContainer_v1 );
    // CMMCPHits
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CMMCPHitsContainer_v1 );
    // CMMEtSums
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CMMEtSumsContainer_v1 );
    // CMMJetHits
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CMMJetHitsContainer_v1 );
    // JetElement
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, JetElementContainer_v1 );
    // RODHeader
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, RODHeaderContainer_v1 );
    // TriggerTower
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TriggerTowerContainer_v1 );
    // CMMRoI
    XAOD_INSTANTIATE_NS_OBJECT_TYPES( xAOD, CMMRoI_v1 );

    // Run 2

    // TriggerTower
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TriggerTowerContainer_v2 );
    // RODHeader
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, RODHeaderContainer_v2 );
    // CMXCPHits
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CMXCPHitsContainer_v1 );
    // CMXCPTob
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CMXCPTobContainer_v1 );
    // CMXJetHits
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CMXJetHitsContainer_v1 );
    // CMXJetTob
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CMXJetTobContainer_v1 );
    // CMXEtSums
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CMXEtSumsContainer_v1 );
    // CMXRoI
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CMXRoIContainer_v1 );
    // CPMTower
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CPMTowerContainer_v2 );
    // CPMTobRoI
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CPMTobRoIContainer_v1 );
    // JEMEtSums
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, JEMEtSumsContainer_v2 );
    // JEMTobRoI
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, JEMTobRoIContainer_v1 );
    // JetElement
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, JetElementContainer_v2 );
    // L1TopoRawData
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, L1TopoRawDataContainer_v1 );
  };
}

#endif // XAODTRIGL1CALO_XAODTRIGL1CALOCALOEVENTDICT_H
