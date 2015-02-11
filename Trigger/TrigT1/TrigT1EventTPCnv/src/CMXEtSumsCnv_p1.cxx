/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigT1CaloEvent/CMXEtSums.h"
#undef private
#undef protected


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/CMXEtSums_p1.h"
#include "TrigT1EventTPCnv/CMXEtSumsCnv_p1.h"

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
  transObj->m_crate   = persObj->m_crate;
  transObj->m_source  = persObj->m_source;
  transObj->m_peak    = persObj->m_peak;
  transObj->m_Et      = persObj->m_Et;
  transObj->m_Ex      = persObj->m_Ex;
  transObj->m_Ey      = persObj->m_Ey;
  transObj->m_EtError = persObj->m_EtError;
  transObj->m_ExError = persObj->m_ExError;
  transObj->m_EyError = persObj->m_EyError;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CMXEtSums from persistent state [OK]" << endreq;

  return;

}

void CMXEtSumsCnv_p1::transToPers( const CMXEtSums* transObj, CMXEtSums_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CMXEtSums..." << endreq;

  persObj->m_crate    = transObj->m_crate;
  persObj->m_source   = transObj->m_source;
  persObj->m_peak     = transObj->m_peak;
  persObj->m_Et       = transObj->m_Et;
  persObj->m_Ex       = transObj->m_Ex;
  persObj->m_Ey       = transObj->m_Ey;
  persObj->m_EtError  = transObj->m_EtError;
  persObj->m_ExError  = transObj->m_ExError;
  persObj->m_EyError  = transObj->m_EyError;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CMXEtSums [OK]" << endreq;

  return;

}
