/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_MUONFEATUREDETAILSCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_MUONFEATUREDETAILSCONTAINER_CNV_H

// #include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigMuonEvent/MuonFeatureDetailsContainer.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainer_p2.h"

typedef MuonFeatureDetailsContainer_p2   MuonFeatureDetailsContainer_PERS;

typedef T_AthenaPoolCustomCnv<MuonFeatureDetailsContainer, MuonFeatureDetailsContainer_PERS> MuonFeatureDetailsContainerCnvBase;

class MuonFeatureDetailsContainerCnv : public MuonFeatureDetailsContainerCnvBase {

  friend class CnvFactory<MuonFeatureDetailsContainerCnv>;

 protected:

  MuonFeatureDetailsContainerCnv( ISvcLocator *svcloc ): MuonFeatureDetailsContainerCnvBase(svcloc){}

  virtual MuonFeatureDetailsContainer_PERS *createPersistent( MuonFeatureDetailsContainer *transObj);
  virtual MuonFeatureDetailsContainer      *createTransient();

};


#endif
