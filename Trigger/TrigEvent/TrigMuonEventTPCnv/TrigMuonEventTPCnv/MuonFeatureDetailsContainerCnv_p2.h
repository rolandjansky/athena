/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_MUONFEATUREDETAILSCONTAINER_CNV_P2_H
#define TRIGMUONEVENTTPCNV_MUONFEATUREDETAILSCONTAINER_CNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigMuonEvent/MuonFeatureDetailsContainer.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainer_p2.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsCnv_p2.h"


typedef T_AthenaPoolTPCnvVector< MuonFeatureDetailsContainer, MuonFeatureDetailsContainer_p2, MuonFeatureDetailsCnv_p2 > MuonFeatureDetailsContainerCnv_p2;

#endif
