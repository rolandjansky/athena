/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigDecisionEvent/TrigDecision.h"
#include "TrigDecisionEventTPCnv/TrigDecision_p2.h"
#include "TrigDecisionEventTPCnv/TrigDecisionCnv_p2.h"
#include "DataModelAthenaPool/DataLinkCnv_p1.h"
namespace {
  const DataLinkCnv_p1<DataLink<HLT::HLTResult> > dataLinkConverter;
}

using namespace TrigDec;


void TrigDecisionCnv_p2::transToPers(const TrigDec::TrigDecision* trans, 
                                     TrigDecision_p2* pers, MsgStream &log) const
{

  log << MSG::DEBUG << "TrigDecisionCnv_p2::transToPers called " << trans << " " << pers << endmsg;

  pers->m_configMasterKey = trans->masterKey();
  
  
  m_lvl1ResultCnv.transToPers(&trans->getL1Result(), &pers->m_l1_result, log);
  //     log << MSG::ERROR << "TrigDecisionCnv_p2::done L1 " << &trans->m_l1_result << " " << &pers->m_l1_result << endmsg;


  DataLink<HLT::HLTResult> l2_result = trans->getL2ResultLink();
  l2_result.toPersistent();
  dataLinkConverter.transToPers(l2_result, pers->m_l2_result, log);

  dataLinkConverter.transToPers(trans->getEFResultLink(), pers->m_ef_result, log);
}

void TrigDecisionCnv_p2::persToTrans(const TrigDec::TrigDecision_p2* pers, 
                                     TrigDecision* trans, MsgStream &log) const
{
  log << MSG::DEBUG << "TrigDecisionCnv_p2::persToTrans called " << endmsg;

  LVL1CTP::Lvl1Result l1_result;  
  m_lvl1ResultCnv.persToTrans(&pers->m_l1_result, &l1_result, log);

  DataLink<HLT::HLTResult> l2_result;
  dataLinkConverter.persToTrans( &pers->m_l2_result, &l2_result, log);

  DataLink<HLT::HLTResult> ef_result;
  dataLinkConverter.persToTrans( &pers->m_ef_result, &ef_result, log);

  *trans = TrigDecision (l1_result,
                         l2_result,
                         ef_result,
                         pers->m_configMasterKey);
}
