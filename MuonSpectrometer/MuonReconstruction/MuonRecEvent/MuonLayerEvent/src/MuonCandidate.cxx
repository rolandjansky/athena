/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerEvent/MuonCandidate.h"


namespace Muon {
  MuonCandidate::MuonCandidate(){}

  MuonCandidate::MuonCandidate( std::vector<MuonLayerIntersection>&& layerIntersections_) :
    layerIntersections(layerIntersections_) {}


}
