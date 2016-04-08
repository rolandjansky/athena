/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigDecisionEvent/TrigDecision.h"
#include "TrigDecisionEventTPCnv/TrigDecision_p4.h"
#include "TrigDecisionEventTPCnv/TrigDecisionCnv_p4.h"
#include "DataModelAthenaPool/DataLinkCnv_p1.h"
namespace {
DataLinkCnv_p1<DataLink<HLT::HLTResult> > dataLinkConverter;
}

using namespace TrigDec;


void TrigDecisionCnv_p4::transToPers(const TrigDec::TrigDecision* trans,
                                     TrigDecision_p4* pers, MsgStream &log)
{
  log << MSG::DEBUG << "TrigDecisionCnv_p4::transToPers called " << trans << " " << pers << endreq;

  pers->m_configMasterKey = trans->masterKey();
  pers->m_bgCode          = trans->BGCode();

  m_lvl1ResultCnv.transToPers(&trans->getL1Result(), &pers->m_l1_result, log);

  DataLink<HLT::HLTResult> l2_result = trans->getL2ResultLink();
  l2_result.toPersistent();
  dataLinkConverter.transToPers(l2_result, pers->m_l2_result, log);

  dataLinkConverter.transToPers(trans->getEFResultLink(), pers->m_ef_result, log);
}

void TrigDecisionCnv_p4::persToTrans(const TrigDec::TrigDecision_p4* pers,
                                     TrigDecision* trans, MsgStream &log)
{
  log << MSG::DEBUG << "TrigDecisionCnv_p4::persToTrans called " << endreq;

  LVL1CTP::Lvl1Result l1_result;  
  m_lvl1ResultCnv.persToTrans(&pers->m_l1_result, &l1_result, log);

  DataLink<HLT::HLTResult> l2_result;
  dataLinkConverter.persToTrans( &pers->m_l2_result, &l2_result, log);

  DataLink<HLT::HLTResult> ef_result;
  dataLinkConverter.persToTrans( &pers->m_ef_result, &ef_result, log);

  *trans = TrigDecision (l1_result,
                         l2_result,
                         ef_result,
                         pers->m_configMasterKey,
                         pers->m_bgCode);
}
