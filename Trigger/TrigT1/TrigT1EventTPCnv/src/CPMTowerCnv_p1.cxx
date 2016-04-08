/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CPMTower.h"


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigT1EventTPCnv/CPMTowerCnv_p1.h"

using namespace LVL1;

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
  *transObj = CPMTower (persObj->m_phi,
                        persObj->m_eta,
                        persObj->m_em_energy,
                        persObj->m_em_error,
                        persObj->m_had_energy,
                        persObj->m_had_error,
                        persObj->m_peak);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CPMTower from persistent state [OK]" << endreq;

  return;

}

void CPMTowerCnv_p1::transToPers( const CPMTower* transObj, CPMTower_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CPMTower..." << endreq;

  persObj->m_phi          = transObj->phi();
  persObj->m_eta          = transObj->eta();
  persObj->m_peak         = transObj->peak();
  
  persObj->m_em_energy    = transObj->emEnergyVec();
  persObj->m_had_energy   = transObj->hadEnergyVec();
  persObj->m_em_error     = transObj->emErrorVec();
  persObj->m_had_error    = transObj->hadErrorVec();

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CPMTower [OK]" << endreq;

  return;

}
