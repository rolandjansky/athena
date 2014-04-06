/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// typedefs for bjets

#ifndef TOP_B_JET_MAP_H
#define TOP_B_JET_MAP_H

#include "JetEvent/JetCollection.h"
#include "TopInputs/TopInputMap.h"

namespace TopRec {
  typedef  TopInputMap<JetCollection> TopBJetMap;
  typedef  ElementLink<JetCollection> TopBJetLink;
  typedef  std::pair< TopBJetLink, TopOverlap > TopBJetOverlapPair;
}

#endif // TOP_B_JET_MAP_H
