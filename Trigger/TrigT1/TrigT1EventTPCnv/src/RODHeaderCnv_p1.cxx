/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigT1CaloEvent/RODHeader.h"
#undef private
#undef protected


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigT1EventTPCnv/RODHeaderCnv_p1.h"

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
  transObj->m_version     = persObj->m_version;
  transObj->m_sourceId    = persObj->m_sourceId;
  transObj->m_run         = persObj->m_run;
  transObj->m_lvl1Id      = persObj->m_lvl1Id;
  transObj->m_bcId        = persObj->m_bcId;
  transObj->m_trigType    = persObj->m_trigType;
  transObj->m_detType     = persObj->m_detType;
  transObj->m_statusWords = persObj->m_statusWords;
  transObj->m_payloadSize = persObj->m_payloadSize;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted RODHeader from persistent state [OK]" << endreq;

  return;

}

void RODHeaderCnv_p1::transToPers( const RODHeader* transObj, RODHeader_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of RODHeader..." << endreq;

  persObj->m_version     = transObj->m_version;
  persObj->m_sourceId    = transObj->m_sourceId;
  persObj->m_run         = transObj->m_run;
  persObj->m_lvl1Id      = transObj->m_lvl1Id;
  persObj->m_bcId        = transObj->m_bcId;
  persObj->m_trigType    = transObj->m_trigType;
  persObj->m_detType     = transObj->m_detType;
  persObj->m_statusWords = transObj->m_statusWords;
  persObj->m_payloadSize = transObj->m_payloadSize;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of RODHeader [OK]" << endreq;

  return;

}
