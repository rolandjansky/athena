/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_MUONFEATUREDETAILSCNV_P1_H
#define TRIGMUONEVENTTPCNV_MUONFEATUREDETAILSCNV_P1_H

//converter base
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

//MuonFeature transient and persistent classes
#include "TrigMuonEventTPCnv/MuonFeatureDetails_p1.h"


class MsgStream;


class MuonFeatureDetailsCnv_p1 : public T_AthenaPoolTPCnvBase<MuonFeatureDetails, MuonFeatureDetails_p1>
{

 public:

  MuonFeatureDetailsCnv_p1() {}

  virtual void persToTrans(const MuonFeatureDetails_p1* persObj, MuonFeatureDetails* transObj, MsgStream &log);
  virtual void transToPers(const MuonFeatureDetails* transObj, MuonFeatureDetails_p1* persObj, MsgStream &log);
};

#endif

