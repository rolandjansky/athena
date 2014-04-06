/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Needed to be able to add TopInputMap<TauJetContainer> to store gate

#ifndef TOP_TAU_JET_MAP_H
#define TOP_TAU_JET_MAP_H

#include "tauEvent/TauJetContainer.h"
#include "TopInputs/TopInputMap.h"
#include "SGTools/CLASS_DEF.h"

namespace TopRec {
  typedef  TopInputMap<Analysis::TauJetContainer> TopTauJetMap;
  typedef  ElementLink<Analysis::TauJetContainer> TopTauJetLink;
  typedef  std::pair< TopTauJetLink, TopOverlap > TopTauJetOverlapPair;
}

CLASS_DEF(TopRec::TopTauJetMap, 472917850, 1)

#endif // TOP_TAU_JET_MAP_H
