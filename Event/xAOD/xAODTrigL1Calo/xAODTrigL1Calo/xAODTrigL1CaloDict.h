// Dear emacs, this is -*- c++ -*-

/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigL1CaloDict.h 804221 2017-05-03 16:06:15Z cylin $
#ifndef XAODTRIGL1CALO_XAODTRIGL1CALOCALOEVENTDICT_H
#define XAODTRIGL1CALO_XAODTRIGL1CALOCALOEVENTDICT_H

// System include(s):
#include <vector>

// Local include(s):
// Run 1
#include "xAODTrigL1Calo/versions/JEMHits_v1.h"
#include "xAODTrigL1Calo/versions/JEMHitsContainer_v1.h"
#include "xAODTrigL1Calo/versions/JEMHitsAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/JEMEtSums_v1.h"
#include "xAODTrigL1Calo/versions/JEMEtSumsContainer_v1.h"
#include "xAODTrigL1Calo/versions/JEMEtSumsAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/JEMRoI_v1.h"
#include "xAODTrigL1Calo/versions/JEMRoIContainer_v1.h"
#include "xAODTrigL1Calo/versions/JEMRoIAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/CPMHits_v1.h"
#include "xAODTrigL1Calo/versions/CPMHitsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CPMHitsAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/CPMTower_v1.h"
#include "xAODTrigL1Calo/versions/CPMTowerContainer_v1.h"
#include "xAODTrigL1Calo/versions/CPMTowerAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/CPMRoI_v1.h"
#include "xAODTrigL1Calo/versions/CPMRoIContainer_v1.h"
#include "xAODTrigL1Calo/versions/CPMRoIAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/CMMCPHits_v1.h"
#include "xAODTrigL1Calo/versions/CMMCPHitsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMMCPHitsAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/CMMEtSums_v1.h"
#include "xAODTrigL1Calo/versions/CMMEtSumsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMMEtSumsAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/CMMJetHits_v1.h"
#include "xAODTrigL1Calo/versions/CMMJetHitsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMMJetHitsAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/CMMRoI_v1.h"
#include "xAODTrigL1Calo/versions/CMMRoIAuxInfo_v1.h"

#include "xAODTrigL1Calo/versions/JetElement_v1.h"
#include "xAODTrigL1Calo/versions/JetElementContainer_v1.h"
#include "xAODTrigL1Calo/versions/JetElementAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/RODHeader_v1.h"
#include "xAODTrigL1Calo/versions/RODHeaderContainer_v1.h"
#include "xAODTrigL1Calo/versions/RODHeaderAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/RODHeader_v2.h"
#include "xAODTrigL1Calo/versions/RODHeaderContainer_v2.h"
#include "xAODTrigL1Calo/versions/RODHeaderAuxContainer_v2.h"

#include "xAODTrigL1Calo/versions/TriggerTower_v1.h"
#include "xAODTrigL1Calo/versions/TriggerTowerContainer_v1.h"
#include "xAODTrigL1Calo/versions/TriggerTowerAuxContainer_v1.h"

// Run 2

#include "xAODTrigL1Calo/versions/TriggerTower_v2.h"
#include "xAODTrigL1Calo/versions/TriggerTowerContainer_v2.h"
#include "xAODTrigL1Calo/versions/TriggerTowerAuxContainer_v2.h"

#include "xAODTrigL1Calo/versions/CMXCPHits_v1.h"
#include "xAODTrigL1Calo/versions/CMXCPHitsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMXCPHitsAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/CMXCPTob_v1.h"
#include "xAODTrigL1Calo/versions/CMXCPTobContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMXCPTobAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/CMXJetHits_v1.h"
#include "xAODTrigL1Calo/versions/CMXJetHitsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMXJetHitsAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/CMXJetTob_v1.h"
#include "xAODTrigL1Calo/versions/CMXJetTobContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMXJetTobAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/CMXEtSums_v1.h"
#include "xAODTrigL1Calo/versions/CMXEtSumsContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMXEtSumsAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/CMXRoI_v1.h"
#include "xAODTrigL1Calo/versions/CMXRoIContainer_v1.h"
#include "xAODTrigL1Calo/versions/CMXRoIAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/CPMTower_v2.h"
#include "xAODTrigL1Calo/versions/CPMTowerContainer_v2.h"
#include "xAODTrigL1Calo/versions/CPMTowerAuxContainer_v2.h"

#include "xAODTrigL1Calo/versions/CPMTobRoI_v1.h"
#include "xAODTrigL1Calo/versions/CPMTobRoIContainer_v1.h"
#include "xAODTrigL1Calo/versions/CPMTobRoIAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/JEMEtSums_v2.h"
#include "xAODTrigL1Calo/versions/JEMEtSumsContainer_v2.h"
#include "xAODTrigL1Calo/versions/JEMEtSumsAuxContainer_v2.h"

#include "xAODTrigL1Calo/versions/JEMTobRoI_v1.h"
#include "xAODTrigL1Calo/versions/JEMTobRoIContainer_v1.h"
#include "xAODTrigL1Calo/versions/JEMTobRoIAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/JetElement_v2.h"
#include "xAODTrigL1Calo/versions/JetElementContainer_v2.h"
#include "xAODTrigL1Calo/versions/JetElementAuxContainer_v2.h"

#include "xAODTrigL1Calo/versions/L1TopoRawData_v1.h"
#include "xAODTrigL1Calo/versions/L1TopoRawDataContainer_v1.h"
#include "xAODTrigL1Calo/versions/L1TopoRawDataAuxContainer_v1.h"

// Run 3

#include "xAODTrigL1Calo/versions/JGTower_v1.h"
#include "xAODTrigL1Calo/versions/JGTowerContainer_v1.h"
#include "xAODTrigL1Calo/versions/JGTowerAuxContainer_v1.h"

#include "xAODTrigL1Calo/versions/GBlock_v1.h"
#include "xAODTrigL1Calo/versions/GBlockContainer_v1.h"
#include "xAODTrigL1Calo/versions/GBlockAuxContainer_v1.h"

// Versionless AuxContainers
#include "xAODTrigL1Calo/CMMCPHitsAuxContainer.h"
#include "xAODTrigL1Calo/CMMEtSumsAuxContainer.h"
#include "xAODTrigL1Calo/CMMJetHitsAuxContainer.h"
#include "xAODTrigL1Calo/CMMRoIAuxInfo.h"
#include "xAODTrigL1Calo/CMXCPHitsAuxContainer.h"
#include "xAODTrigL1Calo/CMXCPTobAuxContainer.h"
#include "xAODTrigL1Calo/CMXEtSumsAuxContainer.h"
#include "xAODTrigL1Calo/CMXJetHitsAuxContainer.h"
#include "xAODTrigL1Calo/CMXJetTobAuxContainer.h"
#include "xAODTrigL1Calo/CMXRoIAuxContainer.h"
#include "xAODTrigL1Calo/CPMHitsAuxContainer.h"
#include "xAODTrigL1Calo/CPMRoIAuxContainer.h"
#include "xAODTrigL1Calo/CPMTobRoIAuxContainer.h"
#include "xAODTrigL1Calo/CPMTowerAuxContainer.h"
#include "xAODTrigL1Calo/JEMEtSumsAuxContainer.h"
#include "xAODTrigL1Calo/JEMHitsAuxContainer.h"
#include "xAODTrigL1Calo/JEMRoIAuxContainer.h"
#include "xAODTrigL1Calo/JEMTobRoIAuxContainer.h"
#include "xAODTrigL1Calo/JetElementAuxContainer.h"
#include "xAODTrigL1Calo/RODHeaderAuxContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"
#include "xAODTrigL1Calo/JGTowerAuxContainer.h"
#include "xAODTrigL1Calo/L1TopoRawDataAuxContainer.h"

namespace
{
  struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGL1CALO
  {

    // Run 1

    // JEMHits
    xAOD::JEMHitsContainer_v1 JEMHits_v1_c1;

    // JEMEtSums
    xAOD::JEMEtSumsContainer_v1 JEMEtSums_v1_c1;

    // JEMRoI
    xAOD::JEMRoIContainer_v1 JEMRoI_v1_c1;

    // CPMHits
    xAOD::CPMHitsContainer_v1 CPMHits_v1_c1;

    // CPMTower
    xAOD::CPMTowerContainer_v1 CPMTower_v1_c1;

    // CPMRoI
    xAOD::CPMRoIContainer_v1 CPMRoI_v1_c1;

    // CMMCPHits
    xAOD::CMMCPHitsContainer_v1 CMMCPHits_v1_c1;

    // CMMEtSums
    xAOD::CMMEtSumsContainer_v1 CMMEtSums_v1_c1;

    // CMMJetHits
    xAOD::CMMJetHitsContainer_v1 CMMJetHits_v1_c1;

    // JetElement
    xAOD::JetElementContainer_v1 JetElement_v1_c1;

    // RODHeader
    xAOD::RODHeaderContainer_v1 RODHeader_v1_c1;

    // TriggerTower
    xAOD::TriggerTowerContainer_v1 TriggerTower_v1_c1;

    // Run 2

    // TriggerTower

    xAOD::TriggerTowerContainer_v2 TriggerTower_v2_c1;

    //Run 3
    xAOD::JGTowerContainer_v1 JGTower_v1_c1;
    xAOD::GBlockContainer_v1 GBlock_v1_c1;

    // RODHeader
    xAOD::RODHeaderContainer_v2 RODHeader_v2_c1;

    // CMXCPHits
    xAOD::CMXCPHitsContainer_v1 CMXCPHits_v1_c1;

    // CMXCPTob
    xAOD::CMXCPTobContainer_v1 CMXCPTob_v1_c1;

    // CMXJetHits
    xAOD::CMXJetHitsContainer_v1 CMXJetHits_v1_c1;

    // CMXJetTob
    xAOD::CMXJetTobContainer_v1 CMXJetTob_v1_c1;

    // CMXEtSums
    xAOD::CMXEtSumsContainer_v1 CMXEtSums_v1_c1;

    // CMXRoI
    xAOD::CMXRoIContainer_v1 CMXRoI_v1_c1;

    // CPMTower
    xAOD::CPMTowerContainer_v2 CPMTower_v2_c1;

    // CPMTobRoI
    xAOD::CPMTobRoIContainer_v1 CPMTobRoI_v1_c1;

    // JEMEtSums
    xAOD::JEMEtSumsContainer_v2 JEMEtSums_v2_c1;

    // JEMTobRoI
    xAOD::JEMTobRoIContainer_v1 JEMTobRoI_v1_c1;

    // JetElement
    xAOD::JetElementContainer_v2 JetElement_v2_c1;

    // L1TopoRawData
    xAOD::L1TopoRawDataContainer_v1 L1TopoRawData_v1_c1;
  };
} // namespace

#endif // XAODTRIGL1CALO_XAODTRIGL1CALOCALOEVENTDICT_H
