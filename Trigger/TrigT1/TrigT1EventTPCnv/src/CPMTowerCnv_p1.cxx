/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigT1CaloEvent/CPMTower.h"
#undef private
#undef protected


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigT1EventTPCnv/CPMTowerCnv_p1.h"

/*
CPMTowerCnv_p1::CPMTowerCnv_p1()
  : T_AthenaPoolTPCnvBase< CPMTower, CPMTower_p1 >() {

}
*/

void CPMTowerCnv_p1::persToTrans( const CPMTower_p1* persObj, CPMTower* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting CPMTower from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the CPMTower
  // 
  transObj->m_eta = persObj->m_eta;
  transObj->m_phi = persObj->m_phi;
  transObj->m_peak = persObj->m_peak;
  
  transObj->m_em_energy  = persObj->m_em_energy;
  transObj->m_em_error   = persObj->m_em_error;
  transObj->m_had_energy  = persObj->m_had_energy;
  transObj->m_had_error   = persObj->m_had_error;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CPMTower from persistent state [OK]" << endreq;

  return;

}

void CPMTowerCnv_p1::transToPers( const CPMTower* transObj, CPMTower_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CPMTower..." << endreq;

  persObj->m_phi          = transObj->m_phi;
  persObj->m_eta          = transObj->m_eta;
  persObj->m_peak         = transObj->m_peak;
  
  persObj->m_em_energy    = transObj->m_em_energy;
  persObj->m_had_energy   = transObj->m_had_energy;
  persObj->m_em_error     = transObj->m_em_error;
  persObj->m_had_error    = transObj->m_had_error;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CPMTower [OK]" << endreq;

  return;

}
