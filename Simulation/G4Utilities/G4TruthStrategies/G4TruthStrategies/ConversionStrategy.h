/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ConversionStrategy_H
#define ConversionStrategy_H

#include "MCTruthBase/TruthStrategy.h"
class ConversionStrategy: public TruthStrategy {
public:
	ConversionStrategy(const std::string);
	bool AnalyzeVertex(const G4Step*);
};

#endif
