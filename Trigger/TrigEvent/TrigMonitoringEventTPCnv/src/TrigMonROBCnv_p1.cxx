/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMonitoringEvent/TrigMonROB.h"
#undef private
#undef protected

// This class data and converter
#include "TrigMonitoringEventTPCnv/TrigMonROB_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonROBCnv_p1.h"

void TrigMonROBCnv_p1::persToTrans(const TrigMonROB_p1* persObj, 
				   TrigMonROB* transObj, 
				   MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonROBCnv_p1::persToTrans called " << endreq;
  }

  transObj->m_word = persObj->m_word;
  m_dataCnv.persToTrans(&(persObj->m_data), &(transObj->m_data), log);
}


void TrigMonROBCnv_p1::transToPers(const TrigMonROB* transObj, 
				   TrigMonROB_p1* persObj, 
				   MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonROBCnv_p1::transToPers called " << endreq;
  }

  persObj->m_word = transObj->m_word;
  m_dataCnv.transToPers(&(transObj->m_data), &(persObj->m_data), log);
}
