/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

//
// Includes for MuCTPI_RDO:
//
#include "TrigT1EventTPCnv/MuCTPI_RDO_p1.h"
#include "TrigT1EventTPCnv/MuCTPI_RDOCnv_p1.h"

//
// Includes for CTP_RDO:
//
#include "TrigT1EventTPCnv/CTP_RDO_p1.h"
#include "TrigT1EventTPCnv/CTP_RDOCnv_p1.h"
#include "TrigT1EventTPCnv/CTP_RDO_p2.h"
#include "TrigT1EventTPCnv/CTP_RDOCnv_p2.h"

//
// Includes for ROIB::RoIBResult:
//
#include "TrigT1EventTPCnv/RoIBResult_p1.h"
#include "TrigT1EventTPCnv/RoIBResultCnv_p1.h"

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

// Includes for LVL1::JEMRoI 
#include "TrigT1EventTPCnv/JEMRoICollection_tlp1.h"
#include "TrigT1EventTPCnv/JEMRoICollection_p1.h"
#include "TrigT1EventTPCnv/JEMRoI_p1.h"

// Includes for LVL1::RODHeader
#include "TrigT1EventTPCnv/RODHeaderCollection_tlp1.h"
#include "TrigT1EventTPCnv/RODHeaderCollection_p1.h"
#include "TrigT1EventTPCnv/RODHeader_p1.h"


DECLARE_TPCNV_FACTORY(CTP_RDOCnv_p1,
                      CTP_RDO,
                      CTP_RDO_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(CTP_RDOCnv_p2,
                      CTP_RDO,
                      CTP_RDO_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(MuCTPI_RDOCnv_p1,
                      MuCTPI_RDO,
                      MuCTPI_RDO_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(RoIBResultCnv_p1,
                      ROIB::RoIBResult,
                      RoIBResult_p1,
                      Athena::TPCnvVers::Current)
