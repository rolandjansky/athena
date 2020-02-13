/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitoringEvent/TrigMonTE.h"

#include <iostream>
#include "TrigMonitoringEventTPCnv/TrigMonTE_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonTECnv_p1.h"

void TrigMonTECnv_p1::persToTrans(const TrigMonTE_p1* persObj, 
				   TrigMonTE* transObj, 
				   MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonTECnv_p1::persToTrans called " << endmsg;
  }

  *transObj = TrigMonTE (persObj->m_id,
                         persObj->m_encoded);
  transObj->addState (static_cast<TrigMonTE::State> (persObj->m_encoded));

  for (uint16_t index : persObj->m_child) {
    transObj->addChildIndex (index);
  }

  for (uint16_t index : persObj->m_parent) {
    transObj->addParentIndex (index);
  }

  for (uint8_t index : persObj->m_roi) {
    transObj->addRoiId (index);
  }

  for (uint32_t clid : persObj->m_clid) {
    transObj->addClid (clid);
  }

  transObj->m_var_key     = persObj->m_var_key;
  transObj->m_var_val     = persObj->m_var_val;
}


void TrigMonTECnv_p1::transToPers(const TrigMonTE* transObj, 
				   TrigMonTE_p1* persObj, 
				   MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonTECnv_p1::transToPers called " << endmsg;
  }

  persObj->m_id          = transObj->getId();
  persObj->m_child       = transObj->getChildIndex();
  persObj->m_parent      = transObj->getParentIndex();
  persObj->m_roi         = transObj->getRoiId();
  persObj->m_clid        = transObj->getClid();
  persObj->m_var_key     = transObj->getVarKey();
  persObj->m_var_val     = transObj->getVarVal();

  persObj->m_encoded     = transObj->m_encoded;
}
