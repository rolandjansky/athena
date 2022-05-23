/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_COMBINEDMUONFEATURECONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_COMBINEDMUONFEATURECONTAINER_CNV_H
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p2.h" 
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p3.h" 
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p4.h" 
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainerCnv_tlp1.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainerCnv_p2.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainerCnv_p3.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainerCnv_p4.h"

typedef CombinedMuonFeatureContainer_p4   CombinedMuonFeatureContainer_PERS;

typedef T_AthenaPoolCustomCnv<CombinedMuonFeatureContainer, CombinedMuonFeatureContainer_PERS> CombinedMuonFeatureContainerCnvBase;

class CombinedMuonFeatureContainerCnv : public CombinedMuonFeatureContainerCnvBase {

  friend class CnvFactory<CombinedMuonFeatureContainerCnv>;

 protected:

public:
  CombinedMuonFeatureContainerCnv( ISvcLocator *svcloc ): CombinedMuonFeatureContainerCnvBase(svcloc){}
protected:

  virtual CombinedMuonFeatureContainer_PERS *createPersistent( CombinedMuonFeatureContainer *transObj);
  virtual CombinedMuonFeatureContainer      *createTransient();
private:
  CombinedMuonFeatureContainerCnv_tlp1   m_converter1;
  CombinedMuonFeatureContainerCnv_p2     m_converter2;
  CombinedMuonFeatureContainerCnv_p3     m_converter3;
  CombinedMuonFeatureContainerCnv_p4     m_converter;


};

#endif //COMBINEDMUONFEATURECONTAINER_CNV_H
