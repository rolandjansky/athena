/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetLowBetaCandidateCnv.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidateCnv_tlp1.h"
#include "MsgUtil.h"

InDetLowBetaCandidateCnv::InDetLowBetaCandidateCnv(ISvcLocator* svcloc) :
  T_AthenaPoolCustomCnv<InDet::InDetLowBetaCandidate, InDetLowBetaCandidate_PERS >( svcloc ),
  m_TPConverter (new InDetLowBetaCandidateCnv_tlp1)
{ }
InDetLowBetaCandidateCnv::~InDetLowBetaCandidateCnv()
{
  delete m_TPConverter;
}



InDetLowBetaCandidate_PERS* InDetLowBetaCandidateCnv::createPersistent(InDet::InDetLowBetaCandidate* transCont) {
  IDEvtAthPool::setMsgName(this,"InDetLowBetaCandidateConverter");//So msg() won't use name "AthenaPoolConverter" 
  //do it in both createPersistent and createTransient since there is no initialize method (could add it of course)

  ATH_MSG_DEBUG("InDetLowBetaCandidateCnv::createPersistent");

  InDetLowBetaCandidate_PERS *persObj = m_TPConverter->createPersistent(transCont, msg());
  
  return persObj;
}


InDet::InDetLowBetaCandidate* InDetLowBetaCandidateCnv::createTransient() {
  IDEvtAthPool::setMsgName(this,"InDetLowBetaCandidateConverter");//So msg() won't use name "AthenaPoolConverter" 
  //do it in both createPersistent and createTransient since there is no initialize method (could add it of course)

  ATH_MSG_DEBUG("InDetLowBetaCandidateCnv::createTransient ");
  
  static pool::Guid tlp1_guid("8C24589F-FBAA-4686-9254-B5C360A94733");

  InDet::InDetLowBetaCandidate *trans_cont(0);
  
  if (compareClassGuid(tlp1_guid) ) {
    ATH_MSG_DEBUG("InDetLowBetaCandidateCnv::reading tlp1 persistent object");
    std::unique_ptr<InDetLowBetaCandidate_tlp1> col_vect(this->poolReadObject<InDetLowBetaCandidate_tlp1>());
    trans_cont = m_TPConverter->createTransient(col_vect.get(), msg());
  } else {
    throw std::runtime_error("Unsupported persistent version of Data container");
  }
  return trans_cont;
}
