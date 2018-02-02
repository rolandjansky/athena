/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_MUONFEATURECONTAINER
#define TRIG_MUONFEATURECONTAINER

#include "AthenaKernel/CLASS_DEF.h"

#ifndef TRIGMUONEVENT_MUON_FEATURE_H
#include "TrigMuonEvent/MuonFeature.h"
#endif

#include "AthenaKernel/BaseInfo.h"

class MuonFeatureContainer : public DataVector<MuonFeature> { }; 

CLASS_DEF( MuonFeatureContainer , 1195782638 , 1 )

SG_BASE(MuonFeatureContainer, DataVector<MuonFeature>);

#endif 
