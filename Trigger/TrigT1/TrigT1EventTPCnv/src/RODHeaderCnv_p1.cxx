/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/RODHeader.h"


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigT1EventTPCnv/RODHeaderCnv_p1.h"

using namespace LVL1;

/*
RODHeaderCnv_p1::RODHeaderCnv_p1()
  : T_AthenaPoolTPCnvBase< RODHeader, RODHeader_p1 >() {

}
*/

void RODHeaderCnv_p1::persToTrans( const RODHeader_p1* persObj, RODHeader* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting RODHeader from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the RODHeader
  // 
  *transObj = RODHeader (persObj->m_version,
                         persObj->m_sourceId,
                         persObj->m_run,
                         persObj->m_lvl1Id,
                         persObj->m_bcId,
                         persObj->m_trigType,
                         persObj->m_detType,
                         persObj->m_statusWords,
                         persObj->m_payloadSize);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted RODHeader from persistent state [OK]" << endreq;

  return;

}

void RODHeaderCnv_p1::transToPers( const RODHeader* transObj, RODHeader_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of RODHeader..." << endreq;

  persObj->m_version     = transObj->version();
  persObj->m_sourceId    = transObj->sourceID();
  persObj->m_run         = transObj->run();
  persObj->m_lvl1Id      = transObj->extendedL1ID();
  persObj->m_bcId        = transObj->bunchCrossing();
  persObj->m_trigType    = transObj->l1TriggerType();
  persObj->m_detType     = transObj->detEventType();
  persObj->m_statusWords = transObj->statusWords();
  persObj->m_payloadSize = transObj->payloadSize();

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of RODHeader [OK]" << endreq;

  return;

}
