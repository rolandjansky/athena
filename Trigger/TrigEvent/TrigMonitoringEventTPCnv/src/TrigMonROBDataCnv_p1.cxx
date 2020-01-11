/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitoringEvent/TrigMonROBData.h"

#include "TrigMonitoringEventTPCnv/TrigMonROBData_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonROBDataCnv_p1.h"

void TrigMonROBDataCnv_p1::persToTrans(const TrigMonROBData_p1* persObj, 
				       TrigMonROBData* transObj, 
				       MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonROBDataCnv_p1::persToTrans called " << endmsg;
  }

  *transObj = TrigMonROBData (persObj->m_rob_id,
                              persObj->m_rob_size);

  transObj->m_word        = persObj->m_rob_size;
}


void TrigMonROBDataCnv_p1::transToPers(const TrigMonROBData* transObj, 
				    TrigMonROBData_p1* persObj, 
				    MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonROBDataCnv_p1::transToPers called " << endmsg;
  }

  persObj->m_rob_id      = transObj->getROBId();
  persObj->m_rob_size    = transObj->m_word;
  persObj->m_word        = 0;
}
