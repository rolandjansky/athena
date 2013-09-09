/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMonitoringEvent/TrigMonAlg.h"
#undef private
#undef protected

#include <iostream>
#include "TrigMonitoringEventTPCnv/TrigMonAlg_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonAlgCnv_p1.h"

void TrigMonAlgCnv_p1::persToTrans(const TrigMonAlg_p1* persObj, 
				   TrigMonAlg* transObj, 
				   MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonAlgCnv_p1::persToTrans called " << endreq;
  }

  transObj->m_byte = persObj->m_byte;
  transObj->m_word = persObj->m_word;
}


void TrigMonAlgCnv_p1::transToPers(const TrigMonAlg* transObj, 
				   TrigMonAlg_p1* persObj, 
				   MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonAlgCnv_p1::transToPers called " << endreq;
  }

  persObj->m_byte = transObj->m_byte;
  persObj->m_word = transObj->m_word;
}
