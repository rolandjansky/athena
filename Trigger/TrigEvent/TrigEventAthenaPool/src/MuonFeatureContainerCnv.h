/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_MUONFEATURECONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_MUONFEATURECONTAINER_CNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainer_p3.h"

typedef MuonFeatureContainer_p3   MuonFeatureContainer_PERS;

typedef T_AthenaPoolCustomCnv<MuonFeatureContainer, MuonFeatureContainer_PERS> MuonFeatureContainerCnvBase;

class MuonFeatureContainerCnv : public MuonFeatureContainerCnvBase {

  friend class CnvFactory<MuonFeatureContainerCnv>;

 protected:

  MuonFeatureContainerCnv( ISvcLocator *svcloc ): MuonFeatureContainerCnvBase(svcloc){}

  virtual MuonFeatureContainer_PERS *createPersistent( MuonFeatureContainer *transObj);
  virtual MuonFeatureContainer      *createTransient();

};

#endif //MUONFEATURECONTAINER_CNV_H

