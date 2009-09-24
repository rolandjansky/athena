/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RpcLogicSimTool/InefficientTrigger.h"
#include "TrigT1RpcLogicSimTool/LowPtTrigger.h"
#include "TrigT1RpcLogicSimTool/RpcEfficiency.h"

InefficientTrigger::InefficientTrigger(LowPtTrigger* t, double globalInefficiency, int s)
{
	globIneff = globalInefficiency;
	trig = t;
	seed = s;
	srand(seed);
}

bool InefficientTrigger::shouldTrigger(vector<unsigned int> hit_ids)
{
	return trig->trigger(hit_ids);
}

vector<unsigned int> InefficientTrigger::removeHits(vector<unsigned int> hit_ids)
{
	int size = hit_ids.size();
	vector<unsigned int> ret(0);

		
	for(int i=0; i<size; i++)
	{
		unsigned int curr = hit_ids.at(i);
		
			
		if(!isNotUsableRPC(curr))
		{
			ret.push_back(curr);
		}	
	}
	
	return ret;
}

vector<unsigned int> InefficientTrigger::removeHitsWithHardware(vector<unsigned int> hit_ids)
{
	int size = hit_ids.size();
	
	vector<unsigned int> ret(0);
	
	for(int i=0; i<size; i++)
	{
		unsigned int curr = hit_ids.at(i);
		
		if(!isNotUsableRPCWithHardware(curr, globIneff))
		{	
			ret.push_back(curr);
		}
	}
	
	return ret;
}


bool InefficientTrigger::doesTrigger(vector<unsigned int> hit_ids)
{
	vector<unsigned int> new_hits(0);
	new_hits = removeHitsWithHardware(hit_ids);
	
	return shouldTrigger(new_hits);	
}

int InefficientTrigger::getSectorType(MuonCalib::MuonFixedId id)
{
	return trig->getSectorType(id);
}
