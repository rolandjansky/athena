/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitoringEvent/TrigConfAlg.h"
#include "TrigMonitoringEventTPCnv/TrigConfAlg_p1.h"
#include "TrigMonitoringEventTPCnv/TrigConfAlgCnv_p1.h"

void TrigConfAlgCnv_p1::persToTrans(const TrigConfAlg_p1* persObj, 
				    TrigConfAlg* transObj, 
				    MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigConfAlgCnv_p1::persToTrans called " << endmsg;
  }

  *transObj = TrigConfAlg (persObj->m_index,
                           persObj->m_position,
                           persObj->m_name,
                           persObj->m_type,
                           persObj->m_name_id,
                           persObj->m_type_id);
}


void TrigConfAlgCnv_p1::transToPers(const TrigConfAlg* transObj, 
				    TrigConfAlg_p1* persObj, 
				    MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigConfAlgCnv_p1::transToPers called " << endmsg;
  }

  persObj->m_index       = transObj->getIndex();
  persObj->m_position    = transObj->getPosition();  
  persObj->m_name_id     = transObj->getNameId(); 
  persObj->m_type_id     = transObj->getTypeId(); 
  persObj->m_name        = transObj->getName(); 
  persObj->m_type        = transObj->getType();
}
