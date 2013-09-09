/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMonitoringEvent/TrigMonRoi.h"
#undef private
#undef protected

#include <iostream>
#include "TrigMonitoringEventTPCnv/TrigMonRoi_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonRoiCnv_p1.h"

void TrigMonRoiCnv_p1::persToTrans(const TrigMonRoi_p1* persObj, 
				   TrigMonRoi* transObj, 
				   MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonRoiCnv_p1::persToTrans called " << endreq;
  }

  transObj->m_word    = persObj->m_word;
  transObj->m_var_key = persObj->m_var_key;
  transObj->m_var_val = persObj->m_var_val;
}


void TrigMonRoiCnv_p1::transToPers(const TrigMonRoi* transObj, 
				   TrigMonRoi_p1* persObj, 
				   MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonRoiCnv_p1::transToPers called " << endreq;
  }

  persObj->m_word    = transObj->m_word;
  persObj->m_var_key = transObj->m_var_key;
  persObj->m_var_val = transObj->m_var_val;
}
