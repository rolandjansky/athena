/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CMXJetTob.h"


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/CMXJetTob_p1.h"
#include "TrigT1EventTPCnv/CMXJetTobCnv_p1.h"

using namespace LVL1;

/*
CMXJetTobCnv_p1::CMXJetTobCnv_p1()
  : T_AthenaPoolTPCnvBase< CMXJetTob, CMXJetTob_p1 >() {

}
*/

void CMXJetTobCnv_p1::persToTrans( const CMXJetTob_p1* persObj, CMXJetTob* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting CMXJetTob from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the CMXJetTob
  // 
  *transObj = CMXJetTob (persObj->m_crate,
                         persObj->m_jem,
                         persObj->m_frame,
                         persObj->m_location,
                         persObj->m_energyLarge,
                         persObj->m_energySmall,
                         persObj->m_error,
                         persObj->m_presenceMap,
                         persObj->m_peak);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CMXJetTob from persistent state [OK]" << endreq;

  return;

}

void CMXJetTobCnv_p1::transToPers( const CMXJetTob* transObj, CMXJetTob_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CMXJetTob..." << endreq;

  persObj->m_crate       = transObj->crate();
  persObj->m_jem         = transObj->jem();
  persObj->m_frame       = transObj->frame();
  persObj->m_location    = transObj->location();
  persObj->m_peak        = transObj->peak();
  persObj->m_energyLarge = transObj->energyLgVec();
  persObj->m_energySmall = transObj->energySmVec();
  persObj->m_error       = transObj->errorVec();
  persObj->m_presenceMap = transObj->presenceMapVec();

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CMXJetTob [OK]" << endreq;

  return;

}
