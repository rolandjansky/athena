/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "AnalysisTriggerEventTPCnv/CTP_DecisionCnv_p2.h"

CTP_DecisionCnv_p2::CTP_DecisionCnv_p2()
  : T_AthenaPoolTPCnvBase< CTP_Decision, CTP_Decision_p2 >() {

}

void CTP_DecisionCnv_p2::persToTrans( const CTP_Decision_p2* persObj, CTP_Decision* transObj, MsgStream &log ) {

  log << MSG::DEBUG << "Converting CTP_Decision from persistent state..." << endreq;

  //
  // Simply copy the contents to the transient object:
  //
  transObj->setWords( persObj->m_CTPResultWord );
  transObj->setTriggerTypeWord( persObj->m_triggerTypeWord );
  transObj->setItems( persObj->m_items );

  log << MSG::DEBUG << "Converting CTP_Decision from persistent state [OK]" << endreq;

  return;

}

void CTP_DecisionCnv_p2::transToPers( const CTP_Decision* transObj, CTP_Decision_p2* persObj, MsgStream &log ) {

  log << MSG::DEBUG << "Creating persistent state of CTP_Decision..." << endreq;

  //
  // Simply copy the contents of the persistent object:
  //
  persObj->m_CTPResultWord = transObj->getWords();
  persObj->m_triggerTypeWord = transObj->getTriggerTypeWord();
  persObj->m_items = transObj->getItems();

  log << MSG::DEBUG << "Creating persistent state of CTP_Decision [OK]" << endreq;

  return;

}
