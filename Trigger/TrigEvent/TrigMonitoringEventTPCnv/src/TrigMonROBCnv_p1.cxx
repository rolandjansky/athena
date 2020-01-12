/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitoringEvent/TrigMonROB.h"

// This class data and converter
#include "TrigMonitoringEventTPCnv/TrigMonROB_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonROBCnv_p1.h"

void TrigMonROBCnv_p1::persToTrans(const TrigMonROB_p1* persObj, 
				   TrigMonROB* transObj, 
				   MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonROBCnv_p1::persToTrans called " << endmsg;
  }

  transObj->getWord() = persObj->m_word;
  m_dataCnv.persToTrans(&(persObj->m_data), &(transObj->getData()), log);
}


void TrigMonROBCnv_p1::transToPers(const TrigMonROB* transObj, 
				   TrigMonROB_p1* persObj, 
				   MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonROBCnv_p1::transToPers called " << endmsg;
  }

  persObj->m_word = transObj->getWord();
  m_dataCnv.transToPers(&(transObj->getData()), &(persObj->m_data), log);
}
