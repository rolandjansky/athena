/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4TruthStrategies_HadronicInteractionStrategy_H
#define G4TruthStrategies_HadronicInteractionStrategy_H

#include "MCTruthBase/TruthStrategy.h"
class HadronicInteractionStrategy: public TruthStrategy {
public:
  HadronicInteractionStrategy(const std::string&);
  bool AnalyzeVertex(const G4Step*);
};

#endif
