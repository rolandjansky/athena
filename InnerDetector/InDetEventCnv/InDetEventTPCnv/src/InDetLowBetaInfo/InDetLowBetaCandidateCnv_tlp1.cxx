/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidateCnv_tlp1.h"


InDetLowBetaCandidateCnv_tlp1::InDetLowBetaCandidateCnv_tlp1() {
  // add the "main" base class converter (ie. InDetLowBetaCandidateCnv)
  addMainTPConverter();
}

void InDetLowBetaCandidateCnv_tlp1::setPStorage(InDetLowBetaCandidate_tlp1* storage) {
  // for the base class converter
  setMainCnvPStorage(&storage->m_lowBetaCandidate);
}


void T_TPCnv<InDet::InDetLowBetaCandidate, InDetLowBetaCandidate_tlp1 >::persToTrans ATLAS_NOT_THREAD_SAFE (const InDetLowBetaCandidate_tlp1 *pers,
											    InDet::InDetLowBetaCandidate *trans,
											    MsgStream &msg) {
  setPStorage(const_cast<InDetLowBetaCandidate_tlp1*>(pers));
  m_mainConverter.pstoreToTrans(0, trans, msg);
}

void T_TPCnv<InDet::InDetLowBetaCandidate, InDetLowBetaCandidate_tlp1 >::transToPers(const InDet::InDetLowBetaCandidate *trans,
										     InDetLowBetaCandidate_tlp1 *pers,
										     MsgStream &msg) {
//  if (log.level() <= MSG::DEBUG)  msg << MSG::DEBUG << "::transToPers called " << endmsg;
  this->setTLPersObject(pers);
  m_mainConverter.virt_toPersistent(trans, msg);
  this->clearTLPersObject();
}
