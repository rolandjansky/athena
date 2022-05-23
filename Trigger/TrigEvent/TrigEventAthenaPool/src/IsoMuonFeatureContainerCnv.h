/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_ISOMUONFEATURECONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_ISOMUONFEATURECONTAINER_CNV_H
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigMuonEvent/IsoMuonFeatureContainer.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_p2.h" 
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_p3.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainerCnv_tlp1.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainerCnv_p2.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainerCnv_p3.h"

typedef IsoMuonFeatureContainer_p3   IsoMuonFeatureContainer_PERS;

typedef T_AthenaPoolCustomCnv<IsoMuonFeatureContainer, IsoMuonFeatureContainer_PERS> IsoMuonFeatureContainerCnvBase;

class IsoMuonFeatureContainerCnv : public IsoMuonFeatureContainerCnvBase {

  friend class CnvFactory<IsoMuonFeatureContainerCnv>;

 protected:

public:
  IsoMuonFeatureContainerCnv( ISvcLocator *svcloc ): IsoMuonFeatureContainerCnvBase(svcloc){}
protected:

  virtual IsoMuonFeatureContainer_PERS *createPersistent( IsoMuonFeatureContainer *transObj);
  virtual IsoMuonFeatureContainer      *createTransient();

private:
  IsoMuonFeatureContainerCnv_tlp1   m_converter1;
  IsoMuonFeatureContainerCnv_p2     m_converter2;
  IsoMuonFeatureContainerCnv_p3     m_converter;

};

#endif //ISOMUONFEATURECONTAINER_CNV_H
