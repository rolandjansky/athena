// Dear emacs, this is -*- c++ -*-
#ifndef TRIGT1EVENTTPCNV_TRIGT1EVENTTPCNVDICT_H
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#define TRIGT1EVENTTPCNV_TRIGT1EVENTTPCNVDICT_H

//
// Includes for MuCTPI_RDO:
//
#include "TrigT1EventTPCnv/MuCTPI_RDO_p1.h"

//
// Includes for CTP_RDO:
//
#include "TrigT1EventTPCnv/CTP_RDO_p1.h"
#include "TrigT1EventTPCnv/CTP_RDO_p2.h"

//
// Includes for ROIB::RoIBResult:
//
#include "TrigT1EventTPCnv/RoIBResult_p1.h"

//
// Includes for LVL1::TriggerTower
//
#include "TrigT1EventTPCnv/TriggerTowerCollection_tlp1.h"
#include "TrigT1EventTPCnv/TriggerTowerCollection_p1.h"
#include "TrigT1EventTPCnv/TriggerTower_p1.h"

//
// Includes for LVL1::JetElement
//
#include "TrigT1EventTPCnv/JetElementCollection_tlp1.h"
#include "TrigT1EventTPCnv/JetElementCollection_p1.h"
#include "TrigT1EventTPCnv/JetElement_p1.h"

// Includes for LVL1::CPMTower
#include "TrigT1EventTPCnv/CPMTowerCollection_tlp1.h"
#include "TrigT1EventTPCnv/CPMTowerCollection_p1.h"
#include "TrigT1EventTPCnv/CPMTower_p1.h"

// Includes for LVL1::CPMHits 
#include "TrigT1EventTPCnv/CPMHitsCollection_tlp1.h"
#include "TrigT1EventTPCnv/CPMHitsCollection_p1.h"
#include "TrigT1EventTPCnv/CPMHits_p1.h"

// Includes for LVL1::CPMRoI
#include "TrigT1EventTPCnv/CPMRoICollection_tlp1.h"
#include "TrigT1EventTPCnv/CPMRoICollection_p1.h"
#include "TrigT1EventTPCnv/CPMRoI_p1.h"

// Includes for LVL1::CPMTobRoI
#include "TrigT1EventTPCnv/CPMTobRoICollection_tlp1.h"
#include "TrigT1EventTPCnv/CPMTobRoICollection_p1.h"
#include "TrigT1EventTPCnv/CPMTobRoI_p1.h"

// Includes for LVL1::CMXCPTob
#include "TrigT1EventTPCnv/CMXCPTobCollection_tlp1.h"
#include "TrigT1EventTPCnv/CMXCPTobCollection_p1.h"
#include "TrigT1EventTPCnv/CMXCPTob_p1.h"

// Includes for LVL1::CMXCPHits
#include "TrigT1EventTPCnv/CMXCPHitsCollection_tlp1.h"
#include "TrigT1EventTPCnv/CMXCPHitsCollection_p1.h"
#include "TrigT1EventTPCnv/CMXCPHits_p1.h"

// Includes for LVL1::CMXJetTob
#include "TrigT1EventTPCnv/CMXJetTobCollection_tlp1.h"
#include "TrigT1EventTPCnv/CMXJetTobCollection_p1.h"
#include "TrigT1EventTPCnv/CMXJetTob_p1.h"

// Includes for LVL1::CMXJetHits
#include "TrigT1EventTPCnv/CMXJetHitsCollection_tlp1.h"
#include "TrigT1EventTPCnv/CMXJetHitsCollection_p1.h"
#include "TrigT1EventTPCnv/CMXJetHits_p1.h"

// Includes for LVL1::CMXRoI
#include "TrigT1EventTPCnv/CMXRoI_p1.h"

// Includes for LVL1::CMXEtSums 
#include "TrigT1EventTPCnv/CMXEtSumsCollection_tlp1.h"
#include "TrigT1EventTPCnv/CMXEtSumsCollection_p1.h"
#include "TrigT1EventTPCnv/CMXEtSums_p1.h"

// Includes for LVL1::CMMCPHits
#include "TrigT1EventTPCnv/CMMCPHitsCollection_tlp1.h"
#include "TrigT1EventTPCnv/CMMCPHitsCollection_p1.h"
#include "TrigT1EventTPCnv/CMMCPHits_p1.h"

// Includes for LVL1::CMMJetHits
#include "TrigT1EventTPCnv/CMMJetHitsCollection_tlp1.h"
#include "TrigT1EventTPCnv/CMMJetHitsCollection_p1.h"
#include "TrigT1EventTPCnv/CMMJetHits_p1.h"

// Includes for LVL1::CMMRoI
#include "TrigT1EventTPCnv/CMMRoI_p1.h"

// Includes for LVL1::CMMEtSums 
#include "TrigT1EventTPCnv/CMMEtSumsCollection_tlp1.h"
#include "TrigT1EventTPCnv/CMMEtSumsCollection_p1.h"
#include "TrigT1EventTPCnv/CMMEtSums_p1.h"

// Includes for LVL1::JEMHits
#include "TrigT1EventTPCnv/JEMHitsCollection_tlp1.h"
#include "TrigT1EventTPCnv/JEMHitsCollection_p1.h"
#include "TrigT1EventTPCnv/JEMHits_p1.h"

// Includes for LVL1::JEMEtSums 
#include "TrigT1EventTPCnv/JEMEtSumsCollection_tlp1.h"
#include "TrigT1EventTPCnv/JEMEtSumsCollection_p1.h"
#include "TrigT1EventTPCnv/JEMEtSums_p1.h"

// Includes for LVL1::JEMTobRoI 
#include "TrigT1EventTPCnv/JEMTobRoICollection_tlp1.h"
#include "TrigT1EventTPCnv/JEMTobRoICollection_p1.h"
#include "TrigT1EventTPCnv/JEMTobRoI_p1.h"

// Includes for LVL1::JEMRoI 
#include "TrigT1EventTPCnv/JEMRoICollection_tlp1.h"
#include "TrigT1EventTPCnv/JEMRoICollection_p1.h"
#include "TrigT1EventTPCnv/JEMRoI_p1.h"

// Includes for LVL1::RODHeader
#include "TrigT1EventTPCnv/RODHeaderCollection_tlp1.h"
#include "TrigT1EventTPCnv/RODHeaderCollection_p1.h"
#include "TrigT1EventTPCnv/RODHeader_p1.h"

struct GCCXML_DUMMY_INSTANTIATION_TRIGT1EVENTTPCNV {
   std::vector<TriggerTower_p1> m_dummyTriggerTower_p1;
   std::vector<JetElement_p1>   m_dummyJetElement_p1;
   std::vector<CPMTower_p1>     m_dummyCPMTower_p1;
   std::vector<CPMHits_p1>      m_dummyCPMHits_p1;
   std::vector<CPMRoI_p1>       m_dummyCPMRoI_p1;
   std::vector<CPMTobRoI_p1>    m_dummyCPMTobRoI_p1;
   std::vector<CMXCPTob_p1>     m_dummyCMXCPTob_p1;
   std::vector<CMXCPHits_p1>    m_dummyCMXCPHits_p1;
   std::vector<CMXJetTob_p1>    m_dummyCMXJetTob_p1;
   std::vector<CMXJetHits_p1>   m_dummyCMXJetHits_p1;
   std::vector<CMXRoI_p1>       m_dummyCMXRoI_p1;
   std::vector<CMXEtSums_p1>    m_dummyCMXEtSums_p1;
   std::vector<CMMCPHits_p1>    m_dummyCMMCPHits_p1;
   std::vector<CMMJetHits_p1>   m_dummyCMMJetHits_p1;
   std::vector<CMMRoI_p1>       m_dummyCMMRoI_p1;
   std::vector<CMMEtSums_p1>    m_dummyCMMEtSums_p1;
   std::vector<JEMHits_p1>      m_dummyJEMHits_p1;
   std::vector<JEMEtSums_p1>    m_dummyJEMEtSums_p1;
   std::vector<JEMTobRoI_p1>    m_dummyJEMTobRoI_p1;
   std::vector<JEMRoI_p1>       m_dummyJEMRoI_p1;
   std::vector<RODHeader_p1>    m_dummyRODHeader_p1;
};

#endif // TRIGT1EVENTTPCNV_TRIGT1EVENTTPCNVDICT_H
