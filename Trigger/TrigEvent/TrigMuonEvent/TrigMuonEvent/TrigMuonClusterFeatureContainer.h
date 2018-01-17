// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUONCLUSTERFEATURECONTAINER
#define TRIG_TRIGMUONCLUSTERFEATURECONTAINER

#include "AthenaKernel/CLASS_DEF.h"
#include "TrigMuonEvent/TrigMuonClusterFeature.h"
#include "AthenaKernel/BaseInfo.h"

class TrigMuonClusterFeatureContainer : public DataVector<TrigMuonClusterFeature> { }; 

CLASS_DEF( TrigMuonClusterFeatureContainer , 1279277891 , 1 )

SG_BASE(TrigMuonClusterFeatureContainer, DataVector<TrigMuonClusterFeature>);

#endif 
