/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigT1CaloEvent/JEMEtSums.h"
#undef private
#undef protected


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/JEMEtSum_p1.h"
#include "TrigT1EventTPCnv/JEMEtSumsCnv_p1.h"

/*
JEMEtSumCnv_p1::JEMEtSumCnv_p1()
  : T_AthenaPoolTPCnvBase< JEMEtSum, JEMEtSum_p1 >() {

}
*/

void JEMEtSumsCnv_p1::persToTrans( const JEMEtSums_p1* persObj, JEMEtSums* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting JEMEtSum from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the JEMEtSum
  // 
  transObj->m_crate  = persObj->m_crate;
  transObj->m_module = persObj->m_module;
  transObj->m_peak   = persObj->m_peak; 
  transObj->m_Et     = persObj->m_Et;
  transObj->m_Ex     = persObj->m_Ex;
  transObj->m_Ey     = persObj->m_Ey;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted JEMEtSum from persistent state [OK]" << endreq;

  return;

}

void JEMEtSumsCnv_p1::transToPers( const JEMEtSums* transObj, JEMEtSums_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of JEMEtSum..." << endreq;

  persObj->m_crate    = transObj->m_crate;
  persObj->m_module   = transObj->m_module;
  persObj->m_peak     = transObj->m_peak;
  persObj->m_Et       = transObj->m_Et;
  persObj->m_Ex       = transObj->m_Ex;
  persObj->m_Ey       = transObj->m_Ey;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of JEMEtSum [OK]" << endreq;

  return;

}
