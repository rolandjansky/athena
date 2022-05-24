/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TILEMUFEATURECONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TILEMUFEATURECONTAINER_CNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigMuonEvent/TileMuFeatureContainer.h"
#include "TrigMuonEventTPCnv/TileMuFeatureContainer_p2.h"
#include "TrigMuonEventTPCnv/TileMuFeatureContainerCnv_tlp1.h"
#include "TrigMuonEventTPCnv/TileMuFeatureContainerCnv_p2.h"

typedef TileMuFeatureContainer_p2   TileMuFeatureContainer_PERS;

typedef T_AthenaPoolCustomCnv<TileMuFeatureContainer, TileMuFeatureContainer_PERS> TileMuFeatureContainerCnvBase;

class TileMuFeatureContainerCnv : public TileMuFeatureContainerCnvBase {

  friend class CnvFactory<TileMuFeatureContainerCnv>;

 protected:

public:
  TileMuFeatureContainerCnv( ISvcLocator *svcloc ): TileMuFeatureContainerCnvBase(svcloc){}
protected:

  virtual TileMuFeatureContainer_PERS *createPersistent( TileMuFeatureContainer *transObj);
  virtual TileMuFeatureContainer      *createTransient();

private:
  TileMuFeatureContainerCnv_tlp1   m_converter1;
  TileMuFeatureContainerCnv_p2     m_converter;

};

#endif //TILEMUFEATURECONTAINER_CNV_H
