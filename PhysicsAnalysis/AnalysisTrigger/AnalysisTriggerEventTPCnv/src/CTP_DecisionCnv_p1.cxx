/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "AnalysisTriggerEventTPCnv/CTP_DecisionCnv_p1.h"

CTP_DecisionCnv_p1::CTP_DecisionCnv_p1()
  : T_AthenaPoolTPCnvBase< CTP_Decision, CTP_Decision_p1 >() {

}

void CTP_DecisionCnv_p1::persToTrans( const CTP_Decision_p1* persObj, CTP_Decision* transObj, MsgStream &log ) {

  log << MSG::DEBUG << "Converting CTP_Decision from persistent state..." << endreq;

  //
  // Simply copy the contents to the transient object:
  //
  transObj->setWord0( persObj->m_word0 );
  transObj->setWord1( persObj->m_word1 );
  transObj->setWord2( persObj->m_word2 );
  transObj->setTriggerTypeWord( persObj->m_triggerTypeWord );
  for( std::vector< std::string >::const_iterator it = persObj->m_items.begin(); it != persObj->m_items.end(); ++it ) {
    transObj->setItem( *it );
  }

  log << MSG::DEBUG << "Converting CTP_Decision from persistent state [OK]" << endreq;

  return;

}

void CTP_DecisionCnv_p1::transToPers( const CTP_Decision* transObj, CTP_Decision_p1* persObj, MsgStream &log ) {

  log << MSG::DEBUG << "Creating persistent state of CTP_Decision..." << endreq;

  //
  // Simply copy the contents of the persistent object:
  //
  persObj->m_word0 = transObj->getWord0();
  persObj->m_word1 = transObj->getWord1();
  persObj->m_word2 = transObj->getWord2();
  persObj->m_triggerTypeWord = transObj->getTriggerTypeWord();
  for( std::vector< std::string >::const_iterator it = transObj->getItems().begin(); it != transObj->getItems().end(); ++it ) {
    persObj->m_items.push_back( *it );
  }

  log << MSG::DEBUG << "Creating persistent state of CTP_Decision [OK]" << endreq;

  return;

}
