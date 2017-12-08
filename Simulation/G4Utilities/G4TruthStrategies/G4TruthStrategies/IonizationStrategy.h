/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4TruthStrategies_IonizationStrategy_H
#define G4TruthStrategies_IonizationStrategy_H

#include "MCTruthBase/TruthStrategy.h"
class IonizationStrategy: public TruthStrategy {
public:
  IonizationStrategy(const std::string&);
  bool AnalyzeVertex(const G4Step*);
};

#endif
