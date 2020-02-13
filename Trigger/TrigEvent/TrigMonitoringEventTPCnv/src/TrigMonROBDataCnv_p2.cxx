/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitoringEvent/TrigMonROBData.h"

#include "TrigMonitoringEventTPCnv/TrigMonROBData_p2.h"
#include "TrigMonitoringEventTPCnv/TrigMonROBDataCnv_p2.h"

void TrigMonROBDataCnv_p2::persToTrans(const TrigMonROBData_p2* persObj, 
				       TrigMonROBData* transObj, 
				       MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonROBDataCnv_p2::persToTrans called " << endmsg;
  }

  *transObj = TrigMonROBData (persObj->m_rob_id,
                              persObj->m_word);
  transObj->m_word        = persObj->m_word;
}


void TrigMonROBDataCnv_p2::transToPers(const TrigMonROBData* transObj, 
				    TrigMonROBData_p2* persObj, 
				    MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonROBDataCnv_p2::transToPers called " << endmsg;
  }

  persObj->m_rob_id      = transObj->getROBId();
  persObj->m_word        = transObj->m_word;
}
