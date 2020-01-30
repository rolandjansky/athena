/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitoringEvent/TrigMonEvent.h"

// This class data and converter
#include "TrigMonitoringEventTPCnv/TrigMonEvent_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonEventCnv_p1.h"

void TrigMonEventCnv_p1::persToTrans(const TrigMonEvent_p1* persObj, 
				     TrigMonEvent* transObj, 
				     MsgStream &log) const
{
  transObj->setEventID (persObj->m_event,
                        persObj->m_lumi,
                        persObj->m_bxid,
                        persObj->m_run,
                        persObj->m_sec,
                        persObj->m_nsec);

  transObj->m_lumi = persObj->m_lumi;
  transObj->m_bxid = persObj->m_bxid;

  transObj->getWord()       = persObj->m_word;
  transObj->getL1Item()     = persObj->m_l1;
  transObj->getChain()      = persObj->m_hlt;
  transObj->getVarKey()     = persObj->m_var_key;
  transObj->getVarVal()     = persObj->m_var_val;

  m_robCnv.persToTrans(&(persObj->m_rob),
                       &transObj->getVec<TrigMonROB>(),
                       log);
  m_roiCnv.persToTrans(&(persObj->m_roi),
                       &transObj->getVec<TrigMonRoi>(),
                       log);
  m_seqCnv.persToTrans(&(persObj->m_seq),
                       &transObj->getVec<TrigMonSeq>(),
                       log);
  m_teCnv. persToTrans(&(persObj->m_te),
                       &transObj->getVec<TrigMonTE>(),
                       log);
}


void TrigMonEventCnv_p1::transToPers(const TrigMonEvent* transObj, 
				     TrigMonEvent_p1* persObj, 
				     MsgStream &log) const
{
  persObj->m_event      = transObj->getEvent();
  persObj->m_run        = transObj->getRun();
  persObj->m_sec        = transObj->getSec();
  persObj->m_nsec       = transObj->getNanoSec();
  persObj->m_word       = transObj->getWord();
  persObj->m_l1         = transObj->getL1Item();
  persObj->m_hlt        = transObj->getChain();
  persObj->m_var_key    = transObj->getVarKey();
  persObj->m_var_val    = transObj->getVarVal();

  persObj->m_lumi       = transObj->m_lumi;
  persObj->m_bxid       = transObj->m_bxid;

  m_robCnv.transToPers(&transObj->getVec<TrigMonROB>(),
                       &(persObj->m_rob), log);
  m_roiCnv.transToPers(&transObj->getVec<TrigMonRoi>(),
                       &(persObj->m_roi), log);
  m_seqCnv.transToPers(&transObj->getVec<TrigMonSeq>(),
                       &(persObj->m_seq), log);
  m_teCnv. transToPers(&transObj->getVec<TrigMonTE>(),
                       &(persObj->m_te),  log);
}
