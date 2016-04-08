/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/TriggerTower.h"


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigT1EventTPCnv/TriggerTowerCnv_p1.h"

using namespace LVL1;

/*
TriggerTowerCnv_p1::TriggerTowerCnv_p1()
  : T_AthenaPoolTPCnvBase< TriggerTower, TriggerTower_p1 >() {

}
*/

void TriggerTowerCnv_p1::persToTrans( const TriggerTower_p1* persObj, TriggerTower* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting TriggerTower from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the TriggerTower
  // 
  *transObj = TriggerTower (persObj->m_phi,
                            persObj->m_eta,
                            persObj->m_key,
                            persObj->m_em_adc,
                            persObj->m_em_energy,
                            persObj->m_em_extBCID,
                            persObj->m_em_BCID,
                            persObj->m_em_error,
                            persObj->m_em_peak,
                            persObj->m_em_adc_peak,
                            persObj->m_had_adc,
                            persObj->m_had_energy,
                            persObj->m_had_extBCID,
                            persObj->m_had_BCID,
                            persObj->m_had_error,
                            persObj->m_had_peak,
                            persObj->m_had_adc_peak);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted TriggerTower from persistent state [OK]" << endreq;

  return;

}

void TriggerTowerCnv_p1::transToPers( const TriggerTower* transObj, TriggerTower_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of TriggerTower..." << endreq;

  persObj->m_em_energy    = transObj->emLUT();
  persObj->m_had_energy   = transObj->hadLUT();
  persObj->m_em_adc       = transObj->emADC();
  persObj->m_had_adc      = transObj->hadADC();
  persObj->m_em_BCID      = transObj->emBCIDvec();
  persObj->m_had_BCID     = transObj->hadBCIDvec();
  persObj->m_em_extBCID   = transObj->emBCIDext();
  persObj->m_had_extBCID  = transObj->hadBCIDext();
  persObj->m_phi          = transObj->phi();
  persObj->m_eta          = transObj->eta();
  persObj->m_key          = transObj->key();
  persObj->m_em_error     = transObj->emError();
  persObj->m_had_error    = transObj->hadError();
  persObj->m_em_peak      = transObj->emPeak();
  persObj->m_em_adc_peak  = transObj->emADCPeak();
  persObj->m_had_peak     = transObj->hadPeak();
  persObj->m_had_adc_peak = transObj->hadADCPeak();

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of TriggerTower [OK]" << endreq;

  return;

}
