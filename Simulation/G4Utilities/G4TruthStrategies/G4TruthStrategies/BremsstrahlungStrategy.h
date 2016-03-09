/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4TruthStrategies_BremsstrahlungStrategy_H
#define G4TruthStrategies_BremsstrahlungStrategy_H

#include "MCTruthBase/TruthStrategy.h"
class BremsstrahlungStrategy: public TruthStrategy {
public:
  BremsstrahlungStrategy(const std::string&);
  bool AnalyzeVertex(const G4Step*);
};

#endif
