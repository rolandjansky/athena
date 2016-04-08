/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CMXCPTob.h"


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/CMXCPTob_p1.h"
#include "TrigT1EventTPCnv/CMXCPTobCnv_p1.h"

using namespace LVL1;

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
  *transObj = CMXCPTob (persObj->m_crate,
                        persObj->m_cmx,
                        persObj->m_cpm,
                        persObj->m_chip,
                        persObj->m_location,
                        persObj->m_energy,
                        persObj->m_isolation,
                        persObj->m_error,
                        persObj->m_presenceMap,
                        persObj->m_peak);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CMXCPTob from persistent state [OK]" << endreq;

  return;

}

void CMXCPTobCnv_p1::transToPers( const CMXCPTob* transObj, CMXCPTob_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CMXCPTob..." << endreq;

  persObj->m_crate       = transObj->crate();
  persObj->m_cmx         = transObj->cmx();
  persObj->m_cpm         = transObj->cpm();
  persObj->m_chip        = transObj->chip();
  persObj->m_location    = transObj->location();
  persObj->m_peak        = transObj->peak();
  persObj->m_energy      = transObj->energyVec();
  persObj->m_isolation   = transObj->isolationVec();
  persObj->m_error       = transObj->errorVec();
  persObj->m_presenceMap = transObj->presenceMapVec();

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CMXCPTob [OK]" << endreq;

  return;

}
