/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetLowBetaInfo/InDetLowBetaCandidate.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidate_p1.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidateCnv_p1.h"


void InDetLowBetaCandidateCnv_p1::persToTrans(const InDetLowBetaCandidate_p1* persObj, InDet::InDetLowBetaCandidate* transObj, MsgStream &/*log*/)
{
  size_t sz = persObj->m_data.size();
  *transObj = InDet::InDetLowBetaCandidate
    (sz>0 ? (persObj->m_data)[0] : 0, // TRTCorrBitsOverThreshold
     sz>1 ? (persObj->m_data)[1] : 0, // TRTTrailingEdge
     sz>2 ? (persObj->m_data)[2] : 0, // TRTTrailingEdgeError
     persObj->m_TRTNLastBits,
     sz>3 ? (persObj->m_data)[3] : 0, // TRTdEdx
     sz>4 ? (persObj->m_data)[4] : 0, // TRTLikelihoodBeta
     sz>5 ? (persObj->m_data)[5] : 0, // TRTLikelihoodError
     sz>6 ? (persObj->m_data)[6] : 0); // TRTHighTbits
}


void InDetLowBetaCandidateCnv_p1::transToPers(const InDet::InDetLowBetaCandidate* transObj, InDetLowBetaCandidate_p1* persObj, MsgStream &/*log*/)
{
  persObj->m_data.push_back(transObj->getTRTCorrBitsOverThreshold());
  persObj->m_data.push_back(transObj->getTRTTrailingEdge());
  persObj->m_data.push_back(transObj->getTRTTrailingEdgeError());
  persObj->m_TRTNLastBits = transObj->getTRTNLastBits();
  persObj->m_data.push_back(transObj->getTRTdEdx());
  persObj->m_data.push_back(transObj->getTRTLikelihoodBeta());
  persObj->m_data.push_back(transObj->getTRTLikelihoodError());
  persObj->m_data.push_back(transObj->getTRTHighTbits());

}

