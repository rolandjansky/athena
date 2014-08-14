/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_ISOMUONFEATURECONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_ISOMUONFEATURECONTAINER_CNV_H
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigMuonEvent/IsoMuonFeatureContainer.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_p2.h" 
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_p3.h" 

typedef IsoMuonFeatureContainer_p3   IsoMuonFeatureContainer_PERS;

typedef T_AthenaPoolCustomCnv<IsoMuonFeatureContainer, IsoMuonFeatureContainer_PERS> IsoMuonFeatureContainerCnvBase;

class IsoMuonFeatureContainerCnv : public IsoMuonFeatureContainerCnvBase {

  friend class CnvFactory<IsoMuonFeatureContainerCnv>;

 protected:

  IsoMuonFeatureContainerCnv( ISvcLocator *svcloc ): IsoMuonFeatureContainerCnvBase(svcloc){}

  virtual IsoMuonFeatureContainer_PERS *createPersistent( IsoMuonFeatureContainer *transObj);
  virtual IsoMuonFeatureContainer      *createTransient();

};

#endif //ISOMUONFEATURECONTAINER_CNV_H
