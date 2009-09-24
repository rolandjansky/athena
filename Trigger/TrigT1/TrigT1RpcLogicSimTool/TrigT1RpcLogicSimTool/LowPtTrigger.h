/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LOW_PT_TRIGGER_H
#define LOW_PT_TRIGGER_H

#include "MuonCalibIdentifier/MuonFixedId.h"
#include <vector>

using std::vector;

class TriggerAlgorithm;

class LowPtTrigger{

	public:
		LowPtTrigger(int road_width);
		~LowPtTrigger();
		bool trigger(vector<unsigned int> hit_ids);
		int getRPC1(int num);
		int getRPC2(int num);
		int getRPC3(int num);
		int getRPCNum(MuonCalib::MuonFixedId id);
		int getSectorType(MuonCalib::MuonFixedId id);
		
	private:
		TriggerAlgorithm* algo1;
		TriggerAlgorithm* algo2;			

};
#endif // LOW_PT_TRIGGER_H
