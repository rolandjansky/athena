/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4TruthStrategies_ConversionStrategy_H
#define G4TruthStrategies_ConversionStrategy_H

#include "MCTruthBase/TruthStrategy.h"
class ConversionStrategy: public TruthStrategy {
public:
  ConversionStrategy(const std::string&);
  bool AnalyzeVertex(const G4Step*);
};

#endif
