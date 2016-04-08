/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CMMJetHits.h"

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/CMMJetHits_p1.h"
#include "TrigT1EventTPCnv/CMMJetHitsCnv_p1.h"

using namespace LVL1;

/*
CMMJetHitsCnv_p1::CMMJetHitsCnv_p1()
  : T_AthenaPoolTPCnvBase< CMMJetHits, CMMJetHits_p1 >() {

}
*/

void CMMJetHitsCnv_p1::persToTrans( const CMMJetHits_p1* persObj, CMMJetHits* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting CMMJetHits from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the CMMJetHits
  // 
  *transObj = CMMJetHits (persObj->m_crate,
                          persObj->m_dataID,
                          persObj->m_hits,
                          persObj->m_errors,
                          persObj->m_peak);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CMMJetHits from persistent state [OK]" << endreq;

  return;

}

void CMMJetHitsCnv_p1::transToPers( const CMMJetHits* transObj, CMMJetHits_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CMMJetHits..." << endreq;

  persObj->m_crate    = transObj->crate();
  persObj->m_dataID   = transObj->dataID();
  persObj->m_peak     = transObj->peak();
  persObj->m_hits     = transObj->HitsVec();
  persObj->m_errors   = transObj->ErrorVec();

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CMMJetHits [OK]" << endreq;

  return;

}
