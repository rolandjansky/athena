/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMonitoringEvent/TrigConfChain.h"
#undef private
#undef protected

// This class data and converter
#include "TrigMonitoringEventTPCnv/TrigConfChain_p1.h"
#include "TrigMonitoringEventTPCnv/TrigConfChainCnv_p1.h"

void TrigConfChainCnv_p1::persToTrans(const TrigConfChain_p1* persObj, 
				      TrigConfChain* transObj, 
				      MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigConfChainCnv_p1::persToTrans called " << endreq;
  }

  transObj->m_chain_name       = persObj->m_chain_name;
  transObj->m_lower_name       = persObj->m_lower_name;
  transObj->m_chain_id         = persObj->m_chain_id;
  transObj->m_lower_id         = persObj->m_lower_id;
  transObj->m_chain_counter    = persObj->m_chain_counter;
  transObj->m_lower_counter    = persObj->m_lower_counter;
  transObj->m_level            = persObj->m_level;
  transObj->m_prescale         = persObj->m_prescale;
  transObj->m_pass_through     = persObj->m_pass_through;
  transObj->m_lower_ids        = persObj->m_lower_ids;
  transObj->m_stream_prescale  = persObj->m_stream_prescale;
  transObj->m_stream_name      = persObj->m_stream_name;
  transObj->m_group            = persObj->m_group;

  m_sigCnv.persToTrans(&(persObj->m_signature), &(transObj->m_signature), log);
}


void TrigConfChainCnv_p1::transToPers(const TrigConfChain* transObj, 
				      TrigConfChain_p1* persObj, 
				      MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigConfChainCnv_p1::transToPers called " << endreq;
  }

  persObj->m_chain_name      = transObj->m_chain_name;
  persObj->m_lower_name      = transObj->m_lower_name;
  persObj->m_chain_id        = transObj->m_chain_id;
  persObj->m_lower_id        = transObj->m_lower_id;
  persObj->m_chain_counter   = transObj->m_chain_counter;
  persObj->m_lower_counter   = transObj->m_lower_counter;
  persObj->m_level           = transObj->m_level;
  persObj->m_prescale        = transObj->m_prescale;
  persObj->m_pass_through    = transObj->m_pass_through;
  persObj->m_lower_ids       = transObj->m_lower_ids;
  persObj->m_stream_prescale = transObj->m_stream_prescale;
  persObj->m_stream_name     = transObj->m_stream_name;
  persObj->m_group           = transObj->m_group;

  m_sigCnv.transToPers(&(transObj->m_signature), &(persObj->m_signature), log);
}
