/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMonitoringEvent/TrigMonROBData.h"
#undef private
#undef protected

#include "TrigMonitoringEventTPCnv/TrigMonROBData_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonROBDataCnv_p1.h"

void TrigMonROBDataCnv_p1::persToTrans(const TrigMonROBData_p1* persObj, 
				       TrigMonROBData* transObj, 
				       MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonROBDataCnv_p1::persToTrans called " << endreq;
  }
  
  transObj->m_rob_id      = persObj->m_rob_id;
  transObj->m_word        = persObj->m_rob_size;
}


void TrigMonROBDataCnv_p1::transToPers(const TrigMonROBData* transObj, 
				    TrigMonROBData_p1* persObj, 
				    MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonROBDataCnv_p1::transToPers called " << endreq;
  }

  persObj->m_rob_id      = transObj->m_rob_id;
  persObj->m_rob_size    = transObj->m_word;
  persObj->m_word        = 0;
}
