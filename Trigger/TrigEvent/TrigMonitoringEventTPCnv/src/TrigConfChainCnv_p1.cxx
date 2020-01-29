/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitoringEvent/TrigConfChain.h"

// This class data and converter
#include "TrigMonitoringEventTPCnv/TrigConfChain_p1.h"
#include "TrigMonitoringEventTPCnv/TrigConfChainCnv_p1.h"

void TrigConfChainCnv_p1::persToTrans(const TrigConfChain_p1* persObj, 
				      TrigConfChain* transObj, 
				      MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigConfChainCnv_p1::persToTrans called " << endmsg;
  }

  *transObj = TrigConfChain (persObj->m_chain_name,
                             persObj->m_chain_counter,
                             persObj->m_chain_id,
                             persObj->m_level == 1 ? "L1" : "HLT",
                             persObj->m_lower_name,
                             persObj->m_lower_counter,
                             persObj->m_lower_id,
                             persObj->m_prescale,
                             persObj->m_pass_through);
  for (uint32_t id : persObj->m_lower_ids) {
    transObj->addLowerChainId (id);
  }

  transObj->getStreamPS() = persObj->m_stream_prescale;
  transObj->getStream() = persObj->m_stream_name;
  transObj->getGroup() = persObj->m_group;

  std::vector<TrigConfSig> tmp;
  m_sigCnv.persToTrans(&(persObj->m_signature), &tmp, log);
  for (const TrigConfSig& sig : tmp) {
    transObj->addSignature (sig);
  }
}


void TrigConfChainCnv_p1::transToPers(const TrigConfChain* transObj, 
				      TrigConfChain_p1* persObj, 
				      MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigConfChainCnv_p1::transToPers called " << endmsg;
  }

  persObj->m_chain_name      = transObj->getChainName();
  persObj->m_lower_name      = transObj->getLowerName();
  persObj->m_chain_id        = transObj->getId();
  persObj->m_lower_id        = transObj->getLowerId();
  persObj->m_chain_counter   = transObj->getCounter();
  persObj->m_lower_counter   = transObj->getLowerCounter();
  persObj->m_level           = transObj->getLevelId();
  persObj->m_prescale        = transObj->getPrescale();
  persObj->m_pass_through    = transObj->getPassThrough();
  persObj->m_lower_ids       = transObj->getLowerIds();
  persObj->m_stream_prescale = transObj->getStreamPS();
  persObj->m_stream_name     = transObj->getStream();
  persObj->m_group           = transObj->getGroup();

  m_sigCnv.transToPers(&(transObj->getSignature()), &(persObj->m_signature), log);
}
