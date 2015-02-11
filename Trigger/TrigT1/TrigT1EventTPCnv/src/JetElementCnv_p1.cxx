/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigT1CaloEvent/JetElement.h"
#undef private
#undef protected


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigT1EventTPCnv/JetElementCnv_p1.h"

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
  transObj->m_eta        = persObj->m_eta;
  transObj->m_phi        = persObj->m_phi;
  transObj->m_key        = persObj->m_key;
  transObj->m_peak       = persObj->m_peak;
  transObj->m_emEnergy   = persObj->m_emEnergy;
  transObj->m_hadEnergy  = persObj->m_hadEnergy;
  transObj->m_em_error   = persObj->m_em_error;
  transObj->m_had_error  = persObj->m_had_error;
  transObj->m_link_error = persObj->m_link_error;
  
  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted JetElement from persistent state [OK]" << endreq;

  return;

}

void JetElementCnv_p1::transToPers( const JetElement* transObj, JetElement_p1* persObj, MsgStream &log ) {

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Creating persistent state of JetElement..." << endreq;

  persObj->m_eta        = transObj->m_eta;
  persObj->m_phi        = transObj->m_phi;
  persObj->m_key        = transObj->m_key;
  persObj->m_peak       = transObj->m_peak;
  persObj->m_emEnergy   = transObj->m_emEnergy;
  persObj->m_hadEnergy  = transObj->m_hadEnergy;
  persObj->m_em_error   = transObj->m_em_error;
  persObj->m_had_error  = transObj->m_had_error;
  persObj->m_link_error = transObj->m_link_error;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of JetElement [OK]" << endreq;

  return;

}
