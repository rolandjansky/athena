/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CMMEtSums.h"


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/CMMEtSums_p1.h"
#include "TrigT1EventTPCnv/CMMEtSumsCnv_p1.h"

using namespace LVL1;

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
  *transObj = CMMEtSums (persObj->m_crate,
                         persObj->m_dataID,
                         persObj->m_Et,
                         persObj->m_Ex,
                         persObj->m_Ey,
                         persObj->m_EtError,
                         persObj->m_ExError,
                         persObj->m_EyError,
                         persObj->m_peak);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CMMEtSums from persistent state [OK]" << endreq;

  return;

}

void CMMEtSumsCnv_p1::transToPers( const CMMEtSums* transObj, CMMEtSums_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CMMEtSums..." << endreq;

  persObj->m_crate    = transObj->crate();
  persObj->m_dataID   = transObj->dataID();
  persObj->m_Et       = transObj->EtVec();
  persObj->m_Ex       = transObj->ExVec();
  persObj->m_Ey       = transObj->EyVec();
  persObj->m_EtError  = transObj->EtErrorVec();
  persObj->m_ExError  = transObj->ExErrorVec();
  persObj->m_EyError  = transObj->EyErrorVec();
  persObj->m_peak     = transObj->peak();

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CMMEtSums [OK]" << endreq;

  return;

}
