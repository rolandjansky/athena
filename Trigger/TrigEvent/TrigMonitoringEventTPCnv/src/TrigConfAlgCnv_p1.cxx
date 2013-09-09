/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMonitoringEvent/TrigConfAlg.h"
#undef private
#undef protected

#include "TrigMonitoringEventTPCnv/TrigConfAlg_p1.h"
#include "TrigMonitoringEventTPCnv/TrigConfAlgCnv_p1.h"

void TrigConfAlgCnv_p1::persToTrans(const TrigConfAlg_p1* persObj, 
				    TrigConfAlg* transObj, 
				    MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigConfAlgCnv_p1::persToTrans called " << endreq;
  }

  transObj->m_index       = persObj->m_index;
  transObj->m_position    = persObj->m_position;  
  transObj->m_name_id     = persObj->m_name_id; 
  transObj->m_type_id     = persObj->m_type_id; 
  transObj->m_name        = persObj->m_name; 
  transObj->m_type        = persObj->m_type;
}


void TrigConfAlgCnv_p1::transToPers(const TrigConfAlg* transObj, 
				    TrigConfAlg_p1* persObj, 
				    MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigConfAlgCnv_p1::transToPers called " << endreq;
  }

  persObj->m_index       = transObj->m_index;
  persObj->m_position    = transObj->m_position;  
  persObj->m_name_id     = transObj->m_name_id; 
  persObj->m_type_id     = transObj->m_type_id; 
  persObj->m_name        = transObj->m_name; 
  persObj->m_type        = transObj->m_type;
}
