/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_MUONFEATUREDETAILSCONTAINER_CNV_P1_H
#define TRIGMUONEVENTTPCNV_MUONFEATUREDETAILSCONTAINER_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigMuonEvent/MuonFeatureDetailsContainer.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainer_p1.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsCnv_p1.h"

class MuonFeatureDetailsContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< MuonFeatureDetailsContainer, MuonFeatureDetailsContainer_p1, ITPConverterFor<MuonFeatureDetails> >

{

 public:
  MuonFeatureDetailsContainerCnv_p1() {}

}; //end of class definitions

#endif
