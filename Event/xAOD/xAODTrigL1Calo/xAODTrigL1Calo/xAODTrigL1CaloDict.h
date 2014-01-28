// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigL1CaloDict.h 576243 2013-12-19 11:24:27Z morrisj $
#ifndef XAODTRIGL1CALO_XAODTRIGL1CALOCALOEVENTDICT_H
#define XAODTRIGL1CALO_XAODTRIGL1CALOCALOEVENTDICT_H

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"

// Local include(s):
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

#include "xAODTrigL1Calo/versions/TriggerTower_v1.h"
#include "xAODTrigL1Calo/versions/TriggerTowerContainer_v1.h"
#include "xAODTrigL1Calo/versions/TriggerTowerAuxContainer_v1.h"

#ifdef __GCCXML__
namespace {
   struct GCCXML_DUMMY_INSTANTIATION {
      
      // JEMHits    
      // Container(s):
      xAOD::JEMHitsContainer_v1 JEMHits_c1;
      // Element link(s):
      ElementLink< xAOD::JEMHitsContainer_v1 > JEMHits_el1;
      std::vector< ElementLink< xAOD::JEMHitsContainer_v1 > > JEMHits_el2;
      // Element link vector(s):
      ElementLinkVector< xAOD::JEMHitsContainer_v1 > JEMHits_elv1;
      std::vector< ElementLinkVector< xAOD::JEMHitsContainer_v1 > > JEMHits_elv2;
      
      // JEMEtSums
      // Container(s):
      xAOD::JEMEtSumsContainer_v1 JEMEtSums_c1;
      // Element link(s):
      ElementLink< xAOD::JEMEtSumsContainer_v1 > JEMEtSums_el1;
      std::vector< ElementLink< xAOD::JEMEtSumsContainer_v1 > > JEMEtSums_el2;
      // Element link vector(s):
      ElementLinkVector< xAOD::JEMEtSumsContainer_v1 > JEMEtSums_elv1;
      std::vector< ElementLinkVector< xAOD::JEMEtSumsContainer_v1 > > JEMEtSums_elv2;      
 
      // JEMRoI    
      // Container(s):
      xAOD::JEMRoIContainer_v1 JEMRoI_c1;
      // Element link(s):
      ElementLink< xAOD::JEMRoIContainer_v1 > JEMRoI_el1;
      std::vector< ElementLink< xAOD::JEMRoIContainer_v1 > > JEMRoI_el2;
      // Element link vector(s):
      ElementLinkVector< xAOD::JEMRoIContainer_v1 > JEMRoI_elv1;
      std::vector< ElementLinkVector< xAOD::JEMRoIContainer_v1 > > JEMRoI_elv2;  
      
      
      // CPMHits    
      // Container(s):
      xAOD::CPMHitsContainer_v1 CPMHits_c1;
      // Element link(s):
      ElementLink< xAOD::CPMHitsContainer_v1 > CPMHits_el1;
      std::vector< ElementLink< xAOD::CPMHitsContainer_v1 > > CPMHits_el2;
      // Element link vector(s):
      ElementLinkVector< xAOD::CPMHitsContainer_v1 > CPMHits_elv1;
      std::vector< ElementLinkVector< xAOD::CPMHitsContainer_v1 > > CPMHits_elv2; 
      
      // CPMTower    
      // Container(s):
      xAOD::CPMTowerContainer_v1 CPMTower_c1;
      // Element link(s):
      ElementLink< xAOD::CPMTowerContainer_v1 > CPMTower_el1;
      std::vector< ElementLink< xAOD::CPMTowerContainer_v1 > > CPMTower_el2;
      // Element link vector(s):
      ElementLinkVector< xAOD::CPMTowerContainer_v1 > CPMTower_elv1;
      std::vector< ElementLinkVector< xAOD::CPMTowerContainer_v1 > > CPMTower_elv2;    
      
      // CPMRoI    
      // Container(s):
      xAOD::CPMRoIContainer_v1 CPMRoI_c1;
      // Element link(s):
      ElementLink< xAOD::CPMRoIContainer_v1 > CPMRoI_el1;
      std::vector< ElementLink< xAOD::CPMRoIContainer_v1 > > CPMRoI_el2;
      // Element link vector(s):
      ElementLinkVector< xAOD::CPMRoIContainer_v1 > CPMRoI_elv1;
      std::vector< ElementLinkVector< xAOD::CPMRoIContainer_v1 > > CPMRoI_elv2;  

      // CMMCPHits
      // Container(s):
      xAOD::CMMCPHitsContainer_v1 CMMCPHits_c1;
      // Data link(s):
      DataLink< xAOD::CMMCPHitsContainer_v1 > CMMCPHits_dl1;
      std::vector< DataLink< xAOD::CMMCPHitsContainer_v1 > > CMMCPHits_dl2;
      // Element link(s):
      ElementLink< xAOD::CMMCPHitsContainer_v1 > CMMCPHits_el1;
      std::vector< ElementLink< xAOD::CMMCPHitsContainer_v1 > > CMMCPHits_el2;
      std::vector< std::vector< ElementLink< xAOD::CMMCPHitsContainer_v1 > > > CMMCPHits_el3;
      
      // CMMEtSums    
      // Container(s):
      xAOD::CMMEtSumsContainer_v1 CMMEtSums_c1;
      // Element link(s):
      ElementLink< xAOD::CMMEtSumsContainer_v1 > CMMEtSums_el1;
      std::vector< ElementLink< xAOD::CMMEtSumsContainer_v1 > > CMMEtSums_el2;
      // Element link vector(s):
      ElementLinkVector< xAOD::CMMEtSumsContainer_v1 > CMMEtSums_elv1;
      std::vector< ElementLinkVector< xAOD::CMMEtSumsContainer_v1 > > CMMEtSums_elv2; 
      
      // CMMJetHits    
      // Container(s):
      xAOD::CMMJetHitsContainer_v1 CMMJetHits_c1;
      // Element link(s):
      ElementLink< xAOD::CMMJetHitsContainer_v1 > CMMJetHits_el1;
      std::vector< ElementLink< xAOD::CMMJetHitsContainer_v1 > > CMMJetHits_el2;
      // Element link vector(s):
      ElementLinkVector< xAOD::CMMJetHitsContainer_v1 > CMMJetHits_elv1;
      std::vector< ElementLinkVector< xAOD::CMMJetHitsContainer_v1 > > CMMJetHits_elv2; 
      
      
      // JetElement    
      // Container(s):
      xAOD::JetElementContainer_v1 JetElement_c1;
      // Element link(s):
      ElementLink< xAOD::JetElementContainer_v1 > JetElement_el1;
      std::vector< ElementLink< xAOD::JetElementContainer_v1 > > JetElement_el2;
      // Element link vector(s):
      ElementLinkVector< xAOD::JetElementContainer_v1 > JetElement_elv1;
      std::vector< ElementLinkVector< xAOD::JetElementContainer_v1 > > JetElement_elv2;   
      
      // RODHeader    
      // Container(s):
      xAOD::RODHeaderContainer_v1 RODHeader_c1;
      // Element link(s):
      ElementLink< xAOD::RODHeaderContainer_v1 > RODHeader_el1;
      std::vector< ElementLink< xAOD::RODHeaderContainer_v1 > > RODHeader_el2;
      // Element link vector(s):
      ElementLinkVector< xAOD::RODHeaderContainer_v1 > RODHeader_elv1;
      std::vector< ElementLinkVector< xAOD::RODHeaderContainer_v1 > > RODHeader_elv2;   
      
      // TriggerTower    
      // Container(s):
      xAOD::TriggerTowerContainer_v1 TriggerTower_c1;
      // Element link(s):
      ElementLink< xAOD::TriggerTowerContainer_v1 > TriggerTower_el1;
      std::vector< ElementLink< xAOD::TriggerTowerContainer_v1 > > TriggerTower_el2;
      // Element link vector(s):
      ElementLinkVector< xAOD::TriggerTowerContainer_v1 > TriggerTower_elv1;
      std::vector< ElementLinkVector< xAOD::TriggerTowerContainer_v1 > > TriggerTower_elv2;        
     
   };
}
#endif // __GCCXML__
#endif // XAODTRIGL1CALO_XAODTRIGL1CALOCALOEVENTDICT_H
