/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloCatchAllStrategy_H
#define CaloCatchAllStrategy_H

#include "MCTruthBase/TruthStrategy.h"
class CaloCatchAllStrategy: public TruthStrategy {
public:
	CaloCatchAllStrategy(const std::string);
	bool AnalyzeVertex(const G4Step*);
};

#endif
