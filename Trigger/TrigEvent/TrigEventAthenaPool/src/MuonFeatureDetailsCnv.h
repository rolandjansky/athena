/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_MUONFEATUREDETAILSCNV_H
#define TRIGEVENTATHENAPOOL_MUONFEATUREDETAILSCNV_H

#include "TrigMuonEvent/MuonFeatureDetails.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

class MuonFeatureDetails_p1;

class MsgStream;

// ARE THESE EVER SAVED STANDALONE? IF NOT THIS CLASS CAN/SHOULD BE REMOVED !!!

// typedef to the latest top level persistent version
// typedef MuonFeature_p1  MuonFeature_PERS;
typedef MuonFeatureDetails_p1  MuonFeatureDetails_PERS;

typedef T_AthenaPoolCustomCnv<MuonFeatureDetails, MuonFeatureDetails_PERS > MuonFeatureDetailsCnvBase;

class MuonFeatureDetailsCnv : public MuonFeatureDetailsCnvBase {

  friend class CnvFactory<MuonFeatureDetailsCnv>;

 protected:
  MuonFeatureDetailsCnv(ISvcLocator *svcloc);
  ~MuonFeatureDetailsCnv();

  virtual MuonFeatureDetails_PERS  *createPersistent(MuonFeatureDetails *transObj);
  virtual MuonFeatureDetails       *createTransient();
};


#endif
