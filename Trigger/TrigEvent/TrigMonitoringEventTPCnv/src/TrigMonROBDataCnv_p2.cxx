/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMonitoringEvent/TrigMonROBData.h"
#undef private
#undef protected

#include "TrigMonitoringEventTPCnv/TrigMonROBData_p2.h"
#include "TrigMonitoringEventTPCnv/TrigMonROBDataCnv_p2.h"

void TrigMonROBDataCnv_p2::persToTrans(const TrigMonROBData_p2* persObj, 
				       TrigMonROBData* transObj, 
				       MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonROBDataCnv_p2::persToTrans called " << endreq;
  }

  transObj->m_rob_id      = persObj->m_rob_id;
  transObj->m_word        = persObj->m_word;
}


void TrigMonROBDataCnv_p2::transToPers(const TrigMonROBData* transObj, 
				    TrigMonROBData_p2* persObj, 
				    MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonROBDataCnv_p2::transToPers called " << endreq;
  }

  persObj->m_rob_id      = transObj->m_rob_id;
  persObj->m_word        = transObj->m_word;
}
