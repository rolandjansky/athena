/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "InDetLowBetaInfo/InDetLowBetaCandidate.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidate_p1.h"
#undef private
#undef protected


#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidateCnv_p1.h"


void InDetLowBetaCandidateCnv_p1::persToTrans(const InDetLowBetaCandidate_p1* persObj, InDet::InDetLowBetaCandidate* transObj, MsgStream &/*log*/)
{
  size_t sz = persObj->m_data.size();
  transObj->m_TRTCorrBitsOverThreshold = sz>0 ? (persObj->m_data)[0] : 0;
  transObj->m_TRTTrailingEdge          = sz>1 ? (persObj->m_data)[1] : 0;
  transObj->m_TRTTrailingEdgeError     = sz>2 ? (persObj->m_data)[2] : 0;
  transObj->m_TRTdEdx                  = sz>3 ? (persObj->m_data)[3] : 0;
  transObj->m_TRTLikelihoodBeta        = sz>4 ? (persObj->m_data)[4] : 0;
  transObj->m_TRTLikelihoodError       = sz>5 ? (persObj->m_data)[5] : 0;
  transObj->m_TRTHighTbits             = sz>6 ? (persObj->m_data)[6] : 0;
}


void InDetLowBetaCandidateCnv_p1::transToPers(const InDet::InDetLowBetaCandidate* transObj, InDetLowBetaCandidate_p1* persObj, MsgStream &/*log*/)
{
  persObj->m_data.push_back(transObj->m_TRTCorrBitsOverThreshold);
  persObj->m_data.push_back(transObj->m_TRTTrailingEdge);
  persObj->m_data.push_back(transObj->m_TRTTrailingEdgeError);
  persObj->m_TRTNLastBits = transObj->m_TRTNLastBits;
  persObj->m_data.push_back(transObj->m_TRTdEdx);
  persObj->m_data.push_back(transObj->m_TRTLikelihoodBeta);
  persObj->m_data.push_back(transObj->m_TRTLikelihoodError);
  persObj->m_data.push_back(transObj->m_TRTHighTbits);

}

