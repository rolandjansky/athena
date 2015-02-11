/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigT1CaloEvent/JEMHits.h"
#undef private
#undef protected


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/JEMHits_p1.h"
#include "TrigT1EventTPCnv/JEMHitsCnv_p1.h"

/*
JEMHitsCnv_p1::JEMHitsCnv_p1()
  : T_AthenaPoolTPCnvBase< JEMHits, JEMHits_p1 >() {

}
*/

void JEMHitsCnv_p1::persToTrans( const JEMHits_p1* persObj, JEMHits* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting JEMHits from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the JEMHits
  // 
  transObj->m_crate   = persObj->m_crate;
  transObj->m_module  = persObj->m_module;
  transObj->m_peak    = persObj->m_peak; 
  transObj->m_JetHits = persObj->m_JetHits;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted JEMHits from persistent state [OK]" << endreq;

  return;

}

void JEMHitsCnv_p1::transToPers( const JEMHits* transObj, JEMHits_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of JEMHits..." << endreq;

  persObj->m_crate    = transObj->m_crate;
  persObj->m_module   = transObj->m_module;
  persObj->m_peak     = transObj->m_peak;
  persObj->m_JetHits  = transObj->m_JetHits;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of JEMHits [OK]" << endreq;

  return;

}
