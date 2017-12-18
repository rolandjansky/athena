// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_ISOMUONFEATURECONTAINER
#define TRIG_ISOMUONFEATURECONTAINER

#include "AthenaKernel/CLASS_DEF.h"
#include "TrigMuonEvent/IsoMuonFeature.h"
#include "AthenaKernel/BaseInfo.h"

class IsoMuonFeatureContainer : public DataVector<IsoMuonFeature> { }; 

CLASS_DEF( IsoMuonFeatureContainer , 1193777390 , 1 )

SG_BASE(IsoMuonFeatureContainer, DataVector<IsoMuonFeature>);

#endif 
