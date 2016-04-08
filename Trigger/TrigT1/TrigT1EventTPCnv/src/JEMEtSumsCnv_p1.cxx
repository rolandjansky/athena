/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/JEMEtSums.h"


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/JEMEtSum_p1.h"
#include "TrigT1EventTPCnv/JEMEtSumsCnv_p1.h"

using namespace LVL1;

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
  *transObj = JEMEtSums (persObj->m_crate,
                         persObj->m_module,
                         persObj->m_Et,
                         persObj->m_Ex,
                         persObj->m_Ey,
                         persObj->m_peak);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted JEMEtSum from persistent state [OK]" << endreq;

  return;

}

void JEMEtSumsCnv_p1::transToPers( const JEMEtSums* transObj, JEMEtSums_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of JEMEtSum..." << endreq;

  persObj->m_crate    = transObj->crate();
  persObj->m_module   = transObj->module();
  persObj->m_peak     = transObj->peak();
  persObj->m_Et       = transObj->EtVec();
  persObj->m_Ex       = transObj->ExVec();
  persObj->m_Ey       = transObj->EyVec();

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of JEMEtSum [OK]" << endreq;

  return;

}
