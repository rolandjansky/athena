/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INEFFICIENT_TRIGGER_H
#define INEFFICIENT_TRIGGER_H

#include "MuonCalibIdentifier/MuonFixedId.h"
#include <vector>

using std::vector;

class LowPtTrigger;

class InefficientTrigger{
	public:
		InefficientTrigger(LowPtTrigger* t, double globalInefficiency, int s);
		bool shouldTrigger(vector<unsigned int> hit_ids);
		bool doesTrigger(vector<unsigned int> hit_ids);
		vector<unsigned int> removeHits(vector<unsigned int> hit_ids);
		vector<unsigned int> removeHitsWithHardware(vector<unsigned int> hit_ids);
		int getSectorType(MuonCalib::MuonFixedId id);
	private:
		LowPtTrigger* trig;
		double globIneff;
		int seed;
		void loadMapping(char path[]);
};
#endif // INEFFICIENT_TRIGGER_H
