/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Needed to be able to add TopInputMap<ElectronContainer> to store gate

#ifndef TOP_ELECTRON_MAP_H
#define TOP_ELECTRON_MAP_H

#include "egammaEvent/ElectronContainer.h"
#include "TopInputs/TopInputMapTrigger.h"
#include "SGTools/CLASS_DEF.h"

namespace TopRec {
  typedef  TopInputMapTrigger<ElectronContainer> TopElectronMap;
  typedef  ElementLink<ElectronContainer> TopElectronLink;
  typedef  std::pair< TopElectronLink, TopOverlapTrigger > TopElectronOverlapPair;
}

CLASS_DEF(TopRec::TopElectronMap, 103847399, 1)

#endif // TOP_ELECTRON_MAP_H
