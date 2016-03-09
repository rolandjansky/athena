/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4TruthStrategies_LLPStrategy_H
#define G4TruthStrategies_LLPStrategy_H

#include "MCTruthBase/TruthStrategy.h"

class LLPStrategy: public TruthStrategy {
  public:
   LLPStrategy(const std::string&);
   bool AnalyzeVertex(const G4Step*);
  private:
   bool isSUSYParticle(const int) const;
};

#endif
