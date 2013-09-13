/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_MUONFEATUREDETAILSCNV_P2_H
#define TRIGMUONEVENTTPCNV_MUONFEATUREDETAILSCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetails_p2.h"


class MsgStream;


class MuonFeatureDetailsCnv_p2 : public T_AthenaPoolTPCnvBase<MuonFeatureDetails, MuonFeatureDetails_p2>{

 public:

  MuonFeatureDetailsCnv_p2() {}

  virtual void persToTrans(const MuonFeatureDetails_p2* persObj, MuonFeatureDetails* transObj, MsgStream &log);
  virtual void transToPers(const MuonFeatureDetails* transObj, MuonFeatureDetails_p2* persObj, MsgStream &log);
};

#endif

