/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONPATTERNCOMBINATIONCOLLECTION_H
#define MUON_MUONPATTERNCOMBINATIONCOLLECTION_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "MuonPattern/MuonPatternCombination.h"

/**
This typedef represents a collection of MuonPatternCombination objects.
It is a DataVector. It can be saved
to storegate and persistified using POOL.
*/
typedef DataVector<Muon::MuonPatternCombination> MuonPatternCombinationCollection;
CLASS_DEF(MuonPatternCombinationCollection, 1141513, 1)

#endif
