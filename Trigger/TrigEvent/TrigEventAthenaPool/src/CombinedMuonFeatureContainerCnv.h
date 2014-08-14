/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_COMBINEDMUONFEATURECONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_COMBINEDMUONFEATURECONTAINER_CNV_H
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p2.h" 
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p3.h" 
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p4.h" 

typedef CombinedMuonFeatureContainer_p4   CombinedMuonFeatureContainer_PERS;

typedef T_AthenaPoolCustomCnv<CombinedMuonFeatureContainer, CombinedMuonFeatureContainer_PERS> CombinedMuonFeatureContainerCnvBase;

class CombinedMuonFeatureContainerCnv : public CombinedMuonFeatureContainerCnvBase {

  friend class CnvFactory<CombinedMuonFeatureContainerCnv>;

 protected:

  CombinedMuonFeatureContainerCnv( ISvcLocator *svcloc ): CombinedMuonFeatureContainerCnvBase(svcloc){}

  virtual CombinedMuonFeatureContainer_PERS *createPersistent( CombinedMuonFeatureContainer *transObj);
  virtual CombinedMuonFeatureContainer      *createTransient();

};

#endif //COMBINEDMUONFEATURECONTAINER_CNV_H
