/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitoringEvent/TrigConfSeq.h"

// This class data and converter
#include "TrigMonitoringEventTPCnv/TrigConfSeq_p1.h"
#include "TrigMonitoringEventTPCnv/TrigConfSeqCnv_p1.h"

void TrigConfSeqCnv_p1::persToTrans(const TrigConfSeq_p1* persObj, 
				    TrigConfSeq* transObj, 
				    MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigConfSeqCnv_p1::persToTrans called " << endmsg;
  }

  *transObj = TrigConfSeq (persObj->m_output_te_id,
                           persObj->m_output_te_index,
                           persObj->m_output_te_name);
  transObj->setTopoTE (persObj->m_topo_te);
  for (uint32_t te : persObj->m_input_te) {
    transObj->addInputTE (te);
  }

  std::vector<TrigConfAlg> tmp;
  m_algCnv.persToTrans(&(persObj->m_alg), &tmp, log);
  for (const TrigConfAlg& alg : tmp) {
    transObj->addAlg (alg);
  }
}


void TrigConfSeqCnv_p1::transToPers(const TrigConfSeq* transObj, 
				    TrigConfSeq_p1* persObj, 
				    MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigConfSeqCnv_p1::transToPers called " << endmsg;
  }

  persObj->m_output_te_name  = transObj->getName();
  persObj->m_output_te_index = transObj->getIndex();
  persObj->m_output_te_id    = transObj->getId();
  persObj->m_topo_te         = transObj->getTopoTE();
  persObj->m_input_te        = transObj->getInputTEs();

  m_algCnv.transToPers(&(transObj->getAlg()), &(persObj->m_alg), log);
}
