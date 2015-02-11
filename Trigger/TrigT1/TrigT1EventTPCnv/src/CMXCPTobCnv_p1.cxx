/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigT1CaloEvent/CMXCPTob.h"
#undef private
#undef protected


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/CMXCPTob_p1.h"
#include "TrigT1EventTPCnv/CMXCPTobCnv_p1.h"

/*
CMXCPTobCnv_p1::CMXCPTobCnv_p1()
  : T_AthenaPoolTPCnvBase< CMXCPTob, CMXCPTob_p1 >() {

}
*/

void CMXCPTobCnv_p1::persToTrans( const CMXCPTob_p1* persObj, CMXCPTob* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting CMXCPTob from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the CMXCPTob
  // 
  transObj->m_crate       = persObj->m_crate;
  transObj->m_cmx         = persObj->m_cmx;
  transObj->m_cpm         = persObj->m_cpm;
  transObj->m_chip        = persObj->m_chip;
  transObj->m_location    = persObj->m_location;
  transObj->m_peak        = persObj->m_peak; 
  transObj->m_energy      = persObj->m_energy;
  transObj->m_isolation   = persObj->m_isolation;
  transObj->m_error       = persObj->m_error;
  transObj->m_presenceMap = persObj->m_presenceMap;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CMXCPTob from persistent state [OK]" << endreq;

  return;

}

void CMXCPTobCnv_p1::transToPers( const CMXCPTob* transObj, CMXCPTob_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CMXCPTob..." << endreq;

  persObj->m_crate       = transObj->m_crate;
  persObj->m_cmx         = transObj->m_cmx;
  persObj->m_cpm         = transObj->m_cpm;
  persObj->m_chip        = transObj->m_chip;
  persObj->m_location    = transObj->m_location;
  persObj->m_peak        = transObj->m_peak;
  persObj->m_energy      = transObj->m_energy;
  persObj->m_isolation   = transObj->m_isolation;
  persObj->m_error       = transObj->m_error;
  persObj->m_presenceMap = transObj->m_presenceMap;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CMXCPTob [OK]" << endreq;

  return;

}
