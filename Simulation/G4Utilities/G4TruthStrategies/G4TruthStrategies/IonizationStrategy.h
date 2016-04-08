/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IonizationStrategy_H
#define IonizationStrategy_H

#include "MCTruthBase/TruthStrategy.h"
class IonizationStrategy: public TruthStrategy {
public:
	IonizationStrategy(const std::string);
	bool AnalyzeVertex(const G4Step*);
};

#endif
