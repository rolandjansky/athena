/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigDecisionEvent/TrigDecision.h"
#include "TrigDecisionEventTPCnv/TrigDecision_p1.h"
#include "TrigDecisionEventTPCnv/TrigDecisionCnv_p1.h"

using namespace TrigDec;

// this method is not needd anymore
void TrigDecisionCnv_p1::transToPers(const TrigDec::TrigDecision* trans, 
                                     TrigDecision_p1* pers, MsgStream &log)
{

  log << MSG::DEBUG << "TrigDecisionCnv_p1::transToPers called " << endreq;

  pers->m_configMasterKey = trans->masterKey();

  m_lvl1ResultCnv.transToPers(&trans->getL1Result(), &pers->m_l1_result, log);
  m_hltResultCnv.transToPers (&trans->getL2Result(), &pers->m_l2_result, log);
  m_hltResultCnv.transToPers (&trans->getEFResult(), &pers->m_ef_result, log);

}

void TrigDecisionCnv_p1::persToTrans(const TrigDec::TrigDecision_p1* pers, 
                                     TrigDecision* trans, MsgStream &log)
{
  log << MSG::DEBUG << "TrigDecisionCnv_p1::persToTrans called " << endreq;

  trans->m_configMasterKey = pers->m_configMasterKey;

  LVL1CTP::Lvl1Result l1result;
  m_lvl1ResultCnv.persToTrans(&pers->m_l1_result, &l1result, log);

  trans->m_l2_result_ptr = new HLT::HLTResult();
  trans->m_ef_result_ptr = new HLT::HLTResult();
  m_hltResultCnv.persToTrans (&pers->m_l2_result, trans->m_l2_result_ptr, log);
  m_hltResultCnv.persToTrans (&pers->m_ef_result, trans->m_ef_result_ptr, log);

  //trans->resetCache();
}
