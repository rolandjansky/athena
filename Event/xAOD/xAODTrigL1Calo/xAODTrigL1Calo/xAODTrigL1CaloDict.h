// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigL1CaloDict.h 694350 2015-09-11 12:02:19Z krasznaa $
#ifndef XAODTRIGL1CALO_XAODTRIGL1CALOCALOEVENTDICT_H
#define XAODTRIGL1CALO_XAODTRIGL1CALOCALOEVENTDICT_H

// System include(s):
#include <vector>

// EDM include(s):
/*
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"
*/

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
#include "xAODTrigL1Calo/L1TopoRawDataAuxContainer.h"


namespace {
  struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGL1CALO {
      
    // Run 1
    
    // JEMHits  
    xAOD::JEMHitsContainer_v1 JEMHits_v1_c1;
     /*
    DataLink< xAOD::JEMHitsContainer_v1 > JEMHits_v1_l1;
    ElementLink< xAOD::JEMHitsContainer_v1 > JEMHits_v1_l2;
    ElementLinkVector< xAOD::JEMHitsContainer_v1 > JEMHits_v1_l3;
    std::vector< DataLink< xAOD::JEMHitsContainer_v1 > > JEMHits_v1_l4;
    std::vector< ElementLink< xAOD::JEMHitsContainer_v1 > > JEMHits_v1_l5;
    std::vector< ElementLinkVector< xAOD::JEMHitsContainer_v1 > > JEMHits_v1_l6;    
     */

    // JEMEtSums
    xAOD::JEMEtSumsContainer_v1 JEMEtSums_v1_c1;
     /*
    DataLink< xAOD::JEMEtSumsContainer_v1 > JEMEtSums_v1_l1;
    ElementLink< xAOD::JEMEtSumsContainer_v1 > JEMEtSums_v1_l2;
    ElementLinkVector< xAOD::JEMEtSumsContainer_v1 > JEMEtSums_v1_l3;
    std::vector< DataLink< xAOD::JEMEtSumsContainer_v1 > > JEMEtSums_v1_l4;
    std::vector< ElementLink< xAOD::JEMEtSumsContainer_v1 > > JEMEtSums_v1_l5;
    std::vector< ElementLinkVector< xAOD::JEMEtSumsContainer_v1 > > JEMEtSums_v1_l6;     
     */

    // JEMRoI   
    xAOD::JEMRoIContainer_v1 JEMRoI_v1_c1;
     /*
    DataLink< xAOD::JEMRoIContainer_v1 > JEMRoI_v1_l1;
    ElementLink< xAOD::JEMRoIContainer_v1 > JEMRoI_v1_l2;
    ElementLinkVector< xAOD::JEMRoIContainer_v1 > JEMRoI_v1_l3;
    std::vector< DataLink< xAOD::JEMRoIContainer_v1 > > JEMRoI_v1_l4;
    std::vector< ElementLink< xAOD::JEMRoIContainer_v1 > > JEMRoI_v1_l5;
    std::vector< ElementLinkVector< xAOD::JEMRoIContainer_v1 > > JEMRoI_v1_l6;      \
     */

    // CPMHits  
    xAOD::CPMHitsContainer_v1 CPMHits_v1_c1;
     /*
    DataLink< xAOD::CPMHitsContainer_v1 > CPMHits_v1_l1;
    ElementLink< xAOD::CPMHitsContainer_v1 > CPMHits_v1_l2;
    ElementLinkVector< xAOD::CPMHitsContainer_v1 > CPMHits_v1_l3;
    std::vector< DataLink< xAOD::CPMHitsContainer_v1 > > CPMHits_v1_l4;
    std::vector< ElementLink< xAOD::CPMHitsContainer_v1 > > CPMHits_v1_l5;
    std::vector< ElementLinkVector< xAOD::CPMHitsContainer_v1 > > CPMHits_v1_l6;      
     */
    
    // CPMTower  
    xAOD::CPMTowerContainer_v1 CPMTower_v1_c1;
     /*
    DataLink< xAOD::CPMTowerContainer_v1 > CPMTower_v1_l1;
    ElementLink< xAOD::CPMTowerContainer_v1 > CPMTower_v1_l2;
    ElementLinkVector< xAOD::CPMTowerContainer_v1 > CPMTower_v1_l3;
    std::vector< DataLink< xAOD::CPMTowerContainer_v1 > > CPMTower_v1_l4;
    std::vector< ElementLink< xAOD::CPMTowerContainer_v1 > > CPMTower_v1_l5;
    std::vector< ElementLinkVector< xAOD::CPMTowerContainer_v1 > > CPMTower_v1_l6;        
     */
    
    // CPMRoI 
    xAOD::CPMRoIContainer_v1 CPMRoI_v1_c1;
     /*
    DataLink< xAOD::CPMRoIContainer_v1 > CPMRoI_v1_l1;
    ElementLink< xAOD::CPMRoIContainer_v1 > CPMRoI_v1_l2;
    ElementLinkVector< xAOD::CPMRoIContainer_v1 > CPMRoI_v1_l3;
    std::vector< DataLink< xAOD::CPMRoIContainer_v1 > > CPMRoI_v1_l4;
    std::vector< ElementLink< xAOD::CPMRoIContainer_v1 > > CPMRoI_v1_l5;
    std::vector< ElementLinkVector< xAOD::CPMRoIContainer_v1 > > CPMRoI_v1_l6;      
     */

    // CMMCPHits
    xAOD::CMMCPHitsContainer_v1 CMMCPHits_v1_c1;
     /*
    DataLink< xAOD::CMMCPHitsContainer_v1 > CMMCPHits_v1_l1;
    ElementLink< xAOD::CMMCPHitsContainer_v1 > CMMCPHits_v1_l2;
    ElementLinkVector< xAOD::CMMCPHitsContainer_v1 > CMMCPHits_v1_l3;
    std::vector< DataLink< xAOD::CMMCPHitsContainer_v1 > > CMMCPHits_v1_l4;
    std::vector< ElementLink< xAOD::CMMCPHitsContainer_v1 > > CMMCPHits_v1_l5;
    std::vector< ElementLinkVector< xAOD::CMMCPHitsContainer_v1 > > CMMCPHits_v1_l6;     
     */
    
    // CMMEtSums 
    xAOD::CMMEtSumsContainer_v1 CMMEtSums_v1_c1;
     /*
    DataLink< xAOD::CMMEtSumsContainer_v1 > CMMEtSums_v1_l1;
    ElementLink< xAOD::CMMEtSumsContainer_v1 > CMMEtSums_v1_l2;
    ElementLinkVector< xAOD::CMMEtSumsContainer_v1 > CMMEtSums_v1_l3;
    std::vector< DataLink< xAOD::CMMEtSumsContainer_v1 > > CMMEtSums_v1_l4;
    std::vector< ElementLink< xAOD::CMMEtSumsContainer_v1 > > CMMEtSums_v1_l5;
    std::vector< ElementLinkVector< xAOD::CMMEtSumsContainer_v1 > > CMMEtSums_v1_l6;    
     */
    
    // CMMJetHits   
    xAOD::CMMJetHitsContainer_v1 CMMJetHits_v1_c1;
     /*
    DataLink< xAOD::CMMJetHitsContainer_v1 > CMMJetHits_v1_l1;
    ElementLink< xAOD::CMMJetHitsContainer_v1 > CMMJetHits_v1_l2;
    ElementLinkVector< xAOD::CMMJetHitsContainer_v1 > CMMJetHits_v1_l3;
    std::vector< DataLink< xAOD::CMMJetHitsContainer_v1 > > CMMJetHits_v1_l4;
    std::vector< ElementLink< xAOD::CMMJetHitsContainer_v1 > > CMMJetHits_v1_l5;
    std::vector< ElementLinkVector< xAOD::CMMJetHitsContainer_v1 > > CMMJetHits_v1_l6;        
     */
        
    // JetElement  
    xAOD::JetElementContainer_v1 JetElement_v1_c1;
     /*
    DataLink< xAOD::JetElementContainer_v1 > JetElement_v1_l1;
    ElementLink< xAOD::JetElementContainer_v1 > JetElement_v1_l2;
    ElementLinkVector< xAOD::JetElementContainer_v1 > JetElement_v1_l3;
    std::vector< DataLink< xAOD::JetElementContainer_v1 > > JetElement_v1_l4;
    std::vector< ElementLink< xAOD::JetElementContainer_v1 > > JetElement_v1_l5;
    std::vector< ElementLinkVector< xAOD::JetElementContainer_v1 > > JetElement_v1_l6;        
     */
    
    // RODHeader  
    xAOD::RODHeaderContainer_v1 RODHeader_v1_c1;
     /*
    DataLink< xAOD::RODHeaderContainer_v1 > RODHeader_v1_l1;
    ElementLink< xAOD::RODHeaderContainer_v1 > RODHeader_v1_l2;
    ElementLinkVector< xAOD::RODHeaderContainer_v1 > RODHeader_v1_l3;
    std::vector< DataLink< xAOD::RODHeaderContainer_v1 > > RODHeader_v1_l4;
    std::vector< ElementLink< xAOD::RODHeaderContainer_v1 > > RODHeader_v1_l5;
    std::vector< ElementLinkVector< xAOD::RODHeaderContainer_v1 > > RODHeader_v1_l6;      
     */
    
    // TriggerTower 
    xAOD::TriggerTowerContainer_v1 TriggerTower_v1_c1;
     /*
    DataLink< xAOD::TriggerTowerContainer_v1 > TriggerTower_v1_l1;
    ElementLink< xAOD::TriggerTowerContainer_v1 > TriggerTower_v1_l2;
    ElementLinkVector< xAOD::TriggerTowerContainer_v1 > TriggerTower_v1_l3;
    std::vector< DataLink< xAOD::TriggerTowerContainer_v1 > > TriggerTower_v1_l4;
    std::vector< ElementLink< xAOD::TriggerTowerContainer_v1 > > TriggerTower_v1_l5;
    std::vector< ElementLinkVector< xAOD::TriggerTowerContainer_v1 > > TriggerTower_v1_l6;      
     */
    
    // Run 2
    
    // TriggerTower
    xAOD::TriggerTowerContainer_v2 TriggerTower_v2_c1;
     /*
    DataLink< xAOD::TriggerTowerContainer_v2 > TriggerTower_v2_l1;
    ElementLink< xAOD::TriggerTowerContainer_v2 > TriggerTower_v2_l2;
    ElementLinkVector< xAOD::TriggerTowerContainer_v2 > TriggerTower_v2_l3;
    std::vector< DataLink< xAOD::TriggerTowerContainer_v2 > > TriggerTower_v2_l4;
    std::vector< ElementLink< xAOD::TriggerTowerContainer_v2 > > TriggerTower_v2_l5;
    std::vector< ElementLinkVector< xAOD::TriggerTowerContainer_v2 > > TriggerTower_v2_l6;
     */

    // RODHeader  
    xAOD::RODHeaderContainer_v2 RODHeader_v2_c1;
     /*
    DataLink< xAOD::RODHeaderContainer_v2 > RODHeader_v2_l1;
    ElementLink< xAOD::RODHeaderContainer_v2 > RODHeader_v2_l2;
    ElementLinkVector< xAOD::RODHeaderContainer_v2 > RODHeader_v2_l3;
    std::vector< DataLink< xAOD::RODHeaderContainer_v2 > > RODHeader_v2_l4;
    std::vector< ElementLink< xAOD::RODHeaderContainer_v2 > > RODHeader_v2_l5;
    std::vector< ElementLinkVector< xAOD::RODHeaderContainer_v2 > > RODHeader_v2_l6;
     */

    // CMXCPHits
    xAOD::CMXCPHitsContainer_v1 CMXCPHits_v1_c1;
     /*
    DataLink< xAOD::CMXCPHitsContainer_v1 > CMXCPHits_v1_l1;
    ElementLink< xAOD::CMXCPHitsContainer_v1 > CMXCPHits_v1_l2;
    ElementLinkVector< xAOD::CMXCPHitsContainer_v1 > CMXCPHits_v1_l3;
    std::vector< DataLink< xAOD::CMXCPHitsContainer_v1 > > CMXCPHits_v1_l4;
    std::vector< ElementLink< xAOD::CMXCPHitsContainer_v1 > > CMXCPHits_v1_l5;
    std::vector< ElementLinkVector< xAOD::CMXCPHitsContainer_v1 > > CMXCPHits_v1_l6;    
     */
    
    // CMXCPTob
    xAOD::CMXCPTobContainer_v1 CMXCPTob_v1_c1;
     /*
    DataLink< xAOD::CMXCPTobContainer_v1 > CMXCPTob_v1_l1;
    ElementLink< xAOD::CMXCPTobContainer_v1 > CMXCPTob_v1_l2;
    ElementLinkVector< xAOD::CMXCPTobContainer_v1 > CMXCPTob_v1_l3;
    std::vector< DataLink< xAOD::CMXCPTobContainer_v1 > > CMXCPTob_v1_l4;
    std::vector< ElementLink< xAOD::CMXCPTobContainer_v1 > > CMXCPTob_v1_l5;
    std::vector< ElementLinkVector< xAOD::CMXCPTobContainer_v1 > > CMXCPTob_v1_l6;     
     */
         
    // CMXJetHits
    xAOD::CMXJetHitsContainer_v1 CMXJetHits_v1_c1;
     /*
    DataLink< xAOD::CMXJetHitsContainer_v1 > CMXJetHits_v1_l1;
    ElementLink< xAOD::CMXJetHitsContainer_v1 > CMXJetHits_v1_l2;
    ElementLinkVector< xAOD::CMXJetHitsContainer_v1 > CMXJetHits_v1_l3;
    std::vector< DataLink< xAOD::CMXJetHitsContainer_v1 > > CMXJetHits_v1_l4;
    std::vector< ElementLink< xAOD::CMXJetHitsContainer_v1 > > CMXJetHits_v1_l5;
    std::vector< ElementLinkVector< xAOD::CMXJetHitsContainer_v1 > > CMXJetHits_v1_l6;      
     */

    // CMXJetTob
    xAOD::CMXJetTobContainer_v1 CMXJetTob_v1_c1;
     /*
    DataLink< xAOD::CMXJetTobContainer_v1 > CMXJetTob_v1_l1;
    ElementLink< xAOD::CMXJetTobContainer_v1 > CMXJetTob_v1_l2;
    ElementLinkVector< xAOD::CMXJetTobContainer_v1 > CMXJetTob_v1_l3;
    std::vector< DataLink< xAOD::CMXJetTobContainer_v1 > > CMXJetTob_v1_l4;
    std::vector< ElementLink< xAOD::CMXJetTobContainer_v1 > > CMXJetTob_v1_l5;
    std::vector< ElementLinkVector< xAOD::CMXJetTobContainer_v1 > > CMXJetTob_v1_l6;    
     */

    // CMXEtSums
    xAOD::CMXEtSumsContainer_v1 CMXEtSums_v1_c1;
     /*
    DataLink< xAOD::CMXEtSumsContainer_v1 > CMXEtSums_v1_l1;
    ElementLink< xAOD::CMXEtSumsContainer_v1 > CMXEtSums_v1_l2;
    ElementLinkVector< xAOD::CMXEtSumsContainer_v1 > CMXEtSums_v1_l3;
    std::vector< DataLink< xAOD::CMXEtSumsContainer_v1 > > CMXEtSums_v1_l4;
    std::vector< ElementLink< xAOD::CMXEtSumsContainer_v1 > > CMXEtSums_v1_l5;
    std::vector< ElementLinkVector< xAOD::CMXEtSumsContainer_v1 > > CMXEtSums_v1_l6;  
     */
    
    // CMXRoI
    xAOD::CMXRoIContainer_v1 CMXRoI_v1_c1;
     /*
    DataLink< xAOD::CMXRoIContainer_v1 > CMXRoI_v1_l1;
    ElementLink< xAOD::CMXRoIContainer_v1 > CMXRoI_v1_l2;
    ElementLinkVector< xAOD::CMXRoIContainer_v1 > CMXRoI_v1_l3;
    std::vector< DataLink< xAOD::CMXRoIContainer_v1 > > CMXRoI_v1_l4;
    std::vector< ElementLink< xAOD::CMXRoIContainer_v1 > > CMXRoI_v1_l5;
    std::vector< ElementLinkVector< xAOD::CMXRoIContainer_v1 > > CMXRoI_v1_l6;     
     */
   
    // CPMTower  
    xAOD::CPMTowerContainer_v2 CPMTower_v2_c1;
     /*
    DataLink< xAOD::CPMTowerContainer_v2 > CPMTower_v2_l1;
    ElementLink< xAOD::CPMTowerContainer_v2 > CPMTower_v2_l2;
    ElementLinkVector< xAOD::CPMTowerContainer_v2 > CPMTower_v2_l3;
    std::vector< DataLink< xAOD::CPMTowerContainer_v2 > > CPMTower_v2_l4;
    std::vector< ElementLink< xAOD::CPMTowerContainer_v2 > > CPMTower_v2_l5;
    std::vector< ElementLinkVector< xAOD::CPMTowerContainer_v2 > > CPMTower_v2_l6;     
     */
   
    // CPMTobRoI
    xAOD::CPMTobRoIContainer_v1 CPMTobRoI_v1_c1;
     /*
    DataLink< xAOD::CPMTobRoIContainer_v1 > CPMTobRoI_v1_l1;
    ElementLink< xAOD::CPMTobRoIContainer_v1 > CPMTobRoI_v1_l2;
    ElementLinkVector< xAOD::CPMTobRoIContainer_v1 > CPMTobRoI_v1_l3;
    std::vector< DataLink< xAOD::CPMTobRoIContainer_v1 > > CPMTobRoI_v1_l4;
    std::vector< ElementLink< xAOD::CPMTobRoIContainer_v1 > > CPMTobRoI_v1_l5;
    std::vector< ElementLinkVector< xAOD::CPMTobRoIContainer_v1 > > CPMTobRoI_v1_l6;     
     */

    // JEMEtSums
    xAOD::JEMEtSumsContainer_v2 JEMEtSums_v2_c1;
     /*
    DataLink< xAOD::JEMEtSumsContainer_v2 > JEMEtSums_v2_l1;
    ElementLink< xAOD::JEMEtSumsContainer_v2 > JEMEtSums_v2_l2;
    ElementLinkVector< xAOD::JEMEtSumsContainer_v2 > JEMEtSums_v2_l3;
    std::vector< DataLink< xAOD::JEMEtSumsContainer_v2 > > JEMEtSums_v2_l4;
    std::vector< ElementLink< xAOD::JEMEtSumsContainer_v2 > > JEMEtSums_v2_l5;
    std::vector< ElementLinkVector< xAOD::JEMEtSumsContainer_v2 > > JEMEtSums_v2_l6;     
     */

    // JEMTobRoI
    xAOD::JEMTobRoIContainer_v1 JEMTobRoI_v1_c1;
     /*
    DataLink< xAOD::JEMTobRoIContainer_v1 > JEMTobRoI_v1_l1;
    ElementLink< xAOD::JEMTobRoIContainer_v1 > JEMTobRoI_v1_l2;
    ElementLinkVector< xAOD::JEMTobRoIContainer_v1 > JEMTobRoI_v1_l3;
    std::vector< DataLink< xAOD::JEMTobRoIContainer_v1 > > JEMTobRoI_v1_l4;
    std::vector< ElementLink< xAOD::JEMTobRoIContainer_v1 > > JEMTobRoI_v1_l5;
    std::vector< ElementLinkVector< xAOD::JEMTobRoIContainer_v1 > > JEMTobRoI_v1_l6;        
     */
     
    // JetElement
    xAOD::JetElementContainer_v2 JetElement_v2_c1;
     /*
    DataLink< xAOD::JetElementContainer_v2 > JetElement_v2_l1;
    ElementLink< xAOD::JetElementContainer_v2 > JetElement_v2_l2;
    ElementLinkVector< xAOD::JetElementContainer_v2 > JetElement_v2_l3;
    std::vector< DataLink< xAOD::JetElementContainer_v2 > > JetElement_v2_l4;
    std::vector< ElementLink< xAOD::JetElementContainer_v2 > > JetElement_v2_l5;
    std::vector< ElementLinkVector< xAOD::JetElementContainer_v2 > > JetElement_v2_l6;
     */
    
    // L1TopoRawData
    xAOD::L1TopoRawDataContainer_v1 L1TopoRawData_v1_c1;
     /*
    DataLink< xAOD::L1TopoRawDataContainer_v1 > L1TopoRawData_v1_l1;
    ElementLink< xAOD::L1TopoRawDataContainer_v1 > L1TopoRawData_v1_l2;
    ElementLinkVector< xAOD::L1TopoRawDataContainer_v1 > L1TopoRawData_v1_l3;
    std::vector< DataLink< xAOD::L1TopoRawDataContainer_v1 > > L1TopoRawData_v1_l4;
    std::vector< ElementLink< xAOD::L1TopoRawDataContainer_v1 > > L1TopoRawData_v1_l5;
    std::vector< ElementLinkVector< xAOD::L1TopoRawDataContainer_v1 > > L1TopoRawData_v1_l6;    
     */
    
  };
}

#endif // XAODTRIGL1CALO_XAODTRIGL1CALOCALOEVENTDICT_H
