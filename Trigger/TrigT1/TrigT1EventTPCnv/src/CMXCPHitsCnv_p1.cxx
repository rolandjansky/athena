/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CMXCPHits.h"


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/CMXCPHits_p1.h"
#include "TrigT1EventTPCnv/CMXCPHitsCnv_p1.h"

using namespace LVL1;

/*
CMXCPHitsCnv_p1::CMXCPHitsCnv_p1()
  : T_AthenaPoolTPCnvBase< CMXCPHits, CMXCPHits_p1 >() {

}
*/

void CMXCPHitsCnv_p1::persToTrans( const CMXCPHits_p1* persObj, CMXCPHits* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting CMXCPHits from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the CMXCPHits
  // 
  *transObj = CMXCPHits (persObj->m_crate,
                         persObj->m_cmx,
                         persObj->m_source,
                         persObj->m_hits0,
                         persObj->m_hits1,
                         persObj->m_error0,
                         persObj->m_error1,
                         persObj->m_peak);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CMXCPHits from persistent state [OK]" << endreq;

  return;

}

void CMXCPHitsCnv_p1::transToPers( const CMXCPHits* transObj, CMXCPHits_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CMXCPHits..." << endreq;

  persObj->m_crate    = transObj->crate();
  persObj->m_cmx      = transObj->cmx();
  persObj->m_source   = transObj->source();
  persObj->m_peak     = transObj->peak();
  persObj->m_hits0    = transObj->hitsVec0();
  persObj->m_hits1    = transObj->hitsVec1();
  persObj->m_error0   = transObj->errorVec0();
  persObj->m_error1   = transObj->errorVec1();

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CMXCPHits [OK]" << endreq;

  return;

}
