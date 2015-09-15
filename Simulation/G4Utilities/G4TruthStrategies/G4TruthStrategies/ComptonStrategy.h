/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ComptonStrategy_H
#define ComptonStrategy_H

#include "MCTruthBase/TruthStrategy.h"
class ComptonStrategy: public TruthStrategy {
public:
	ComptonStrategy(const std::string);
	bool AnalyzeVertex(const G4Step*);
};

#endif
