/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPATTERN_MUONPATTERNDICT_H
#define MUONPATTERN_MUONPATTERNDICT_H

#include "MuonPattern/MuonPatternCombination.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"

class MuonPatternDummyDict {
    MuonPatternCombinationCollection m_myPattCombColl;
    Muon::MuonPatternCombination m_myTempPattComb;
};

#endif
