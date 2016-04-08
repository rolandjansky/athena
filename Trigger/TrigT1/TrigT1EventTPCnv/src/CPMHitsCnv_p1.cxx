/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CPMHits.h"


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigT1EventTPCnv/CPMHits_p1.h"
#include "TrigT1EventTPCnv/CPMHitsCnv_p1.h"

using namespace LVL1;

/*
CPMHitsCnv_p1::CPMHitsCnv_p1()
  : T_AthenaPoolTPCnvBase< CPMHits, CPMHits_p1 >() {

}
*/

void CPMHitsCnv_p1::persToTrans( const CPMHits_p1* persObj, CPMHits* transObj, MsgStream &log ) {

  //log << MSG::DEBUG << "Converting CPMHits from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the CPMHits
  // 
  *transObj = CPMHits (persObj->m_crate,
                       persObj->m_module,
                       persObj->m_Hits0,
                       persObj->m_Hits1,
                       persObj->m_peak);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CPMHits from persistent state [OK]" << endreq;

  return;

}

void CPMHitsCnv_p1::transToPers( const CPMHits* transObj, CPMHits_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CPMHits..." << endreq;

  persObj->m_crate    = transObj->crate();
  persObj->m_module   = transObj->module();
  persObj->m_peak     = transObj->peak();
  persObj->m_Hits0    = transObj->HitsVec0();
  persObj->m_Hits1    = transObj->HitsVec1();

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CPMHits [OK]" << endreq;

  return;

}
