// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_COMBINEDMUONFEATURECONTAINER
#define TRIG_COMBINEDMUONFEATURECONTAINER

#include "CLIDSvc/CLASS_DEF.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "SGTools/BaseInfo.h"

class CombinedMuonFeatureContainer : public DataVector<CombinedMuonFeature> { }; 
CLASS_DEF( CombinedMuonFeatureContainer , 1078838239 , 1 )

SG_BASE(CombinedMuonFeatureContainer, DataVector<CombinedMuonFeature>);

#endif 
