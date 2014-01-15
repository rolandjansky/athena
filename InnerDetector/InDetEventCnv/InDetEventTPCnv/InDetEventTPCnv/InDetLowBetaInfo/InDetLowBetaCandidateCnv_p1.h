/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetLowBetaCandidateCnv_P1_H
#define InDetLowBetaCandidateCnv_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "InDetLowBetaInfo/InDetLowBetaCandidate.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidate_p1.h"

class MsgStream;

class InDetLowBetaCandidateCnv_p1  : public T_AthenaPoolTPCnvBase<InDet::InDetLowBetaCandidate, InDetLowBetaCandidate_p1> {

 public:
  InDetLowBetaCandidateCnv_p1() {}

  virtual void          persToTrans(const InDetLowBetaCandidate_p1* persObj, InDet::InDetLowBetaCandidate* transObj, MsgStream &log);
  virtual void          transToPers(const InDet::InDetLowBetaCandidate* transObj, InDetLowBetaCandidate_p1* persObj, MsgStream &log);

};

#endif
