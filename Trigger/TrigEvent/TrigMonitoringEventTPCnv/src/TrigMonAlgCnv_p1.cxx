/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitoringEvent/TrigMonAlg.h"

#include <iostream>
#include "TrigMonitoringEventTPCnv/TrigMonAlg_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonAlgCnv_p1.h"

void TrigMonAlgCnv_p1::persToTrans(const TrigMonAlg_p1* persObj, 
				   TrigMonAlg* transObj, 
				   MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonAlgCnv_p1::persToTrans called " << endmsg;
  }

  transObj->m_byte = persObj->m_byte;
  transObj->m_word = persObj->m_word;
}


void TrigMonAlgCnv_p1::transToPers(const TrigMonAlg* transObj, 
				   TrigMonAlg_p1* persObj, 
				   MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonAlgCnv_p1::transToPers called " << endmsg;
  }

  persObj->m_byte = transObj->getByte();
  persObj->m_word = transObj->getWord();
}
