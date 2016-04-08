/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/JetElement.h"


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigT1EventTPCnv/JetElementCnv_p1.h"

using namespace LVL1;

/*
JetElementCnv_p1::JetElementCnv_p1()
  : T_AthenaPoolTPCnvBase< JetElement, JetElement_p1 >() {

}
*/

void JetElementCnv_p1::persToTrans( const JetElement_p1* persObj, JetElement* transObj, MsgStream &log ) {

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converting JetElement from persistent state..." << endreq;

  //
  // Translate the JetElement
  // 
  *transObj = JetElement (persObj->m_phi,
                          persObj->m_eta,
                          persObj->m_emEnergy,
                          persObj->m_hadEnergy,
                          persObj->m_key,
                          persObj->m_em_error,
                          persObj->m_had_error,
                          persObj->m_link_error,
                          persObj->m_peak);
  
  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted JetElement from persistent state [OK]" << endreq;

  return;

}

void JetElementCnv_p1::transToPers( const JetElement* transObj, JetElement_p1* persObj, MsgStream &log ) {

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Creating persistent state of JetElement..." << endreq;

  persObj->m_eta        = transObj->eta();
  persObj->m_phi        = transObj->phi();
  persObj->m_key        = transObj->key();
  persObj->m_peak       = transObj->peak();
  persObj->m_emEnergy   = transObj->emEnergyVec();
  persObj->m_hadEnergy  = transObj->hadEnergyVec();
  persObj->m_em_error   = transObj->emErrorVec();
  persObj->m_had_error  = transObj->hadErrorVec();
  persObj->m_link_error = transObj->linkErrorVec();

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of JetElement [OK]" << endreq;

  return;

}
