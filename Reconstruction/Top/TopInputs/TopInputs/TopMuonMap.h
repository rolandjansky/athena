/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Needed to be able to add TopInputMap<MuonContainer> to store gate

#ifndef TOP_MUON_MAP_H
#define TOP_MUON_MAP_H

#include "muonEvent/MuonContainer.h"
#include "TopInputs/TopInputMapTrigger.h"
#include "SGTools/CLASS_DEF.h"

namespace TopRec {
  typedef  TopInputMapTrigger<Analysis::MuonContainer>	TopMuonMap;
  typedef  ElementLink<Analysis::MuonContainer>		TopMuonLink;
  typedef  std::pair< TopMuonLink, TopOverlapTrigger >	TopMuonOverlapPair;  
}

CLASS_DEF(TopRec::TopMuonMap, 73592123, 1)

#endif // TOP_MUON_MAP_H
