/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_MUONFEATUREDETAILSCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_MUONFEATUREDETAILSCONTAINER_CNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigMuonEvent/MuonFeatureDetailsContainer.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainer_p2.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainerCnv_tlp1.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainerCnv_p2.h"

typedef MuonFeatureDetailsContainer_p2   MuonFeatureDetailsContainer_PERS;

typedef T_AthenaPoolCustomCnv<MuonFeatureDetailsContainer, MuonFeatureDetailsContainer_PERS> MuonFeatureDetailsContainerCnvBase;

class MuonFeatureDetailsContainerCnv : public MuonFeatureDetailsContainerCnvBase {

  friend class CnvFactory<MuonFeatureDetailsContainerCnv>;

 protected:

public:
  MuonFeatureDetailsContainerCnv( ISvcLocator *svcloc ): MuonFeatureDetailsContainerCnvBase(svcloc){}
protected:

  virtual MuonFeatureDetailsContainer_PERS *createPersistent( MuonFeatureDetailsContainer *transObj);
  virtual MuonFeatureDetailsContainer      *createTransient();

private:
  MuonFeatureDetailsContainerCnv_tlp1   m_converter1;
  MuonFeatureDetailsContainerCnv_p2     m_converter;

};


#endif
