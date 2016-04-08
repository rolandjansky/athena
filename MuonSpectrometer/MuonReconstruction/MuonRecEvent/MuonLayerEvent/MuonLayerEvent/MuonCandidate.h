/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONCANDIDATE_H
#define MUON_MUONCANDIDATE_H

#include <vector>
#include "MuonLayerEvent/MuonLayerIntersection.h"


namespace Muon {
  
  struct MuonCandidate {
    /** constructors */
    MuonCandidate();
    MuonCandidate( std::vector<MuonLayerIntersection>&& layerIntersections_);

    /** MuonLayerIntersections */
    std::vector<MuonLayerIntersection> layerIntersections;

  };

}


#endif
