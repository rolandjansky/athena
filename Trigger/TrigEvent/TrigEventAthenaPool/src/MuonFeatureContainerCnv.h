/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_MUONFEATURECONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_MUONFEATURECONTAINER_CNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainerCnv_tlp1.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainerCnv_tlp2.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainerCnv_p3.h"

typedef MuonFeatureContainer_p3   MuonFeatureContainer_PERS;

typedef T_AthenaPoolCustomCnv<MuonFeatureContainer, MuonFeatureContainer_PERS> MuonFeatureContainerCnvBase;

class MuonFeatureContainerCnv : public MuonFeatureContainerCnvBase {

  friend class CnvFactory<MuonFeatureContainerCnv>;

 protected:

public:
  MuonFeatureContainerCnv( ISvcLocator *svcloc ): MuonFeatureContainerCnvBase(svcloc){}
protected:

  virtual MuonFeatureContainer_PERS *createPersistent( MuonFeatureContainer *transObj);
  virtual MuonFeatureContainer      *createTransient();
private:
  MuonFeatureContainerCnv_tlp1   m_converter1;
  MuonFeatureContainerCnv_tlp2   m_converter2;
  MuonFeatureContainerCnv_p3     m_converter;

};

#endif //MUONFEATURECONTAINER_CNV_H

