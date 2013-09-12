/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public

#include "TrigDecisionEvent/TrigDecision.h"
#include "TrigDecisionEventTPCnv/TrigDecision_p3.h"

#undef private
#undef protected

#include "TrigDecisionEventTPCnv/TrigDecisionCnv_p3.h"
#include "DataModelAthenaPool/DataLinkCnv_p1.h"
namespace {
DataLinkCnv_p1<DataLink<HLT::HLTResult> > dataLinkConverter;
}

using namespace TrigDec;


void TrigDecisionCnv_p3::transToPers(const TrigDec::TrigDecision* trans,
                                     TrigDecision_p3* pers, MsgStream &log)
{

  log << MSG::DEBUG << "TrigDecisionCnv_p3::transToPers called " << trans << " " << pers << endreq;

  pers->m_configMasterKey = trans->m_configMasterKey;


  m_lvl1ResultCnv.transToPers(&trans->m_l1_result, &pers->m_l1_result, log);
  //     log << MSG::ERROR << "TrigDecisionCnv_p3::done L1 " << &trans->m_l1_result << " " << &pers->m_l1_result << endreq;


  const_cast<DataLink<HLT::HLTResult>& >(trans->m_l2_result).toPersistent();
  //     log << MSG::ERROR << "TrigDecisionCnv_p3::doooing L2 "
  //	 << " " << trans->m_l2_result.dataID() <<endreq;
  //     log << MSG::ERROR << "TrigDecisionCnv_p3::printing L2 >"
  //	 << pers->m_l2_result.m_link << "< dupa" << endreq;

  dataLinkConverter.transToPers(trans->m_l2_result, pers->m_l2_result, log);
  //     pers->m_l2_result.m_link  = trans->m_l2_result.dataID();
  //     log << MSG::ERROR << "TrigDecisionCnv_p3::done0 L2 " << endreq;
  //     log << MSG::ERROR << "TrigDecisionCnv_p3::done L2 " << &trans->m_l2_result << " " << &pers->m_l2_result <<endreq;
  //     log << MSG::ERROR << "TrigDecisionCnv_p3::doing EF " << &trans->m_ef_result << " " << &pers->m_ef_result <<endreq;

  dataLinkConverter.transToPers(trans->m_ef_result, pers->m_ef_result, log);

  //     log << MSG::ERROR << "TrigDecisionCnv_p3::done EF " << &trans->m_ef_result << " " << &pers->m_ef_result << endreq;

}

void TrigDecisionCnv_p3::persToTrans(const TrigDec::TrigDecision_p3* pers,
                                     TrigDecision* trans, MsgStream &log)
{
  log << MSG::DEBUG << "TrigDecisionCnv_p3::persToTrans called " << endreq;

  trans->m_configMasterKey = pers->m_configMasterKey;

  m_lvl1ResultCnv.persToTrans(&pers->m_l1_result, &trans->m_l1_result, log);
  dataLinkConverter.persToTrans( &pers->m_l2_result, &trans->m_l2_result, log);
  dataLinkConverter.persToTrans( &pers->m_ef_result, &trans->m_ef_result, log);

  //  m_hltResultCnv.persToTrans (&pers->m_l2_result, &trans->m_l2_result, log);
  //  m_hltResultCnv.persToTrans (&pers->m_ef_result, &trans->m_ef_result, log);

  //trans->resetCache();
}
