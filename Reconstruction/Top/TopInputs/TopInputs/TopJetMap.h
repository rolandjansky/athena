/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Needed to be able to add TopInputMap<JetContainer> to store gate

#ifndef TOP_JET_MAP_H
#define TOP_JET_MAP_H

#include "JetEvent/JetCollection.h"
#include "TopInputs/TopInputMap.h"
#include "SGTools/CLASS_DEF.h"

namespace TopRec {
  typedef  TopInputMap<JetCollection>          TopJetMap;
  typedef  ElementLink<JetCollection>          TopJetLink;
  typedef  std::pair< TopJetLink, TopOverlap > TopJetOverlapPair;
}

CLASS_DEF(TopRec::TopJetMap, 74965939, 1)

#endif // TOP_JET_MAP_H
