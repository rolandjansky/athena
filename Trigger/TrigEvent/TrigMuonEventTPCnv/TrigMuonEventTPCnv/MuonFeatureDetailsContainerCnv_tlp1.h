/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGMUONEVENTTPCNV_MUONFEATUREDETAILSCONTAINER_CNV_TLP1_H
#define  TRIGMUONEVENTTPCNV_MUONFEATUREDETAILSCONTAINER_CNV_TLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainer_tlp1.h"

#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainerCnv_p1.h"

class MuonFeatureDetailsContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< MuonFeatureDetailsContainerCnv_p1, MuonFeatureDetailsContainer_tlp1 >
{

 public:

  MuonFeatureDetailsContainerCnv_tlp1();
  virtual ~MuonFeatureDetailsContainerCnv_tlp1() {}

  //default methods
  virtual void  setPStorage( MuonFeatureDetailsContainer_tlp1 *storage );

 protected:
  MuonFeatureDetailsContainerCnv_p1    m_muonFeatureDetailsContainerCnv;
  MuonFeatureDetailsCnv_p1             m_muonFeatureDetailsCnv;

}; //end of class definitions


#endif
