/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_MUONFEATUREDETAILSCONTAINER
#define TRIG_MUONFEATUREDETAILSCONTAINER

#include "AthenaKernel/CLASS_DEF.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include "AthenaKernel/BaseInfo.h"

class MuonFeatureDetailsContainer : public DataVector<MuonFeatureDetails> { }; 

CLASS_DEF( MuonFeatureDetailsContainer , 1224779873 , 1 )

SG_BASE(MuonFeatureDetailsContainer, DataVector<MuonFeatureDetails>);

#endif 
