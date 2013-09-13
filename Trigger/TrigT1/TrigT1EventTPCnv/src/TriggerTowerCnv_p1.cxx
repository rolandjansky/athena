/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigT1CaloEvent/TriggerTower.h"
#undef private
#undef protected


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigT1EventTPCnv/TriggerTowerCnv_p1.h"

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
  transObj->m_eta = persObj->m_eta;
  transObj->m_phi = persObj->m_phi;
  transObj->m_key = persObj->m_key;
  
  transObj->m_em_adc     = persObj->m_em_adc;
  transObj->m_em_energy  = persObj->m_em_energy;
  transObj->m_em_extBCID = persObj->m_em_extBCID;
  transObj->m_em_BCID    = persObj->m_em_BCID;
  transObj->m_em_error   = persObj->m_em_error;
  transObj->m_em_peak    = persObj->m_em_peak;
  transObj->m_em_adc_peak = persObj->m_em_adc_peak;
  
  transObj->m_had_adc     = persObj->m_had_adc;
  transObj->m_had_energy  = persObj->m_had_energy;
  transObj->m_had_extBCID = persObj->m_had_extBCID;
  transObj->m_had_BCID    = persObj->m_had_BCID;
  transObj->m_had_error   = persObj->m_had_error;
  transObj->m_had_peak    = persObj->m_had_peak;
  transObj->m_had_adc_peak = persObj->m_had_adc_peak;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted TriggerTower from persistent state [OK]" << endreq;

  return;

}

void TriggerTowerCnv_p1::transToPers( const TriggerTower* transObj, TriggerTower_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of TriggerTower..." << endreq;

  persObj->m_em_energy    = transObj->m_em_energy;
  persObj->m_had_energy   = transObj->m_had_energy;
  persObj->m_em_adc       = transObj->m_em_adc;
  persObj->m_had_adc      = transObj->m_had_adc;
  persObj->m_em_BCID      = transObj->m_em_BCID;
  persObj->m_had_BCID     = transObj->m_had_BCID;
  persObj->m_em_extBCID   = transObj->m_em_extBCID;
  persObj->m_had_extBCID  = transObj->m_had_extBCID;
  persObj->m_phi          = transObj->m_phi;
  persObj->m_eta          = transObj->m_eta;
  persObj->m_key          = transObj->m_key;
  persObj->m_em_error     = transObj->m_em_error;
  persObj->m_had_error    = transObj->m_had_error;
  persObj->m_em_peak      = transObj->m_em_peak;
  persObj->m_em_adc_peak  = transObj->m_em_adc_peak;
  persObj->m_had_peak     = transObj->m_had_peak;
  persObj->m_had_adc_peak = transObj->m_had_adc_peak;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of TriggerTower [OK]" << endreq;

  return;

}
