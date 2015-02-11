/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public

#include "TrigDecisionEvent/TrigDecision.h"
#include "TrigDecisionEventTPCnv/TrigDecision_p5.h"

#undef private
#undef protected

#include "TrigDecisionEventTPCnv/TrigDecisionCnv_p5.h"
#include "DataModelAthenaPool/DataLinkCnv_p2.h"
namespace {
DataLinkCnv_p2<DataLink<HLT::HLTResult> > dataLinkConverter;
}

using namespace TrigDec;


void TrigDecisionCnv_p5::transToPers(const TrigDec::TrigDecision* trans,
                                     TrigDecision_p5* pers, MsgStream &log)
{

  log << MSG::DEBUG << "TrigDecisionCnv_p5::transToPers called " << trans << " " << pers << endreq;

  pers->m_configMasterKey = trans->m_configMasterKey;
  pers->m_bgCode          = trans->m_bgCode;

  m_lvl1ResultCnv.transToPers(&trans->m_l1_result, &pers->m_l1_result, log);

  const_cast<DataLink<HLT::HLTResult>& >(trans->m_l2_result).toPersistent();

  dataLinkConverter.transToPers(trans->m_l2_result, pers->m_l2_result, log);

  dataLinkConverter.transToPers(trans->m_ef_result, pers->m_ef_result, log);

}

void TrigDecisionCnv_p5::persToTrans(const TrigDec::TrigDecision_p5* pers,
                                     TrigDecision* trans, MsgStream &log)
{
  log << MSG::DEBUG << "TrigDecisionCnv_p5::persToTrans called " << endreq;

  trans->m_configMasterKey = pers->m_configMasterKey;
  trans->m_bgCode          = pers->m_bgCode;

  m_lvl1ResultCnv.persToTrans(&pers->m_l1_result, &trans->m_l1_result, log);
  dataLinkConverter.persToTrans( &pers->m_l2_result, &trans->m_l2_result, log);
  dataLinkConverter.persToTrans( &pers->m_ef_result, &trans->m_ef_result, log);

  // trans->resetCache();
}
