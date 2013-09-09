/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMonitoringEvent/TrigMonTE.h"
#undef private
#undef protected

#include <iostream>
#include "TrigMonitoringEventTPCnv/TrigMonTE_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonTECnv_p1.h"

void TrigMonTECnv_p1::persToTrans(const TrigMonTE_p1* persObj, 
				   TrigMonTE* transObj, 
				   MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonTECnv_p1::persToTrans called " << endreq;
  }

  transObj->m_id          = persObj->m_id;
  transObj->m_encoded     = persObj->m_encoded;  
  transObj->m_child       = persObj->m_child;
  transObj->m_parent      = persObj->m_parent;
  transObj->m_roi         = persObj->m_roi;
  transObj->m_clid        = persObj->m_clid;
  transObj->m_var_key     = persObj->m_var_key;
  transObj->m_var_val     = persObj->m_var_val;
}


void TrigMonTECnv_p1::transToPers(const TrigMonTE* transObj, 
				   TrigMonTE_p1* persObj, 
				   MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonTECnv_p1::transToPers called " << endreq;
  }

  persObj->m_id          = transObj->m_id;
  persObj->m_encoded     = transObj->m_encoded;  
  persObj->m_child       = transObj->m_child;
  persObj->m_parent      = transObj->m_parent;
  persObj->m_roi         = transObj->m_roi;
  persObj->m_clid        = transObj->m_clid;
  persObj->m_var_key     = transObj->m_var_key;
  persObj->m_var_val     = transObj->m_var_val;
}
