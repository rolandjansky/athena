/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigT1CaloEvent/CMMEtSums.h"
#undef private
#undef protected


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/CMMEtSums_p1.h"
#include "TrigT1EventTPCnv/CMMEtSumsCnv_p1.h"

/*
CMMEtSumsCnv_p1::CMMEtSumsCnv_p1()
  : T_AthenaPoolTPCnvBase< CMMEtSums, CMMEtSums_p1 >() {

}
*/

void CMMEtSumsCnv_p1::persToTrans( const CMMEtSums_p1* persObj, CMMEtSums* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting CMMEtSums from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the CMMEtSums
  // 
  transObj->m_crate   = persObj->m_crate;
  transObj->m_dataID  = persObj->m_dataID;
  transObj->m_peak    = persObj->m_peak;
  transObj->m_Et      = persObj->m_Et;
  transObj->m_Ex      = persObj->m_Ex;
  transObj->m_Ey      = persObj->m_Ey;
  transObj->m_EtError = persObj->m_EtError;
  transObj->m_ExError = persObj->m_ExError;
  transObj->m_EyError = persObj->m_EyError;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CMMEtSums from persistent state [OK]" << endreq;

  return;

}

void CMMEtSumsCnv_p1::transToPers( const CMMEtSums* transObj, CMMEtSums_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CMMEtSums..." << endreq;

  persObj->m_crate    = transObj->m_crate;
  persObj->m_dataID   = transObj->m_dataID;
  persObj->m_peak     = transObj->m_peak;
  persObj->m_Et       = transObj->m_Et;
  persObj->m_Ex       = transObj->m_Ex;
  persObj->m_Ey       = transObj->m_Ey;
  persObj->m_EtError  = transObj->m_EtError;
  persObj->m_ExError  = transObj->m_ExError;
  persObj->m_EyError  = transObj->m_EyError;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CMMEtSums [OK]" << endreq;

  return;

}
