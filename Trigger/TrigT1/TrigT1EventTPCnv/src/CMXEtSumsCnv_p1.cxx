/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CMXEtSums.h"


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/CMXEtSums_p1.h"
#include "TrigT1EventTPCnv/CMXEtSumsCnv_p1.h"

using namespace LVL1;

/*
CMXEtSumsCnv_p1::CMXEtSumsCnv_p1()
  : T_AthenaPoolTPCnvBase< CMXEtSums, CMXEtSums_p1 >() {

}
*/

void CMXEtSumsCnv_p1::persToTrans( const CMXEtSums_p1* persObj, CMXEtSums* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting CMXEtSums from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the CMXEtSums
  // 
  *transObj = CMXEtSums (persObj->m_crate,
                         persObj->m_source,
                         persObj->m_Et,
                         persObj->m_Ex,
                         persObj->m_Ey,
                         persObj->m_EtError,
                         persObj->m_ExError,
                         persObj->m_EyError,
                         persObj->m_peak);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CMXEtSums from persistent state [OK]" << endreq;

  return;

}

void CMXEtSumsCnv_p1::transToPers( const CMXEtSums* transObj, CMXEtSums_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CMXEtSums..." << endreq;

  persObj->m_crate    = transObj->crate();
  persObj->m_source   = transObj->source();
  persObj->m_peak     = transObj->peak();
  persObj->m_Et       = transObj->EtVec();
  persObj->m_Ex       = transObj->ExVec();
  persObj->m_Ey       = transObj->EyVec();
  persObj->m_EtError  = transObj->EtErrorVec();
  persObj->m_ExError  = transObj->ExErrorVec();
  persObj->m_EyError  = transObj->EyErrorVec();

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CMXEtSums [OK]" << endreq;

  return;

}
