// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TILETRACKMUFEATURECONTAINER
#define TRIG_TILETRACKMUFEATURECONTAINER

#include "AthenaKernel/CLASS_DEF.h"
#include "TrigMuonEvent/TileTrackMuFeature.h"
#include "AthenaKernel/BaseInfo.h"

class TileTrackMuFeatureContainer : public DataVector<TileTrackMuFeature> { }; 
CLASS_DEF( TileTrackMuFeatureContainer , 1294436667, 1)

SG_BASE(TileTrackMuFeatureContainer, DataVector<TileTrackMuFeature>);

#endif 
