/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Needed to be able to add TopInputMap<PhotonContainer> to store gate

#ifndef TOP_PHOTON_MAP_H
#define TOP_PHOTON_MAP_H

#include "egammaEvent/PhotonContainer.h"
#include "TopInputs/TopInputMap.h"
#include "SGTools/CLASS_DEF.h"

namespace TopRec {
  typedef  TopInputMap<PhotonContainer> TopPhotonMap;
  typedef  ElementLink<PhotonContainer> TopPhotonLink;
  typedef  std::pair< TopPhotonLink, TopOverlap > TopPhotonOverlapPair;
}

CLASS_DEF(TopRec::TopPhotonMap, 130214626, 1)

#endif // TOP_PHOTON_MAP_H
